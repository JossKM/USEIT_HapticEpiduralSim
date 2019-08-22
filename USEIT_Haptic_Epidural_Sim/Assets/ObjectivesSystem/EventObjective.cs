///
// This Objective listens for a string event
///

using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class EventObjective : Objective
{
    [SerializeField]
    string eventName;

    public override void OnBegin()
    {
        base.OnBegin();
        EventManager.StartListening(eventName, CompleteObjective);
    }

    public override void OnComplete()
    {
        base.OnComplete();
        EventManager.StopListening(eventName, OnComplete);
    }

    public override void HandleEvent<T>(T obj)
    {
        throw new NotImplementedException();
    }
}
