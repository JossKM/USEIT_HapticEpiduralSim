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

    //[SerializeField]
    //private int currentObjective = 0;

    [SerializeField]
    Objective currentObjective = null;

    //public static Action OnAdvanceObjectives;
    private static ObjectiveManager objectiveManager;

    public UnityEvent OnStartObjective;
    public UnityEvent OnCompleteObjective;

    ObjectiveUIController controller;


    public static ObjectiveManager Instance
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
        // Get all objectives from gameobject (in order) and add them to the list. Also set the begin condition of each one to the end condition of the last
        BuildObjectiveOrder(objectiveHolderGameObject);
    }

    //recursive call to make each objective start after one another based on the order in the heirarchy. Returns null if there are no sub-objectives, else returns the last objective in the sequence (i.e. the end)
    Objective BuildObjectiveOrder(Transform objectiveParent)
    {
        Objective previousObjective = null;
        Objective objective = null;

        // check all children of the objective
        for (int i = 0; i < objectiveParent.childCount; i++)
        {
            objective = objectiveParent.GetChild(i).GetComponent(typeof(Objective)) as Objective;

            // if an objective was in the list
            if (objective != null && objective.isActiveAndEnabled)
            {
                // set hooks for all objectives starting and ending
                objective.OnObjectiveBegin.AddListener(OnStartObjective.Invoke);
                objective.OnObjectiveComplete.AddListener(OnCompleteObjective.Invoke);

                // build out child heirarchy
                Objective subObjective = BuildObjectiveOrder(objective.transform);
                if (subObjective != null) previousObjective = subObjective;

                // link objectives
                if (previousObjective != null)
                {
                    previousObjective.OnObjectiveComplete.AddListener(objective.Begin);
                    Debug.Log("Objective [" + objective.GetName() + "] set to start when [" + previousObjective .GetName() + "] finishes");
                }

                objectives.Add(objective);

                previousObjective = objective;
            }
        }

        return objective;
    }

    private void Awake()
    {
        ObjectiveManager.Instance.StartTrackingObjectives();
    }

    public void StartTrackingObjectives()
    {
        //foreach(Objective o in objectives)
        //{
        //    o.OnObjectiveComplete += CheckObjectives;
        //}
        //if (currentObjective < objectives.Count)
        //{
        //    objectives[currentObjective].OnBegin();
        //}

        if(currentObjective != null)
        {
            currentObjective.Begin();
        }
    }

    public void SetCurrentObjective(Objective objective)
    {
        currentObjective = objective;
//        if (currentObjective + 1 < objectives.Count)
//        {
//            if (objectives[currentObjective].Equals(objective))
//            {
//                currentObjective++;
//                OnCompleteObjective.Invoke();
//
//#if UNITY_EDITOR
//                Debug.Log("Objective Complete:" + objective.GetName());
//#endif
//                // invoke new objective's on begin function
//                objectives[currentObjective].OnBegin();
//                OnStartObjective.Invoke();
//
//#if UNITY_EDITOR
//                Debug.Log("Started objective: " + objectives[currentObjective].GetName());
//#endif
//            }
//
//        }
    }

    public Objective GetCurrentObjective()
    {
        return currentObjective;
        //if (currentObjective < objectives.Count)
        //{
        //    return objectives[currentObjective];
        //}
        //else
        //{
        //    return null;
        //}
    }
}


