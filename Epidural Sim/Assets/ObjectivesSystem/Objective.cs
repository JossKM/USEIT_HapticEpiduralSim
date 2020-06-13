using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public abstract class Objective : MonoBehaviour
{
    protected enum PROGRESS_STATE
    {
        NOT_STARTED,
        IN_PROGRESS,
        FAILED,
        COMPLETE
    }

    [SerializeField]
    public UnityEvent OnObjectiveBegin;
    [SerializeField]
    public UnityEvent OnObjectiveComplete;

    [SerializeField]
    string objectiveName;

    [SerializeField]
    [Multiline]
    string description;

    [SerializeField]
    protected PROGRESS_STATE state = PROGRESS_STATE.NOT_STARTED;

    [SerializeField]
    UnityEngine.GameObject objectiveMarker = null;
    
    // override this
    public virtual void Begin()
    {
        //#if UNITY_EDITOR
        //        Debug.Log("Started objective: " + objectiveName);
        //#endif

        if(state != PROGRESS_STATE.IN_PROGRESS)
        {
            state = PROGRESS_STATE.IN_PROGRESS;
            if (objectiveMarker != null)
            {
                objectiveMarker.SetActive(true);
            }
        }
        ObjectiveManager.Instance.SetCurrentObjective(this);
        OnObjectiveBegin.Invoke();
    }

    // override this
    public virtual void Progress()
    {

    }

    public virtual void Complete()
    {
        if (state != PROGRESS_STATE.COMPLETE && state != PROGRESS_STATE.NOT_STARTED)
        {
            state = PROGRESS_STATE.COMPLETE;

            if (objectiveMarker != null)
            {
                objectiveMarker.SetActive(false);
            }
        }

        OnObjectiveComplete.Invoke();
    }
    

    public string GetName()
    {
        return objectiveName;
    }

    public string GetDescription()
    {
        return description;
    }
}
