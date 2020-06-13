using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using UnityEngine.Events;

public class CheckForPunctureState : MonoBehaviour
{
    [SerializeField]
    string newLayerMessage = "New Layer Penetrated";
    [SerializeField]
    string penetratedLayerKey = "Penetrated ";
    [SerializeField]
    string leftLayerKey = "Left ";

    [SerializeField]
    public UnityEvent OnLayerChange;

    [SerializeField]
    public UnityEvent OnLeaveLayer;

    [SerializeField]
    public UnityEvent OnEnterLayer;

    [SerializeField]
    public UnityEvent OnLockRotation;

    [SerializeField]
    public UnityEvent OnUnlockRotation;

    [SerializeField]
    public UnityEvent OnFirstPuncture;

    [SerializeField]
    public UnityEvent OnNoPunctures;

    [SerializeField]
    string lockRotationEvent = "Lock Rotation";
    [SerializeField]
    string unlockRotationEvent = "Unlock Rotation";

    [SerializeField]
    TextMeshProUGUI LayerInfoUI;

    // List<PenetrableMaterial> puncturedLayers;

    // ID number of the last layer that was entered, to determine when a new layer was entered
    //int lastLayerPenetratedID = -1;
    PenetrableMaterial lastLayer = null;

    // index of the last layer entered
    int lastLayerDepth = -1;

    // Start is called before the first frame update
    void Update()
    {
        CheckForNewPuncture();
    }

    string GetLayerPenetrated()
    {
        return lastLayer.m_name;
    }

    void CheckForNewPuncture()
    {
        //if (HapticNativePlugin.isPatientPenetrated())
        //{
        PenetrableMaterial currentLayer = PenetrableMaterial.GetLayer(HapticNativePlugin.getLastLayerPenetratedID());
        int currentLayerDepth = HapticNativePlugin.getCurrentLayerDepth();

        if (currentLayer != lastLayer)
        {
            // send layer change event
            OnLayerChange.Invoke();
            EventManager.TriggerEvent(newLayerMessage);

            // leaving a layer i.e. needle moving outwards
            if (currentLayerDepth < lastLayerDepth)
            {
                // send last layer event
                EventManager.TriggerEvent(leftLayerKey + lastLayer.m_name);
                OnLeaveLayer.Invoke();

                if (currentLayer != null)
                {

                    /////////////////   !!!!!!!!   /////////////////
                    // unlock rotation if entering an unlocked layer
                    // POTENTIAL BUG here, because this check does not cover all possible cases! 
                    // e.g. if you go from a layer that is locked to a new layer further in that is not locked. In that case you should still be locked, but this code will unlock you
                    /////////////////   !!!!!!!!   /////////////////
                    if (lastLayer.m_locksRotation && !currentLayer.m_locksRotation)
                    {
                        // last layer should never be null because you cannot go lower than -1 current layer depth
                        EventManager.TriggerEvent(unlockRotationEvent);
                        OnUnlockRotation.Invoke();
                    }
                }
                else
                {
                    // entered air therefore unlock
                    OnUnlockRotation.Invoke();
                    OnNoPunctures.Invoke();
                }

            }
            else
            {
                // entering a further layer i.e. needle moving inwards
                if (currentLayer != null)
                {
                    if (lastLayer == null)
                    {
                        // if going from nothing to something then that is a puncture. Send the event
                        OnFirstPuncture.Invoke();
                    }


                    // send new layer event
                    EventManager.TriggerEvent(penetratedLayerKey + currentLayer.m_name);
                    OnEnterLayer.Invoke();

                    // send lock rotation event
                    if (currentLayer.m_locksRotation)
                    {
                        EventManager.TriggerEvent(lockRotationEvent);
                        OnLockRotation.Invoke();
                    }
                }
            }


            // update info UI
            if (LayerInfoUI != null)
            {
                if (currentLayer != null)
                {
                    LayerInfoUI.text = currentLayer.m_name;
                }
                else
                {
                    LayerInfoUI.text = "";
                }
            }

            lastLayerDepth = currentLayerDepth;
            lastLayer = currentLayer;
        } // if (currentLayer != lastLayer)
    } // CheckForNewPuncture


}
