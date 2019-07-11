using UnityEngine;
using System.Collections;

// uses the object's position for the constraints. Only 1 may be active. Not to be attached to the proxy. 

public class HapticSpringConstraint : MonoBehaviour
{

    public bool constraintEnabled = false;
    public double minDist = 0.0d;
    public double maxDist = 1.0d;
    public double maxForce = 2.0d;

    private Transform origin;

    void Awake()
    {
        origin = GameObject.Find("Haptic Origin").transform;
    }
    
	void Start ()
	{
    }
	
	void Update () {
	    HapticNativePlugin.SetSpringProperties(constraintEnabled, transform.position - origin.position, minDist, maxDist, maxForce);
    }
}


