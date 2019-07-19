using System.Collections;
using System;
using System.Collections.Generic;
using UnityEngine;

public class NumberedObjective : Objective
{
    [SerializeField]
    Objective countedObjective;

    [SerializeField]
    int timesToCount;

    int counter = 0;

    // Start is called before the first frame update
    void Start()
    {
        OnObjectiveComplete += this.HandleEvent;
    }

    public override void HandleEvent<Objective>(Objective o)
    {
        if (o.Equals(countedObjective))
        {
            counter++;

            if (counter >= timesToCount)
            {
                CompleteObjective();
            }
        }            
    }
}
