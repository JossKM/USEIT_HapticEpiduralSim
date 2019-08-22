using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class CollisionObjective : Objective
{
    [SerializeField]
    [Tooltip("To complete this Objective, the Object Of Interest must touch another object")]
    GameObject objectOfInterest;

    [SerializeField]
    [Tooltip("If this is set to true, then the Object Of Interest needs to touch one of the collisionObjectives, else it just needs to touch anything")]
    bool specificObject = true;

    [SerializeField]
    List<GameObject> collisionObjectives;

    // Start is called before the first frame update
    void Start()
    {
        objectOfInterest.AddComponent<CollisionEvent>();
        CollisionEvent.OnCollisionAction += this.HandleEvent;
    }

    public override void HandleEvent<GameObject>(GameObject collidedWith)
    {
        if (specificObject)
        {
            for (int i = 0; i < collisionObjectives.Count; i++)
            {
                if (collisionObjectives[i].Equals(collidedWith))
                {
                    CompleteObjective();
                    return;
                }
            }
        }

        else
        {
            CompleteObjective();
        }
    }
}
