﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FluidResistance : MonoBehaviour
{

    PenetrableMaterial lastLayerPenetrated;

    // Arduino communication for fluid resistance
    [SerializeField]
    ArduinoFluidResistance arduino;

    //[SerializeField]
    //string eventNameToListenFor = "New Layer Penetrated";
    //
    //private void Start()
    //{
    //    EventManager.StartListening(eventNameToListenFor, OnNewLayerPenetrated);
    //}
    
    public void OnLayerChange()
    {
        if (HapticNativePlugin.isPatientPenetrated())
        {
            PenetrableMaterial lastLayerPenetrated = PenetrableMaterial.GetLayer(HapticNativePlugin.getLastLayerPenetratedID());
            arduino.SetResistance((float)lastLayerPenetrated.m_fluidResistance);
        }
        else
        {
            lastLayerPenetrated = null;

            arduino.SetResistance(0.0f);
            //Debug.Log ("not resisting");
        }
    }
}
