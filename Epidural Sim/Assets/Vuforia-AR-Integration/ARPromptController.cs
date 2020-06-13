// Prompts the user to look at trackable if they are not looking at it.
// to stop this behaviour, disable the component

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Vuforia;

public class ARPromptController : MonoBehaviour
{
    [SerializeField]
    TrackableBehaviour trackableTarget;
    [SerializeField]
    Canvas promptCanvas;
    [SerializeField]
    Text promptTextField;

    //[SerializeField]
    //string promptString = "Please center your view on the trackable object: ";

    // Update is called once per frame
    void Update()
    {
        if (trackableTarget.CurrentStatus != TrackableBehaviour.Status.TRACKED)
        {
            promptCanvas.gameObject.SetActive(true);
            promptTextField.text = "Please center your view on a trackable object: " + trackableTarget.TrackableName;
        }
        else
        {
            promptCanvas.gameObject.SetActive(false);
        }
    }

    private void OnDisable()
    {
        promptCanvas.gameObject.SetActive(false);
    }

    //private bool isTrackingMarker(TrackableBehavioure)
    //{
    //    var imageTarget = GameObject.Find(imageTargetName);
    //    var trackable = imageTarget.GetComponent<TrackableBehaviour>();
    //    var status = trackable.CurrentStatus;
    //    return status == TrackableBehaviour.Status.TRACKED;
    //}
}
