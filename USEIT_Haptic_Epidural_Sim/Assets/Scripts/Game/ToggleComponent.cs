///Swaps between materials

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleComponent : MonoBehaviour
{
    [SerializeField]
    Behaviour toToggle;

    [SerializeField]
    string enableEventName = "Disable";
    [SerializeField]
    string disableEventName = "Enable";

    private void OnEnable()
    {
        EventManager.StartListening(disableEventName, DisableObject);
        EventManager.StartListening(enableEventName, EnableObject);
    }

    private void OnDisable()
    {
        EventManager.StopListening(disableEventName, DisableObject);
        EventManager.StopListening(enableEventName, EnableObject);
    }

    public void SetEnabled(bool active)
    {
        toToggle.enabled = active; //SetActive(active);
    }

    public void Toggle()
    {
        toToggle.enabled = (!toToggle.enabled);
    }

    public void DisableObject()
    {
        SetEnabled(true);
    }

    public void EnableObject()
    {
        SetEnabled(false);
    }
}
