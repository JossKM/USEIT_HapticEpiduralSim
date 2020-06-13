using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class ObjectiveUIController : MonoBehaviour
{
    [SerializeField]
    Canvas objectiveCanvas;

    [SerializeField]
    TextMeshProUGUI objectiveName;

    [SerializeField]
    TextMeshProUGUI objectiveDescription;

    [SerializeField]
    ObjectiveManager objectiveManager;

    // Start is called before the first frame update
    void Awake()
    {
        ObjectiveManager.Instance.OnStartObjective.AddListener(UpdateObjectiveText);
        UpdateObjectiveText();
    }


    private void UpdateObjectiveText()
    {
        Objective currentObjective = objectiveManager.GetCurrentObjective();

        string name = "";
        string desc = "";

        if(currentObjective != null)
        {
            name = currentObjective.GetName();
            desc = objectiveManager.GetCurrentObjective().GetDescription();
        }

        objectiveName.text = name;
        objectiveDescription.text = desc;
    }
}
