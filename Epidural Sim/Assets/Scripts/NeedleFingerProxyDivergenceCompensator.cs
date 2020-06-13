using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NeedleFingerProxyDivergenceCompensator : MonoBehaviour
{
    [SerializeField]
    HapticAxialConstraint constraint;

    [Range(0.000001f, 0.1f)]
    [SerializeField]
    float maxDist = 0.01f;
    [SerializeField]
    float minVal = 0.02f;
    [SerializeField]
    float maxVal = 0.8f;

    // Update is called once per frame
    void Update()
    {
        if(HapticManager.isHapticAvail)
        {
        float dist = Vector3.Distance(HapticNativePlugin.GetDevicePosition(), HapticNativePlugin.GetProxyPosition());
            float interpVal = Mathf.InverseLerp(maxDist, 0.0f, dist);
            constraint.orientationWeight = Mathf.Clamp(interpVal, minVal, maxVal);
        }
    }
}
