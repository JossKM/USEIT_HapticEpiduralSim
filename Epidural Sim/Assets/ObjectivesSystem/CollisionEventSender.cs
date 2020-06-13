/// Utility behaviour representing a collision "sentinel" that will pass on collision events

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class CollisionEventSender : MonoBehaviour
{
    [SerializeField]
    public UnityEvent<Collision> OnCollisionAction;

    private void OnCollisionEnter(Collision collision)
    {
        Debug.Log("Collision: " + gameObject.name + " & " + collision.collider);
        OnCollisionAction.Invoke(collision);
    }
}
