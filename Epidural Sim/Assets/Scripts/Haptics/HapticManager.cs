using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.IO;
using System;
using System.Threading;

public class HapticManager : MonoBehaviour
{
    public double worldScale = 1.0d;
    public double toolRadius = 0.005d;

    public bool useHaptic;
    public static bool isHapticAvail;

    [SerializeField]
    public static Transform hapticOrigin;

    // for debugging. will scale accorfing to the chai3d haptic world scale
    public UnityEngine.GameObject workspaceVisualizer;

    // shows the device or "goal" position. Different from the proxy position reflected in this script's parent gameObject
    public UnityEngine.GameObject devicePositionGameObject;

    [Tooltip("haptic tool proxy position")]
    [SerializeField]
    Transform tool;


    int numHapticPoints = 12;

    [Tooltip("use generated objects to show all haptic points")]
    public bool visualizeHapticPoints;

    UnityEngine.GameObject[] hapticPointVisualizerGameObjects;

    [Tooltip("haptic point visualizer to generate")]
    [SerializeField]
    UnityEngine.GameObject hapticPointVisualizerPrefab;

    // Use this for initialization
    private void Awake()
    {
        hapticOrigin = transform;

        if (!useHaptic)
            isHapticAvail = false;

        try
        {
            Debug.Log("Haptic Manager starting simulation...");

            isHapticAvail = HapticNativePlugin.prepareHaptics(worldScale);

            if (workspaceVisualizer != null)
            {
                workspaceVisualizer.transform.localScale = new Vector3((float)worldScale, (float)worldScale, (float)worldScale) * 2.0f;
            }

            if (isHapticAvail)
            {
                HapticNativePlugin.startHaptics();
            }
            else
            {
                gameObject.SetActive(false);
                Debug.Log("Haptic simulation unable to start. Haptics disabled.");
                return;
            }
        }
        catch (Exception e)
        {
            Debug.Log("ERROR when starting haptics: " + e.ToString());
            Application.Quit();
        }
    }

    private void Start()
    {
        if (isHapticAvail)
        {
            HapticNativePlugin.setToolRadius(toolRadius);

            numHapticPoints = HapticNativePlugin.getNumHapticPoints();

            hapticPointVisualizerGameObjects = new UnityEngine.GameObject[numHapticPoints];
            for (int i = 0; i < numHapticPoints; i++)
            {
                hapticPointVisualizerGameObjects[i] = Instantiate(hapticPointVisualizerPrefab, transform);
            }

        }
    }

    private void OnDestroy()
    {
        HapticNativePlugin.stopHaptics();
    }

    private void Update()
    {

        if (devicePositionGameObject != null)
        {
            devicePositionGameObject.transform.localPosition = HapticNativePlugin.GetDevicePosition();
            devicePositionGameObject.transform.localScale = new Vector3((float)toolRadius, (float)toolRadius, (float)toolRadius) * 2.0f;
        }

        Vector3[] positions = HapticNativePlugin.GetAllProxyPositions((uint)HapticNativePlugin.getNumHapticPoints());
        tool.localPosition = positions[0];

        if (visualizeHapticPoints)
        {
            for (int i = 0; i < numHapticPoints; i++)
            {
                hapticPointVisualizerGameObjects[i].transform.localPosition = positions[i];
            }
        }
        else
        {
            transform.localPosition = HapticNativePlugin.GetProxyPosition();
        }
    }
}