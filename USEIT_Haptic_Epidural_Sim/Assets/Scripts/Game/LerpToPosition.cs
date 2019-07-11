using UnityEngine;
using System.Collections;

public class LerpToPosition : MonoBehaviour {
    public Transform target;

    public float tolerance = 0.0001f;
    public float lerpSpeed = 0.2f;

	// Use this for initialization
	//void Start () {
	//
	//}
	
	// Update is called once per frame
	void Update () {
	
        if(Vector3.Distance(transform.position, target.position) > tolerance)
        {
            transform.position = Vector3.Lerp(transform.position, target.position, lerpSpeed);
        } else
        {
            transform.position = target.position;
            this.enabled = false;
        }
	}
}
