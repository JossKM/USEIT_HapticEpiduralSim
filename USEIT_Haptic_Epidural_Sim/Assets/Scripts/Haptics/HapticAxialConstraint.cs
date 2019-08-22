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

    [SerializeField]
    public float manualControlRotationSpeed = 6.0f;

    [Tooltip("Interpolate between 2 modes for determining the final slave orientation: (0: use direction vector from axial constraint to the slave position), and (1: use the axial constraint forward direction alone)")]
    [Range(0.0f, 1.0f)]
    [SerializeField]
    public float orientationWeight = 0.1f;

    [Header("PID properties")]
    public double maxForce = 7.0d;
    public double kp = 120.0d;
    public double ki = 30.0d;
    public double kd = 10.0d;
    public double gain = 1.0d;

    //private Transform origin;

    [SerializeField]
    private Transform constraintSlave;
    
    void Update()
    {
        if (!HapticManager.isHapticAvail)
        {
            gameObject.SetActive(false);
            return;
        }

        if (HapticNativePlugin.isButtonPressed((int)FalconButton.LEFT))
        {
            transform.Rotate(Vector3.up, manualControlRotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.RIGHT))
        {
            transform.Rotate(Vector3.up, -manualControlRotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.FRONT))
        {
            transform.Rotate(Vector3.right, manualControlRotationSpeed * Time.deltaTime);
        }
        if (HapticNativePlugin.isButtonPressed((int)FalconButton.BACK))
        {
            transform.Rotate(Vector3.right, -manualControlRotationSpeed * Time.deltaTime);
        }
    }

    public void UpdateConstraint()
    {
        double currentGain = gain;
        Vector3 pivotPos = transform.localPosition; //position - origin.position;

        if (constraintEnabled)
        {
            HapticNativePlugin.SetHapticEntryPoint(pivotPos, transform.forward);
        }
        else
        {
            currentGain = 0.0;
        }

        HapticNativePlugin.SetNeedleAxialPID(pivotPos, transform.forward, maxForce, kp, ki, kd, currentGain);


        //Set rotation!
        if(constraintEnabled)
        {
            Vector3 constraintToSlaveVec = (constraintSlave.position) - transform.position;

            // point target postion ahead to avoid errors trying to get direction of a zero vector
            constraintToSlaveVec = constraintToSlaveVec + transform.forward * orientationWeight;
            constraintSlave.rotation = Quaternion.LookRotation(constraintToSlaveVec);
        } else
        {
            constraintSlave.localRotation = transform.localRotation;
        }
    }

    public void SetEnabled(bool enabled)
    {
        // gameObject.SetActive(enabled);

        if (enabled)
        {
            HapticNativePlugin.resetAxialConstraintPID();
        }

        constraintEnabled = enabled;
    }

    public bool GetEnabled()
    {
        return constraintEnabled;
    }
}


