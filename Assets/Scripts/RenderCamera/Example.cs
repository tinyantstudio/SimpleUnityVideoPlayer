using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//Attach this script to an empty GameObject
//Create a new Camera (Create>Camera) and position it appropriately. Attach it to the Second Camera field in the Inspector of the GameObject
//Press the space key to enable and disable the second Camera

public class Example : MonoBehaviour
{
    //Attach a new Camera in the Inspector window
    public Camera m_SecondCamera;
    //This is set as the main Camera in this script
    Camera m_FirstCamera;

    void Start()
    {
        //Disable the second Camera
        m_SecondCamera.enabled = false;
        //Set where to place the second Camera along with its width and height
        m_SecondCamera.pixelRect = new Rect(0, 0, 400, 200);
        //Set the first Camera as the main Camera
        m_FirstCamera = Camera.main;
    }

    void Update()
    {
        //Press the space key to toggle the second Camera and output camera pixel width and height
        if (Input.GetKeyDown(KeyCode.Space))
        {
            //Check if the second camera is enabled yet
            if (!m_SecondCamera.enabled)
            {
                //Toggle the second Camera and output the second Camera's details
                ToggleCamera(m_SecondCamera, m_SecondCamera);
            }
            else
            {
                //Toggle the second Camera and output the first Camera's details
                ToggleCamera(m_SecondCamera, m_FirstCamera);
            }
        }
    }

    //Toggle the Camera and output the Camera specified
    void ToggleCamera(Camera cameraToggle, Camera cameraOutput)
    {
        //Set Camera on and off
        cameraToggle.enabled = !cameraToggle.enabled;

        //Output the Camera's new Pixel width
        Debug.Log("Pixel width :" + cameraOutput.pixelWidth + " Pixel height : " + cameraOutput.pixelHeight);
    }
}
