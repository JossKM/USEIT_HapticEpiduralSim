using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class CheckForPunctureState : MonoBehaviour
{

    [SerializeField]
    string eventStringToSend = "New Layer Penetrated";
    [SerializeField]
    string penetratedLayerKey = "Penetrated ";
    [SerializeField]
    string leftLayerKey = "Left ";


    [SerializeField]
    string lockRotationEvent = "Lock Rotation";
    [SerializeField]
    string unlockRotationEvent = "Unlock Rotation";

    [SerializeField]
    TextMeshProUGUI LayerInfoUI;

   // List<PenetrableMaterial> puncturedLayers;

    // ID number of the last layer that was entered, to determine when a new layer was entered
    int lastLayerEnteredID = -1;

    // Start is called before the first frame update
    void Update()
    {
        CheckForNewPuncture();
    }

    void CheckForNewPuncture()
    {
        //if (HapticNativePlugin.isPatientPenetrated())
        //{
        int newLayerPenetratedID = HapticNativePlugin.getLastLayerPenetratedID();
        if (newLayerPenetratedID != lastLayerEnteredID)
        {
            PenetrableMaterial lastLayer = PenetrableMaterial.GetLayer(lastLayerEnteredID);
            PenetrableMaterial newLayer = PenetrableMaterial.GetLayer(newLayerPenetratedID);

            // send layer change event
            EventManager.TriggerEvent(eventStringToSend);
            
            if (lastLayer != null)
            {
                // send last layer event
                EventManager.TriggerEvent(leftLayerKey + lastLayer.m_name);



                // unlock rotation if entering an unlocked layer
                // potential bug here, because this check does not cover all possible cases! 
                // e.g. if you go from a layer that is locked to a new layer further in that is not locked. In that case you should still be locked, but this code will unlock you
                if(lastLayer.m_locksRotation && newLayer != null && !newLayer.m_locksRotation)
                {
                    EventManager.TriggerEvent(unlockRotationEvent);
                }
            }

            if (newLayer != null)
            {
                // send new layer event
                EventManager.TriggerEvent(penetratedLayerKey + newLayer.m_name);

                // send lock rotation event
                if (newLayer.m_locksRotation)
                {
                    EventManager.TriggerEvent(lockRotationEvent);
                }
            } else
            {
                EventManager.TriggerEvent(unlockRotationEvent);
            }

            // update info UI
            if (LayerInfoUI != null)
            {
                LayerInfoUI.text = PenetrableMaterial.GetNameOfLayer(newLayerPenetratedID);
            }
        }

        lastLayerEnteredID = newLayerPenetratedID;
        //}
        //else
        //{
        //    lastLayerEnteredID = -1;
        //}
    }


}
