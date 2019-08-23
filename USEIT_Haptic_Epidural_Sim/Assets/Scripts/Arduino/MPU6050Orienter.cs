using UnityEngine;
using UnityEngine.Events;
using System.Collections;

/// <summary>
/// Rotates itself based on arduino input strings
/// </summary>

public class MPU6050Orienter : MonoBehaviour
{
    //[SerializeField]
    private string orientationKeyString = "ypr"; // yaw pitch roll

    [SerializeField]
    private float rotationScalingFactor = 1.0f;

    private Quaternion calibrationQuat = new Quaternion();

    [SerializeField]
    ArduinoConnection arduino;

    [SerializeField]
    bool isRunning = false;

    [SerializeField]
    string lastMessage;

    [SerializeField]
    Transform transformSlave;

    [SerializeField]
    UnityEvent OnOrientationUpdate;

    public void Calibrate()
    {
        StartCoroutine(Calibrate_CRT());
    }

    IEnumerator Calibrate_CRT()
    {
        arduino.WriteString("!");

        yield return new WaitForSecondsRealtime((float)arduino.Port.ReadTimeout * 0.01f);
        lastMessage = arduino.LastMessage;

        if (lastMessage != null)
        {
            if (lastMessage.StartsWith(orientationKeyString))
            {
                calibrationQuat = Quaternion.Inverse(ParseYPR(lastMessage));
                Debug.Log("calibrated to orientation: " + calibrationQuat);
                // determine the rotation required to make the current rotation reading into identity (i.e. applying the calibration quaternion makes brings us to the "zero rotation"

                // start updating
                StartUpdating();

            }
        }
    }

    void UpdateOrientationYPR()
    {
        lastMessage = arduino.LastMessage;
        if (lastMessage != null)
        {
            if (lastMessage.StartsWith(orientationKeyString))
            { //Debug.Log("updating orientation");
                // apply calibration
                transformSlave.localRotation = calibrationQuat * ParseYPR(lastMessage);
                transformSlave.forward = transformSlave.localRotation * Vector3.forward;
                OnOrientationUpdate.Invoke();
            }
        }
    }

    public void StopUpdating()
    {
        if (isRunning)
        {
            arduino.OnSerialRead.RemoveListener(UpdateOrientationYPR);
            isRunning = false;
        }
    }

    public void StartUpdating()
    {
        if (!isRunning)
        {
            arduino.OnSerialRead.AddListener(UpdateOrientationYPR);
            isRunning = true;
        }
    }

    Quaternion ParseYPR(string input)
    {
        string valueStr = input.Remove(0, orientationKeyString.Length);
        var splitStr = valueStr.Split(","[0]);

        Vector3 yawPitchRoll = new Vector3(float.Parse(splitStr[1]), float.Parse(splitStr[0]), float.Parse(splitStr[2]));//float.Parse (splitStr [1]), 0.0f)//float.Parse (splitStr [2]));

        Quaternion ret = Quaternion.Euler(yawPitchRoll);
        // apply scaling and return

        return Quaternion.SlerpUnclamped(Quaternion.identity, ret, rotationScalingFactor);
    }
}
