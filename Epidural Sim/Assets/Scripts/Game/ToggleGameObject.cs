///Swaps between materials

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleGameObject : MonoBehaviour
{
    [SerializeField]
    UnityEngine.GameObject toggleGameObject;

    [SerializeField]
    string enableEventName = "Disable";
    [SerializeField]
    string disableEventName = "Enable";

    private void OnEnable()
    {
        EventManager.StartListening(enableEventName, EnableObject);
        EventManager.StartListening(disableEventName, DisableObject);
    }

    private void OnDisable()
    {
        EventManager.StopListening(enableEventName, EnableObject);
        EventManager.StopListening(disableEventName, DisableObject);
    }

    public void SetGameObject(bool active)
    {
        toggleGameObject.SetActive(active);
    }

    public void Toggle()
    {
        toggleGameObject.SetActive(!toggleGameObject.activeInHierarchy);
    }

    public void EnableObject()
    {
        SetGameObject(true);
    }

    public void DisableObject()
    {
        SetGameObject(false);
    }

}
