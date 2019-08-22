using System;
using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.IO;

public class ArduinoInterface : MonoBehaviour
{
    const byte COMMAND_NULL = 0;
    const byte COMMAND_START = 1;
    const byte COMMAND_STOP = 2;
    const byte COMMAND_STEP = 3;


    private SerialPort stream;

    public string portName = "COM5";

    public static byte msg_CW = 255;
    public static byte msg_CCW = 0;

	public float currentFluidResistance = 0.0f;
	public int baudRate = 9600;

    void Start()
    {
		stream = new SerialPort(portName, baudRate);
        stream.ReadTimeout = 2;

        // connect to Arduino via Serial Port
        try
        {
            stream.Open();

            Debug.Log("Arduino connected on port: " + portName);
        }
        catch (IOException)
        {
            Debug.Log("ERROR: No device found on port: " + portName);
            return;
        }

		SetResistance (0.0f);
    }

    

   void Update()
   {
		
       // if connected to arduino
       if (stream.IsOpen)
       {
			
       }
       else
       {
           // attempt to connect again
           try
           {
               stream.Open();
   
               Debug.Log("Arduino connected on port: " + portName);
           }
           catch (IOException)
           {
           }
       }
   }

	public void SetResistance(float resistanceValue)
	{
        if(currentFluidResistance != resistanceValue)
        {
            currentFluidResistance = resistanceValue;
        
            if (stream.IsOpen)
            {
                //byte message = (byte)(Mathf.Lerp(1.0f, 200.0f, (currentFluidResistance)) + 0.5f);
                //Debug.Log("sending new resistance message: " + message);
                //WriteByte(message);
        
                byte[] message = new byte[2];
                message[0] = COMMAND_STEP;
                message [1] = (byte)(Mathf.Lerp(0.0f, 255.0f, (currentFluidResistance)) + 0.5f);
                Debug.Log("sending new resistance message: " + message[1]);
        
                stream.Write(message, 0, 2);
            } else
            {
                Debug.Log("Set resistance but stream to arduino is closed");
            }
        }
	}

	void OpenValve()
	{
		WriteByte(msg_CW);
	}

	void CloseValve()
	{
		WriteByte(msg_CCW);
	}

    public virtual void WriteByte(byte value)
    {
        stream.Write(new byte[] { value }, 0, 1);
    }

    //public virtual void WriteShort(short value)
    //{
    //	byte byteLo = (byte)(value & 0xFF);
    //	byte byteHi = (byte)(value >> 8);
    //
    //	stream.Write(new byte[2] {byteLo, byteHi}, 0, 1);
    //}

    public void WriteString(string message)
    {
        stream.WriteLine(message);
        stream.BaseStream.Flush();
    }

    public string ReadArduino()
    {
        try
        {
            string message;

            int numMessagesToReadPerFrame = 1;
            int i = 0;

            do
            {
                message = stream.ReadLine();
                i++;
            } while (i < numMessagesToReadPerFrame); // also leaves the loop when timeout occurs

            return message;
        }
        catch (TimeoutException e)
        {
            return null;
        }
    }

	void OnDestroy()
	{
		stream.Close();
	}
}
