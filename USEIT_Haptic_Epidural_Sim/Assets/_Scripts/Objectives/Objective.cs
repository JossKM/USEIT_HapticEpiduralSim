using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public abstract class Objective : MonoBehaviour
{
    public static event Action<Objective> OnObjectiveComplete;

    [SerializeField]
    string objectiveName;

    [SerializeField]
    [Multiline]
    string description;

    public void CompleteObjective()
    {
        OnObjectiveComplete(this);
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
