using System;
using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.IO;

using UnityEngine.Events;

public class ArduinoConnection : MonoBehaviour
{
    public SerialPort Port { get; private set; }

    public string portName = "COM6";

    public int baudRate = 9600;


    [SerializeField]
    public int maxMessagesReadPerUpdate = 5;

    [Tooltip("If this is set to false then an event will be triggered for every single message, which can cause slowdown depending on how fast they are being sent")]
    [SerializeField]
    bool OnlyUseMostRecentMessage = true;

    [SerializeField]
    public string LastMessage { get; private set; } = "";


    [SerializeField]
    float updateRate = 60.0f; // hertz

    public UnityEvent OnSerialRead;

    // Start is called before the first frame update
    void Start()
    {
        if (Port == null)
        {
            Port = new SerialPort(portName, baudRate);
            Port.ReadTimeout = 2;
        }

        StartCoroutine(AutoConnectProcess());
    }

    public bool isConnectedAndOpen()
    {
        return (Port != null && Port.IsOpen);
    }

    IEnumerator AutoConnectProcess()
    {
        while (!isConnectedAndOpen())
        {
            if (!Port.IsOpen)
            {
                AttemptConnect();
                yield return new WaitForSeconds(0.1f);

            }
        }
    }

    bool AttemptConnect()
    {
        try
        {
            // attempt connection
            Port.Open();
            Debug.Log("Device connected on port: " + portName);

            InvokeRepeating("UpdateArduino", 0.0f, 1.0f / updateRate);
            return true;
        }
        catch (IOException)
        {
            Debug.Log("ERROR: No device found on port: " + portName);
            return false;
        }
    }


    public virtual void WriteByte(byte value)
    {
        Port.Write(new byte[] { value }, 0, 1);
    }

    public virtual void WriteBytes(byte[] values, int length)
    {
        Port.Write(values, 0, length);
    }

    public void WriteString(string message)
    {
        Port.WriteLine(message);
        Port.BaseStream.Flush();
    }

    public void UpdateArduino()
    {
        int i = 0;
        try
        {
            if (!OnlyUseMostRecentMessage)
            {
                while(i < maxMessagesReadPerUpdate)
                {

                    LastMessage = Port.ReadLine();
                    OnSerialRead.Invoke();
                    i++;
                } // also leaves the loop when timeout occurs
            }
            else
            {
                while (i < maxMessagesReadPerUpdate)
                {
                    LastMessage = Port.ReadLine();
                    i++;
                } // also leaves the loop when timeout occurs
            }
        }
        catch (TimeoutException e)
        {
            // if a timeout occured but a message was read, then send the message
           if(OnlyUseMostRecentMessage && i > 0)
            {
                OnSerialRead.Invoke();
            }
        }
    }
}
