using System;
using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.IO;

public class StepperMotorInterface : MonoBehaviour
{
    const byte COMMAND_NULL = 0;
    const byte COMMAND_START = 1;
    const byte COMMAND_STOP = 2;
    const byte COMMAND_STEP = 3;

    [SerializeField]
    System.Collections.Generic.List<Ardunity.CommDevice> foundDevices = null;

    [SerializeField]
	float currentFluidResistance = 0.0f;

    [SerializeField]
    private Ardunity.CommSerial commSerial;

#if UNITY_EDITOR
    private void OnValidate()
    {
        if(foundDevices == null)
        {
            
        }
    }
#endif

    void Start()
    {
        commSerial.StartSearch();
        Debug.Log("lookin fer shit");
    }

    void Update()
    {
	    
    }

    void OnEndSearch()
    {
        foundDevices = commSerial.foundDevices;
        Debug.Log("shit found");
    }



    public void OnConnect()
    {
        SetResistance(0.0f);

    }

	public void SetResistance(float resistanceValue)
	{
        if(currentFluidResistance != resistanceValue)
        {
            currentFluidResistance = resistanceValue;

            if (commSerial.IsOpen)
            {
                byte[] message = new byte[2];
                message[0] = COMMAND_STEP;
                message [1] = (byte)(Mathf.Lerp(0.0f, 255.0f, (currentFluidResistance)) + 0.5f);
                Debug.Log("sending new resistance message: " + message[1]);

                commSerial.Write(message);
            } else
            {
                Debug.Log("Set resistance but stream to arduino is closed");
            }
        }
	}

    public string ReadString()
    {
        try
        {
            return commSerial.Read().ToString(); ;
        }
        catch (TimeoutException e)
        {
            return null;
        }
    }
}
