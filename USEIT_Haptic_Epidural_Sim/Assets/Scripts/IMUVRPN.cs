using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UVRPN.Core;

public class IMUVRPN : MonoBehaviour
{
    Vector2 x;
    Vector2 y;
    Vector2 z;

    public Quaternion calibrationQuat = new Quaternion();

    Quaternion yawPitchRoll;

    public Transform transformSlave;

    [SerializeField]
    VRPN_Analog x_VRPN;

    [SerializeField]
    VRPN_Analog y_VRPN;

    [SerializeField]
    VRPN_Analog z_VRPN;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyUp(KeyCode.C))
        {
            Calibrate();
        }

        x = x_VRPN.Analog;
        y = y_VRPN.Analog;
        z = z_VRPN.Analog;

        // Save the IMU values that come from VRPN
        yawPitchRoll = Quaternion.Euler(0f, x.x, z.x);
        //Debug.Log(yawPitchRoll);

        // The new orientation from offset from the calibration point
        Quaternion newOrientation = calibrationQuat * yawPitchRoll;

        // STOP SLAVERY
        transformSlave.localRotation = newOrientation;

    }

    // Set the the needle to the straight position
    void Calibrate()
    {
        calibrationQuat = Quaternion.Inverse(yawPitchRoll);
    }
}
