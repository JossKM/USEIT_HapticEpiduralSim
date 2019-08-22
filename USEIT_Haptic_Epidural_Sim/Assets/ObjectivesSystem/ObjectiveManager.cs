using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class ObjectiveManager : MonoBehaviour
{
    [Tooltip("A gameobject that holds objective objects. All children of the object should have Objective components and be in desired order")]
    [SerializeField]
    Transform objectiveHolderGameObject;
    
    /// <summary>
    /// list generated from objectives in objectiveHolderGameObject
    /// </summary>
    List<Objective> objectives = new List<Objective>();

    [SerializeField]
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

    public void Init()
    {

        // Get all objectives from gameobject (in order) and add them to the list.
        for( int i = 0; i < objectiveHolderGameObject.childCount; i++)
        {
            var objective = objectiveHolderGameObject.GetChild(i).GetComponent<Objective>();
            if(objective != null)
            {
                objectives.Add(objective);
            }
        }
    }

    private void Start()
    {
        StartTrackingObjectives();
    }

    public void StartTrackingObjectives()
    {
        foreach(Objective o in objectives)
        {
            o.OnObjectiveComplete += CheckObjectives;
        }

        if (currentObjective < objectives.Count)
        {
            objectives[currentObjective].OnBegin();
        }
    }

    public void CheckObjectives(Objective objective)
    {
        if (currentObjective + 1 < objectives.Count)
        {
            if (objectives[currentObjective].Equals(objective))
            {
                currentObjective++;
                Debug.Log("Objective Complete:" + objective.GetName());

                // invoke new objective's on begin function
                objectives[currentObjective].OnBegin();

                OnAdvanceObjectives();
            }

        }
    }

    public Objective GetCurrentObjective()
    {
        if (currentObjective < objectives.Count)
        {
            return objectives[currentObjective];
        }
        else
        {
            return null;
        }
    }
}


