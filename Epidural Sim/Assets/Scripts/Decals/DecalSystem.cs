/// <summary>
/// This singleton handles placement and replacement of decals within a specified limit. It also lets you register them to instantiate by key instead
/// </summary>
/// 

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DecalSystem : MonoBehaviour
{
    [SerializeField]
    int MAX_DECALS = 50;

    [Tooltip("Default distance to offset decals along the surface normal (to appear on top of the surface)")]
    [SerializeField]
    float SURFACE_NORMAL_OFFSET = 0.01f;

    [Tooltip("Visible only for debugging")]
    [SerializeField]
    Queue<GameObject> decalInstances;

    [Tooltip("Visible only for debugging")]
    [SerializeField]
    private List<GameObject> decalPrefabs;


    private static DecalSystem decalSysInstance;

    public static DecalSystem instance
    {
        get
        {
            if (!decalSysInstance)
            {
                decalSysInstance = FindObjectOfType(typeof(DecalSystem)) as DecalSystem;

                if (!decalSysInstance)
                {
                    Debug.LogError("There needs to be one active DecalSystem script on a GameObject in the scene.");
                }
                else
                {
                    decalSysInstance.Init();
                }
            }

            return decalSysInstance;
        }
    }

    void Init()
    {
        if (decalInstances == null)
        {
            decalInstances = new Queue<GameObject>();
        }

        if (decalPrefabs == null)
        {
            decalPrefabs = new List<GameObject>();
        }
    }

    /// <summary>
    /// Register a prefab as a decal that is used often, allowing DecalSystem to instantiate them by ID.
    /// </summary>
    /// <param name="decalPrefab"></param>
    /// <returns> integer ID assigned to the prefab </returns>
    public int RegisterDecalPrefab(GameObject decalPrefab)
    {
        string objectName = decalPrefab.name;
        var decal = decalPrefab.GetComponent<Decal>();
        decal.SetID(decalPrefabs.Count);
        decalPrefabs.Add(decalPrefab);

        return decal.prefabID;
    }

    /// <summary>
    /// Instantiates and queues decal to ensure that no more than MAX_DECALS are allowed to exist. Replaces oldest decals.
    /// </summary>
    /// <param name="decalPrefab"></param>
    /// <param name="position"></param>
    /// <param name="surfaceNormal"></param>
    public void PlaceDecal(GameObject decalPrefab, Vector3 position, Vector3 surfaceNormal)
    {
        if (decalInstances.Count >= MAX_DECALS)
        {
            // destroy oldest decal
            Destroy(decalInstances.Dequeue().gameObject);
        }

        // add new decal
        var decalInstance = Instantiate(decalPrefab, position + surfaceNormal * SURFACE_NORMAL_OFFSET, Quaternion.LookRotation(surfaceNormal));
        decalInstances.Enqueue(decalInstance);
    }


    /// <summary>
    /// places decal prefab by ID. will throw an exception if prefab was not registered prior
    /// </summary>
    /// <param name="decalID"></param>
    /// <param name="position"></param>
    /// <param name="surfaceNormal"></param>
    public void PlaceDecal(int decalID, Vector3 position, Vector3 surfaceNormal)
    {
        PlaceDecal(decalPrefabs[decalID], position, surfaceNormal);
    }
}
