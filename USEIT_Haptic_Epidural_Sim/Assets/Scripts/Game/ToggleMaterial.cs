///Swaps between materials

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToggleMaterial : MonoBehaviour
{
    [SerializeField]
    Material alternateMaterial;
    [SerializeField]
    Material defaultMaterial;

    [SerializeField]
    string useAlternateEventName = "Penetrated Skin";
    [SerializeField]
    string useOriginalEventName = "Left Skin";

    [SerializeField]
    Renderer renderer;

    [SerializeField]
    bool useAlternate = false;

    private void OnEnable()
    {
        EventManager.StartListening(useAlternateEventName, UseAlternateMaterial);
        EventManager.StartListening(useOriginalEventName, UseDefaultMaterial);
        UseAlternateMaterial(useAlternate);
    }

    private void OnDisable()
    {
        EventManager.StopListening(useAlternateEventName, UseAlternateMaterial);
        EventManager.StopListening(useOriginalEventName, UseDefaultMaterial);
    }

        public void UseAlternateMaterial(bool useAlt)
    {
        useAlternate = useAlt;
        if (useAlternate)
        {
            renderer.material = alternateMaterial;
        }
        else
        {
            renderer.material = defaultMaterial;
        }
    }

    public void Toggle()
    {
        UseAlternateMaterial(!useAlternate);
    }

    public void UseAlternateMaterial()
    {
        UseAlternateMaterial(true);
    }

    public void UseDefaultMaterial()
    {
        UseAlternateMaterial(false);
    }
}
