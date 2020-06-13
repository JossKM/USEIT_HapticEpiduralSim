using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using TMPro;

public class EpiduralProcedureTester : MonoBehaviour
{

    [SerializeField]
    TextMeshProUGUI attemptInfo;

    [SerializeField]
    TextMeshProUGUI prompt;

    [SerializeField]
    bool isTestRunning = false;
    bool duraPunctured = false;

    string failureCondition = "Penetrated Dura";

    [SerializeField]
    PenetrableMaterial targetLayer;

    class ProcedureAttempt
    {
        public string summaryText = "";
        public float timeStarted = 0.0f;
        public float timeEnded = 0.0f;
        public bool success = false;
        public string notes = "";
    }

    List<ProcedureAttempt> attempts = new List<ProcedureAttempt>();

    [SerializeField]
    int attemptNum = -1;

    private void RestartTest()
    {
        duraPunctured = false;
        //attemptSuccess = false;
    }

    // Start is called before the first frame update
    void Start()
    {
        EventManager.StartListening(failureCondition, OnEnterDura);
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetButtonDown("Submit"))
        {
            if (isTestRunning)
            {
                EndAttempt();
            }
            else
            {
                StartAttempt();
            }
        }

        if (Input.GetKeyDown(KeyCode.P))
        {
            WriteAttemptResultsToFile();
        }

        UpdateUI();


    }

    void UpdateUI()
    {
        if (attemptNum < attempts.Count)
        {
            if (isTestRunning)
            {
                ProcedureAttempt currentAttempt = attempts[attemptNum];
                attemptInfo.color = Color.green;
                attemptInfo.text = "Attempt # " + (attemptNum) + " Time: " + (Time.realtimeSinceStartup - currentAttempt.timeStarted);
                prompt.text = "Press Enter to End Test";
                prompt.color = Color.white;
            }
            else
            {
                attemptInfo.color = Color.white;
                prompt.text = "Press Enter to Start Test";
                prompt.color = Color.blue;

                if (attempts.Count > 0)
                {
                    ProcedureAttempt lastAttempt = attempts[attemptNum];
                    attemptInfo.text = "Last attempt: " + lastAttempt.summaryText;
                }
            }
        }
    }

    //sets attempt details and returns true if success
    bool EndAttempt()
    {
        isTestRunning = false;

        ProcedureAttempt currentAttempt = attempts[attemptNum];

if(currentAttempt != null)
{


        currentAttempt.timeEnded = Time.realtimeSinceStartup;
        var currentLayer = PenetrableMaterial.GetLayer(HapticNativePlugin.getLastLayerPenetratedID());
        currentAttempt.notes = "ended in: " + currentLayer.m_name;
        if(duraPunctured)
        {
            currentAttempt.notes += ", Dura punctured";
        }
        
        currentAttempt.success = (currentLayer == targetLayer);

        string result;

        if (currentAttempt.success)
        {
            result = "success";
        }
        else
        {
            result = "failure, ";
        }

        currentAttempt.summaryText = "Attempt # " + attemptNum + ": " + result + ", Duration: " + (currentAttempt.timeEnded - currentAttempt.timeStarted) + ", " + currentAttempt.notes; ;
        return currentAttempt.success;
} else return false;
    }

    void StartAttempt()
    {
        isTestRunning = true;
        RestartTest();
        var newAttempt = new ProcedureAttempt();
        newAttempt.timeStarted = Time.realtimeSinceStartup;

        attempts.Add(newAttempt);
        attemptNum = attempts.Count - 1;
        //return CheckForPunctureState.
    }

    void OnEnterDura()
    {
        duraPunctured = true;
    }

    void WriteAttemptResultsToFile()
    {
        string path = "Assets/Resources/test.txt";

        //Write some text to the test.txt file
        StreamWriter writer = new StreamWriter(path, true);
        writer.WriteLine("TEST");

        for(int i = 0; i < attempts.Count; i++)
        {
            var attempt = attempts[i];
           
            writer.WriteLine(attempt.summaryText);
        }
        writer.Close();
    }
}
