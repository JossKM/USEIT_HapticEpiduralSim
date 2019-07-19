using System;
using UnityEngine;
using System.Collections;
using System.IO.Ports;
using System.IO;

public class ArduinoConnection : MonoBehaviour
{
    private SerialPort stream;

    public string portName = "COM6";

    public int baudRate = 9600;

    // Start is called before the first frame update
    void Start()
    {
        //stream = new SerialPort(portName, baudRate);
        //stream.ReadTimeout = 2;

        //// connect to Arduino via Serial Port
        //try
        //{
        //    stream.Open();
        //
        //    Debug.Log("Arduino connected on port: " + portName);
        //}
        //catch (IOException)
        //{
        //    Debug.Log("ERROR: No device found on port: " + portName);
        //    return;
        //}
        StartCoroutine(ConnectProcess());
        
    }

    public SerialPort GetSerialPort()
    {
        return stream;
    }

    public bool isConnectedAndOpen()
    {
        return (stream != null && stream.IsOpen);
    }

    IEnumerator ConnectProcess()
    {
        while (!isConnectedAndOpen())
        {
            if(stream == null)
            {
                stream = new SerialPort(portName, baudRate);
                stream.ReadTimeout = 2;
            }

            if(!stream.IsOpen)
            {
                try
                {
                    // attempt connection
                    stream.Open();
                    Debug.Log("Device connected on port: " + portName);
                }
                catch (IOException)
                {
                    Debug.Log("ERROR: No device found on port: " + portName);
                }

            }

            yield return new WaitForSeconds(0.1f);
        }
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
