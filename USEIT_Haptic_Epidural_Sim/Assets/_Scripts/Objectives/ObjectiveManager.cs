using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class ObjectiveManager : MonoBehaviour   
{
    [SerializeField]
    List<Objective> objectives;

    private int currentObjective = 0;

    public static Action OnAdvanceObjectives;
    private static ObjectiveManager objectiveManager;

    public static ObjectiveManager instance
    {
        get
        {
            //If an objective manager has not already been initialized 
            if (!objectiveManager)
            {
                //Find the objective manager in the scene
                objectiveManager = FindObjectOfType(typeof(ObjectiveManager)) as ObjectiveManager;

                //If there is no objective manager in the scene, complain 
                if (!objectiveManager)
                {
                    Debug.LogError("There needs to be one active ObjectiveManager script on a GameObject in your scene");
                }

                //Initialize the found objective manager
                else
                {
                    objectiveManager.Init();
                }
            }

            //Return the instance if it already exists
            return objectiveManager;
        }
    }
    private void Start()
    {
        Objective.OnObjectiveComplete += CheckObjectives;
    }

    public void Init()
    {
    }

    public void CheckObjectives(Objective objective)
    {
        if (objectives[currentObjective].Equals(objective))
        {
            Debug.Log(objective.GetName());
            currentObjective++;

            OnAdvanceObjectives();
        }
    }

    public Objective GetCurrentObjective()
    {
        return objectives[currentObjective];
    }
}


