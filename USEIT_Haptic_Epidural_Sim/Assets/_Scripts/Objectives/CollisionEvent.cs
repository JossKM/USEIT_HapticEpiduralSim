using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class CollisionEvent : MonoBehaviour
{
    public static Action<GameObject> OnCollisionAction;

    private void OnCollisionEnter(Collision collision)
    {
        Debug.Log("Collision");
        OnCollisionAction(collision.transform.gameObject);
    }
}
