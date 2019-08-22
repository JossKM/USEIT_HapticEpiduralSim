using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public abstract class Objective : MonoBehaviour
{
    public event Action<Objective> OnObjectiveComplete;

    [SerializeField]
    string objectiveName;

    [SerializeField]
    [Multiline]
    string description;

    [SerializeField]
    GameObject objectiveMarker = null;

    public void CompleteObjective()
    {
        OnComplete();
        OnObjectiveComplete(this);
    }

    // override this
    public virtual void OnBegin()
    {
        if (objectiveMarker != null)
        {
            objectiveMarker.SetActive(true);
        }
    }

    // override this
    public virtual void OnProgress()
    {

    }

    // override this
    public virtual void OnComplete()
    {
        if (objectiveMarker != null)
        {
            objectiveMarker.SetActive(false);
        }
    }

    public abstract void HandleEvent<T>(T obj);

    public string GetName()
    {
        return objectiveName;
    }

    public string GetDescription()
    {
        return description;
    }
}
