using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ObjectiveUI : MonoBehaviour
{
    [SerializeField]
    TextMeshProUGUI objectiveText;
    

    // Start is called before the first frame update
    void Start()
    {
        ObjectiveManager.OnAdvanceObjectives += UpdateObjectiveText;
        UpdateObjectiveText();
    }

    // Update is called once per frame
    void Update()
    {
    }

    void UpdateObjectiveText()
    {
        Objective obj = ObjectiveManager.instance.GetCurrentObjective();

        objectiveText.text = "Objective: " + obj.GetName() +
            "\n" + obj.GetDescription();
    }
}
