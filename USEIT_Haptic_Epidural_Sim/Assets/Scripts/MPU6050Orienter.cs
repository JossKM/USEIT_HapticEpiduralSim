using System;
using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.IO;


public class MPU6050Orienter : MonoBehaviour
{
	private SerialPort stream;
	public string portName = "COM5";

	public string lastMessage = "";
	public string orientationKeyString = "quat";

	public Quaternion calibrationQuat = new Quaternion ();

	public Transform transformSlave;
	public Vector3 eulerVec; 

	bool isRotationActive = false;

	void Start()
	{
		stream = new SerialPort(portName, 115200);
		stream.ReadTimeout = 10;

		// connect to Arduino via Serial Port
		try
		{
			stream.Open();

			Debug.Log("Orienter: Arduino connected on port: " + portName);
		}
		catch (IOException)
		{
			Debug.Log("ERROR: No device found on port: " + portName);
			return;
		}
    }

	void Update()
	{	
		// if connected to arduino
		if (stream.IsOpen)
		{
			ReadArduino();

			if (Input.GetKeyUp(KeyCode.C))
			{
				Calibrate();
			}

			ReadOrientationYPR();
		}
		else
		{
			isRotationActive = false;
			// attempt to connect again
			try
			{
				stream.Open();

				Debug.Log("Orienter: Arduino connected on port: " + portName);

                // tell it to begin sending information
                stream.WriteLine("!");
            }
			catch (IOException)
			{
			}
		}
	}

	public string ReadArduino()
	{
		try
		{
			int numMessagesToReadPerFrame = 100;
			int i = 0;

			do
			{
				lastMessage = stream.ReadLine();
				i++;
			} while (i < numMessagesToReadPerFrame); // also leaves the loop when timeout occurs

			return lastMessage;
		}
		catch (TimeoutException e)
		{
			return null;
		}
	}

	void Calibrate()
	{
        stream.WriteLine("!");
        Debug.Log("calibrating...");
		if (lastMessage != null)
		{
			//Debug.Log(lastMessage);
			if (lastMessage.StartsWith(orientationKeyString))
			{
				calibrationQuat = Quaternion.Inverse(ParseYPR(lastMessage));

				Debug.Log("...to orientation: " + calibrationQuat);

                isRotationActive = true;
            }
		}
	}

	void OnDestroy()
	{
        if(stream.IsOpen)
        {
		    stream.Close();
        }
	}


	void ReadOrientationYPR()
	{
		if (lastMessage != null)
		{
			if (lastMessage.StartsWith (orientationKeyString)) 
			{
				Quaternion newOrientation = calibrationQuat * ParseYPR(lastMessage);

				if (transformSlave != null && isRotationActive) 
				{
					transformSlave.localRotation =  transform.localRotation * newOrientation;
				}
			}
		}
	}

	Quaternion ParseYPR(string input)
	{
		string valueStr = input.Remove (0, orientationKeyString.Length);
		var splitStr = valueStr.Split ("," [0]);
	
		Vector3 yawPitchRoll = new Vector3 (float.Parse (splitStr [1]), float.Parse (splitStr [0]), float.Parse (splitStr [2]));//float.Parse (splitStr [1]), 0.0f)//float.Parse (splitStr [2]));
		return Quaternion.Euler (yawPitchRoll);
	}
}
