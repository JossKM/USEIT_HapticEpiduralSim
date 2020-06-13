﻿using System.Collections;
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
        ObjectiveManager.Instance.OnStartObjective.AddListener(UpdateObjectiveText);
        UpdateObjectiveText();
    }

    // Update is called once per frame
    void Update()
    {
    }

    void UpdateObjectiveText()
    {
        Objective obj = ObjectiveManager.Instance.GetCurrentObjective();

        objectiveText.text = "Objective: " + obj.GetName() +
            "\n" + obj.GetDescription();
    }
}
