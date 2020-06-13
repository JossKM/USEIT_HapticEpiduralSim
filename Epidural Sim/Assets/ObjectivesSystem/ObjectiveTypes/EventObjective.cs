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

    public override void Begin()
    {
        base.Begin();
        EventManager.StartListening(eventName, Complete);
    }

    public override void Complete()
    {
	if(state == PROGRESS_STATE.IN_PROGRESS)
	{
        	base.Complete();
        	EventManager.StopListening(eventName, Complete);
	}
    }
    
}
