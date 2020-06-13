using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

public class CollisionObjective : Objective
{
    [SerializeField]
    [Tooltip("To complete this Objective, the Object Of Interest must touch a collisionObjective")]
    GameObject objectOfInterest;

    [SerializeField]
    [Tooltip("What the objectOfInterest can touch to complete the objective")]
    List<GameObject> collisionObjectives;

    // Start is called before the first frame update
    void Start()
    {
        CollisionEventSender eventSender = objectOfInterest.GetComponent<CollisionEventSender>();

        // if there is no sentinel object, add one
        if(eventSender == null)
        {
            eventSender = objectOfInterest.AddComponent<CollisionEventSender>();
        }

        eventSender.OnCollisionAction.AddListener(OnCollision);
    }

    public void OnCollision(Collision collision)
    {
        if (collisionObjectives.Contains(collision.gameObject))
        {
            Complete();
        }
    }
}
