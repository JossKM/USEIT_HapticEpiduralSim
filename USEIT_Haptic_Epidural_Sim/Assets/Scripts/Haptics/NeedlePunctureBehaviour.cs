using UnityEngine;
using System.Collections.Generic;

public struct NeedlePunctureData
{
    public Vector3 entryPoint;
    public Vector3 entrySurfaceNormal;
}

public class NeedlePunctureBehaviour : MonoBehaviour
{
    // Properties /////////////////////////////////////////////////
    [SerializeField]
    float needleLength = 0.1f;

    // how far ahead to cast rays beyond the needle itself
    [SerializeField]
    //float castAheadLength = 0.01f;

    //public HapticAxialConstraint axialConstraint;
    NeedlePunctureData punctureData;

    // applies when penetrating
    [SerializeField]
    public HapticAxialConstraint constraint;

    [SerializeField]
    LayerMask needleLayerMask;
    
    public float needleDepth; // displacement from entry point to tip (or exit point)


    [SerializeField]
    GameObject needlePenetrationPointPrefab;

    List<GameObject> needlePenetrationPoints = new List<GameObject>();
    
    private LinkedList<RaycastHit> sortedHits;


    [Header("PID properties")]
    [SerializeField]
    public double kp = 0.8d;
    [SerializeField]
    public double ki = 0.2d;
    [SerializeField]
    public double kd = 0.5d;
    [SerializeField]
    public double gain = 0.4d;


    // Use this for initialization
    void Start()
    {
        // stop if haptic manager is not enabled
        if (!HapticManager.isHapticAvail)
        {
            gameObject.SetActive(false);
            return;
        }

        needleDepth = -1.0f;
    }

    // Update is called once per frame
    void Update()
    {
        HapticNativePlugin.setNeedlePID(kp, ki, kd, gain);

        Vector3 dir = transform.forward; //transform.TransformDirection(Vector3.forward);

        float len = needleLength;// + castAheadLength;
        Ray newRay = new Ray(transform.position - dir * needleLength, dir);

        RaycastLayers(newRay, len);
    }

    void RaycastLayers(Ray needleRay, float distance)
    {
        ////////////////////////////

        //raycast to determine what is in the needle's path
        RaycastHit[] hits = Physics.RaycastAll(needleRay, distance, needleLayerMask);

        //ensure only haptic objects are evaluated, and sort them by distance
        sortedHits = SortAndFilterPenetrations(hits);
        int numLayers = sortedHits.Count;

        // Tell plugin how many layers were encountered
        HapticNativePlugin.setPatientNumLayersToUse(numLayers);

        // iterate through layers, in order of closest to farthest
        var iterator = sortedHits.First;

        //if (sortedHits.Count <= 0) // if not touching anything
        //{
        //    constraint.SetEnabled(false);
        //}

        if (numLayers > 0)
        {
            // For each layer in order of raycast
            ////////////////////////////
            for (int layerDepthIdx = 0; layerDepthIdx < numLayers; layerDepthIdx++)
            {
                RaycastHit hit = iterator.Value;

                float layerDepth;

                if (layerDepthIdx == 0) // for the first collision
                {
                    layerDepth = 0.0f;

                    ////// they ray casts a little farther than the needle itself
                    if ((hit.distance) < (needleLength)) // if needle has hit the skin
                    {
                        if (needleDepth < 0.0) 
                        {
                            SetPuncturePoint(hit.point, hit.normal);
                        }
                    } else
                    {
                        UnPuncture();
                    }
                    ////
                }
                else
                {
                    layerDepth = Vector3.Distance(hit.point, (punctureData.entryPoint));  // depth of penetration from the first contact point
                }


                // Send new layer data to plugin
                PenetrableMaterial material = hit.collider.gameObject.GetComponent<PenetrableMaterial>();

                HapticNativePlugin.setHapticLayerProperties(
                   layerDepthIdx, // depth order
                   material.GetID(),
                   material.m_stiffness,
                   material.m_stiffnessExponent,
                   material.m_maxFrictionForce,
                   material.m_penetrationThreshold,
                   material.m_malleability,
                   layerDepth);

                //
                // generate and update penetration point visualizer prefabs
                if (needlePenetrationPointPrefab != null)
                {
                    if (numLayers > needlePenetrationPoints.Count)
                    {
                        needlePenetrationPoints.Add(Instantiate(needlePenetrationPointPrefab, hit.point, Quaternion.LookRotation(hit.normal)));
                    }
                    else
                    {
                        needlePenetrationPoints[layerDepthIdx].transform.SetPositionAndRotation(hit.point, Quaternion.LookRotation(hit.normal));
                    }
                    //
                    //
                }

                // to next raycast
                iterator = iterator.Next;
            }

            needleDepth = Vector3.Distance(transform.position, punctureData.entryPoint);

            #if UNITY_EDITOR
            {
                // debug drawing
                {
                    Debug.DrawRay(needleRay.origin, needleRay.direction * needleLength, Color.green);
                }

                // debug drawing
                {
                    // look-ahead portion
                    Debug.DrawLine(needleRay.origin,
                        needleRay.origin + (needleRay.direction * (needleLength)),
                        Color.yellow);

                    Vector3 entryPoint = punctureData.entryPoint;

                    if (sortedHits.Count > 0)
                    {
                        // unpenetrating portion
                        Debug.DrawLine(needleRay.origin,
                        entryPoint,
                        Color.green);

                        // penetrating portion
                        Debug.DrawLine(entryPoint, //needleRay.origin + needleRay.direction * ((RaycastHit)sortedHits[0]).distance,
                            transform.position, //needleRay.direction * (float)punctureData.collisions[(punctureData.collisions.Length - 1)].depth, 
                            Color.red);

                        // surface normal
                        Debug.DrawRay(entryPoint,
                            punctureData.entrySurfaceNormal * 0.01f,
                            Color.cyan);
                    }
                }
            }
            #endif
        } // if hits.Length > 0
        else
        {
            UnPuncture();
        }

    } // method scope


    void SetPuncturePoint(Vector3 position, Vector3 direction)
    {
        punctureData.entryPoint = position;
        punctureData.entrySurfaceNormal = direction;

        constraint.transform.position = position;
        constraint.SetEnabled(true);
    }

    void UnPuncture()
    {
        needleDepth = -1.0f;
        constraint.SetEnabled(false);
    }


    // ensures only haptic objects are returned by the raycast, and also sorts them by distance
    LinkedList<RaycastHit> SortAndFilterPenetrations(RaycastHit[] hits)
    {
        LinkedList<RaycastHit> hitList = new LinkedList<RaycastHit>();

        float maxDistance = Mathf.NegativeInfinity;

        for (int i = 0; i < hits.Length; i++)
        {
            // if it is a haptic material, 
            if (hits[i].collider.gameObject.GetComponent<PenetrableMaterial>() != null)
            {
                // find its place, then add it to the list there

                if (hits[i].distance > maxDistance) // if more than the maximum, add it to the end of the list. Also handles the initial case, of adding the first node.
                {
                    hitList.AddLast(hits[i]);
                    maxDistance = hits[i].distance;
                }
                else
                {
                    var node = hitList.Last.Previous;

                    while (true)
                    {
                        if (node == null)
                        {
                            // it is the first hit. Add it at the beginning.
                            hitList.AddFirst(hits[i]);
                            break;
                        }
                        else
                        {
                            if (hits[i].distance >= node.Value.distance)
                            {
                                // the correct place for it is found. Add it here.
                                hitList.AddAfter(node, hits[i]);
                                break;
                            }
                        }

                        node = node.Previous;
                    }
                }
            }
        }

        return hitList;
    }
}
