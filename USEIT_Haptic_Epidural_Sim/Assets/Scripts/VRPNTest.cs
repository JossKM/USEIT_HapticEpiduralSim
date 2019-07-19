using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UVRPN.Core;

public class VRPNTest : MonoBehaviour
{
    [SerializeField]
    VRPN_Analog mousePosition;

    [SerializeField]
    VRPN_Button keyboard;

    public void TestFunc()
    {
        gameObject.transform.Translate(0.0f, 1.0f, 0.0f);
    }
}
