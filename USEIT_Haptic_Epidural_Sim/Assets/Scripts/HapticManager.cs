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
   
    private Vector3 originalPosition;

    // for debugging. will scale accorfing to the chai3d haptic world scale
    public GameObject workspaceVisualizer;

    // shows the device or "goal" position. Different from the proxy position reflected in this script's parent gameObject
    public GameObject devicePositionGameObject;

    [SerializeField]
    uint numHapticPoints = 12;

    [SerializeField]
    Transform[] proxyPositionVisualizerGameObjects;

    // Use this for initialization
    private void Awake()
    {
        if (!useHaptic)
            isHapticAvail = false;

		try
		{
        isHapticAvail = HapticNativePlugin.prepareHaptics(worldScale);

        if (workspaceVisualizer != null)
        {
            workspaceVisualizer.transform.localScale = new Vector3((float)worldScale, (float)worldScale, (float)worldScale) * 2.0f;
        }

		if(isHapticAvail)
		{
			HapticNativePlugin.startHaptics();
		}
		} catch(Exception e)
		{
			Debug.Log("ERROR when starting haptics: " + e.ToString());
			Application.Quit();
		}
    }

    private void Start()
    {
        HapticNativePlugin.setToolRadius(toolRadius);
    }

    private void OnDestroy()
    {
        if (isHapticAvail)
            HapticNativePlugin.stopHaptics();
    }

    private void Update()
    {
        if (isHapticAvail)
        {
            //transform.localPosition = HapticNativePlugin.GetProxyPosition();

            if (devicePositionGameObject != null)
            {
                devicePositionGameObject.transform.localPosition = HapticNativePlugin.GetDevicePosition();
                devicePositionGameObject.transform.localScale = new Vector3((float)toolRadius, (float)toolRadius, (float)toolRadius) * 2.0f;
            }

            Vector3[] positions = HapticNativePlugin.GetAllProxyPositions(numHapticPoints);
            //transform.localPosition = positions[0];
            for (int i = 0; i < numHapticPoints; i++)
            {
                proxyPositionVisualizerGameObjects[i].localPosition = positions[i];
            }
        }
    }
}