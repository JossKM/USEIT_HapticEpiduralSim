using UnityEngine;
using System.Collections;

// uses the object's position and orientation for the constraints. Only 1 may be active. Not to be attached to the proxy.

public enum FalconButton
{
    BACK = 0,
    LEFT = 1,
    FRONT = 2,
    RIGHT = 3
}


public class HapticAxialConstraint : MonoBehaviour
{
    private bool constraintEnabled = false;

    public float rotationSpeed = 5.0f;

	[Header("PID properties")]
	public double maxForce = 7.0d;
	public double kp 	= 120.0d;
	public double ki 	= 30.0d;
	public double kd 	= 10.0d;
	public double gain	= 1.0d;


    

    private Transform origin;

    public Transform constraintSlave;

    private Quaternion originalSlaveOrientation;

    void Awake()
    {
        origin = GameObject.Find("Haptic Origin").transform;
    }

	void Start ()
	{
	    originalSlaveOrientation = constraintSlave.rotation;
	}
	
	void Update ()
	{
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.LEFT))
        {
            transform.Rotate(Vector3.up, rotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.RIGHT))
        {
            transform.Rotate(Vector3.up, -rotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.FRONT))
        {
            transform.Rotate(Vector3.right, rotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.BACK))
        {
            transform.Rotate(Vector3.right, -rotationSpeed * Time.deltaTime);
        }

        Vector3 pos = transform.position - origin.position;

        //HapticNativePlugin.SetAxialConstraint(constraintEnabled, pos, transform.forward, minDist, maxDist, maxForce, damping);

		if (constraintEnabled) 
		{
			HapticNativePlugin.SetNeedleAxialPID(pos, transform.forward, maxForce, kp, ki, kd, gain);
		} else {
			HapticNativePlugin.SetNeedleAxialPID(pos, transform.forward, maxForce, kp, ki, kd, 0.0);
		}

		HapticNativePlugin.SetHapticEntryPoint(pos, transform.forward);
        
        constraintSlave.rotation = originalSlaveOrientation * transform.rotation;
    }

	public void SetEnabled(bool enabled)
	{
		if(constraintEnabled != enabled)
		{
			HapticNativePlugin.resetAxialConstraintPID();
			constraintEnabled = enabled;
		}
	}
}


