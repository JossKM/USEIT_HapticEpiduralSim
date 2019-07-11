//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D.
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.1.1 $Rev: 1925 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled 
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
cGenericHapticDevicePtr hapticDevice;

// a virtual tool representing the haptic device in the scene
cToolCursor* tool;

// a multi mesh object
cMesh* meshBase;
cMesh* meshShaft;
cMesh* meshBeam;

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelHapticRate;

// audio device to play sound
cAudioDevice* audioDevice;

// audio buffers to store sound files
cAudioBuffer* audioBuffer1;
cAudioBuffer* audioBuffer2;
cAudioBuffer* audioBuffer3;
cAudioBuffer* audioBuffer4;

// audio buffer to store sound of a drill
cAudioBuffer* audioBufferDrill;

// audio source of a drill
cAudioSource* audioSourceDrill;

// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = true;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// information about computer screen and GLUT display window
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

// root resource path
string resourceRoot;


//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a key is pressed
void keySelect(unsigned char key, int x, int y);

// callback to render graphic scene
void updateGraphics(void);

// callback of GLUT timer
void graphicsTimer(int data);

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);


//==============================================================================
/*
    DEMO:    25-sounds

    This example illustrates how to assign sound properties to objects in a
    world.
*/
//==============================================================================

int main(int argc, char* argv[])
{
    //--------------------------------------------------------------------------
    // INITIALIZATION
    //--------------------------------------------------------------------------

    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "Demo: 25-sounds" << endl;
    cout << "Copyright 2003-2016" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << "Keyboard Options:" << endl << endl;
    cout << "[f] - Enable/Disable full screen mode" << endl;
    cout << "[m] - Enable/Disable vertical mirroring" << endl;
    cout << "[x] - Exit application" << endl;
    cout << endl << endl;

    // parse first arg to try and locate resources
    resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);


    //--------------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
    //--------------------------------------------------------------------------

    // initialize GLUT
    glutInit(&argc, argv);

    // retrieve  resolution of computer display and position window accordingly
    screenW = glutGet(GLUT_SCREEN_WIDTH);
    screenH = glutGet(GLUT_SCREEN_HEIGHT);
    windowW = 0.8 * screenH;
    windowH = 0.5 * screenH;
    windowPosY = (screenH - windowH) / 2;
    windowPosX = windowPosY; 

    // initialize the OpenGL GLUT window
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(windowW, windowH);

    if (stereoMode == C_STEREO_ACTIVE)
        glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
    else
        glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    // create display context and initialize GLEW library
    glutCreateWindow(argv[0]);

#ifdef GLEW_VERSION
    // initialize GLEW
    glewInit();
#endif

    // setup GLUT options
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
    glutSetWindowTitle("CHAI3D");

    // set fullscreen mode
    if (fullscreen)
    {
        glutFullScreen();
    }


    //--------------------------------------------------------------------------
    // WORLD - CAMERA - LIGHTING
    //--------------------------------------------------------------------------

    // create a new world
    world = new cWorld();

    // set the background color of the environment
    world->m_backgroundColor.setWhite();

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and orient the camera
    camera->set(cVector3d(0.9, 0.0, 0.4),    // camera position (eye)
                cVector3d(0.0, 0.0, 0.0),    // lookat position (target)
                cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

    // set the near and far clipping planes of the camera
    // anything in front or behind these clipping planes will not be rendered
    camera->setClippingPlanes(0.01, 10.0);

    // set stereo mode
    camera->setStereoMode(stereoMode);

    // set stereo eye separation and focal length (applies only if stereo is enabled)
    camera->setStereoEyeSeparation(0.02);
    camera->setStereoFocalLength(1.0);

    // set vertical mirrored display mode
    camera->setMirrorVertical(mirroredDisplay);

    // create a light source
    light = new cSpotLight(world);

    // attach light to camera
    world->addChild(light);

    // enable light source
    light->setEnabled(true);

    // position the light source
    light->setLocalPos(0.5, 0.5, 0.5);

    // define the direction of the light beam
    light->setDir(-1.0,-1.0,-1.0);

    // enable this light source to generate shadows
    light->setShadowMapEnabled(true);

    // set the resolution of the shadow map
    light->m_shadowMap->setQualityLow();
    //light->m_shadowMap->setQualityMedium();

    // set light cone half angle
    light->setCutOffAngleDeg(40);


    //--------------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //--------------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get access to the first available haptic device
    handler->getDevice(hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

    // create a 3D tool and add it to the world
    tool = new cToolCursor(world);
    camera->addChild(tool);

    // position tool in respect to camera
    tool->setLocalPos(-1.0, 0.0, 0.0);

    // connect the haptic device to the tool
    tool->setHapticDevice(hapticDevice);

    // if the haptic device has a gripper, enable it as a user switch
    hapticDevice->setEnableGripperUserSwitch(true);

    // set radius of tool
    double toolRadius = 0.05;

    // define a radius for the tool
    tool->setRadius(toolRadius);

    // set color of tool
    tool->m_hapticPoint->m_sphereProxy->m_material->setWhite();

    // map the physical workspace of the haptic device to a larger virtual workspace
    tool->setWorkspaceRadius(1.0);

    // enable if objects in the scene are going to rotate of translate
    // or possibly collide against the tool. If the environment
    // is entirely static, you can set this parameter to "false"
    tool->enableDynamicObjects(true);

    // haptic forces are enabled only if small forces are first sent to the device;
    // this mode avoids the force spike that occurs when the application starts when 
    // the tool is located inside an object for instance. 
    tool->setWaitForSmallForce(true);

    // start the haptic tool
    tool->start();


    //--------------------------------------------------------------------------
    // SETUP AUDIO MATERIAL
    //--------------------------------------------------------------------------

    // create an audio device to play sounds
    audioDevice = new cAudioDevice();
    
    // attach audio device to camera
    camera->attachAudioDevice(audioDevice);

    // create an audio buffer and load audio wave file
    audioBuffer1 = audioDevice->newAudioBuffer();
    bool fileload1 = audioBuffer1->loadFromFile(RESOURCE_PATH("../resources/sounds/metal-scraping.wav"));
    if (!fileload1)
    {
        #if defined(_MSVC)
        fileload1 = audioBuffer1->loadFromFile("../../../bin/resources/sounds/metal-scraping.wav");
        #endif
    }

    // create an audio buffer and load audio wave file
    audioBuffer2 = audioDevice->newAudioBuffer();
    bool fileload2 = audioBuffer2->loadFromFile(RESOURCE_PATH("../resources/sounds/metal-impact.wav"));
    if (!fileload2)
    {
        #if defined(_MSVC)
        fileload2 = audioBuffer2->loadFromFile("../../../bin/resources/sounds/metal-impact.wav");
        #endif
    }

    // create an audio buffer and load audio wave file
    audioBuffer3 = audioDevice->newAudioBuffer();
    bool fileload3 = audioBuffer3->loadFromFile(RESOURCE_PATH("../resources/sounds/wood-scraping.wav"));
    if (!fileload3)
    {
        #if defined(_MSVC)
        fileload3 = audioBuffer3->loadFromFile("../../../bin/resources/sounds/wood-scraping.wav");
        #endif
    }

    // create an audio buffer and load audio wave file
    audioBuffer4 = audioDevice->newAudioBuffer();
    bool fileload4 = audioBuffer4->loadFromFile(RESOURCE_PATH("../resources/sounds/wood-impact.wav"));
    if (!fileload4)
    {
        #if defined(_MSVC)
        fileload4 = audioBuffer4->loadFromFile("../../../bin/resources/sounds/wood-impact.wav");
        #endif
    }

    // check for errors
    if (!(fileload1 && fileload2 && fileload3 && fileload4))
    {
        cout << "Error - Sound file failed to load or initialize correctly." << endl;
        close();
        return (-1);
    }

    // here we convert all files to mono. this allows for 3D sound support. if this code
    // is commented files are kept in stereo format and 3D sound is disabled. Compare both!
    audioBuffer1->convertToMono();
    audioBuffer2->convertToMono();
    audioBuffer3->convertToMono();
    audioBuffer4->convertToMono();

    // create an audio source for this tool.
    tool->createAudioSource(audioDevice);


    //--------------------------------------------------------------------------
    // SETUP AUDIO FOR A DRILL
    //--------------------------------------------------------------------------

    audioBufferDrill = audioDevice->newAudioBuffer();
    bool fileload5 = audioBufferDrill->loadFromFile(RESOURCE_PATH("../resources/sounds/drill.wav"));
    if (!fileload5)
    {
        #if defined(_MSVC)
        fileload5 = audioBufferDrill->loadFromFile("../../../bin/resources/sounds/drill.wav");
        #endif
    }
    if (!fileload5)
    {
        cout << "Error - Sound file failed to load or initialize correctly." << endl;
        close();
        return (-1);
    }

    // create audio source
    audioSourceDrill = audioDevice->newAudioSource();

    // assign auio buffer to audio source
    audioSourceDrill->setAudioBuffer(audioBufferDrill);

    // loop playing of sound
    audioSourceDrill->setLoop(true);

    // turn off sound for now
    audioSourceDrill->setGain(0.0);

    // set pitch
    audioSourceDrill->setPitch(0.2);

    // play sound
    audioSourceDrill->play();


    //--------------------------------------------------------------------------
    // CREATE OBJECTS
    //--------------------------------------------------------------------------

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // properties
    double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;


    /////////////////////////////////////////////////////////////////////////
    // MESH BASE:
    /////////////////////////////////////////////////////////////////////////

    // create a mesh
    meshBase = new cMesh();

    // add object to world
    world->addChild(meshBase);

    // build mesh using a cylinder primitive
    cCreateCylinder(meshBase,
                    0.02,
                    0.45,
                    64,
                    1,
                    1,
                    true,
                    true,
                    cVector3d(0.0, 0.0, 0.0),
                    cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ)
                    );

    // enable material properties for graphic rendering
    meshBase->setUseMaterial(true);

    // set material color
    meshBase->m_material->setBlueCornflower();
    
    // set haptic properties
    meshBase->m_material->setStiffness(0.7 * maxStiffness);
    meshBase->m_material->setStaticFriction(0.5);
    meshBase->m_material->setDynamicFriction(0.5);
    meshBase->m_material->setHapticTriangleSides(true, false);

    // set audio properties
    meshBase->m_material->setAudioFrictionBuffer(audioBuffer3);
    meshBase->m_material->setAudioFrictionGain(0.1);
    meshBase->m_material->setAudioFrictionPitchGain(0.2);
    meshBase->m_material->setAudioFrictionPitchOffset(1.0);
    meshBase->m_material->setAudioImpactBuffer(audioBuffer4);
    meshBase->m_material->setAudioImpactGain(0.1);
 
    // create collision detector
    meshBase->createAABBCollisionDetector(toolRadius);


    /////////////////////////////////////////////////////////////////////////
    // MESH SHAFT:
    /////////////////////////////////////////////////////////////////////////

    // create a mesh
    meshShaft = new cMesh();

    // add object to world
    world->addChild(meshShaft);

    // build mesh using a cylinder primitive
    cCreateCylinder(meshShaft,
                    0.1,
                    0.02,
                    64,
                    1,
                    1,
                    true,
                    true,
                    cVector3d(0.0, 0.0, 0.0),
                    cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ)
                    );

    // enable material properties for graphic rendering
    meshShaft->setUseMaterial(true);

    // set material color
    meshShaft->m_material->setWhite();
    
    // set haptic properties
    meshShaft->m_material->setStiffness(0.7 * maxStiffness);
    meshShaft->m_material->setStaticFriction(0.5);
    meshShaft->m_material->setDynamicFriction(0.5);
    meshShaft->m_material->setHapticTriangleSides(true, false);

    // create collision detector
    meshShaft->createAABBCollisionDetector(toolRadius);

    // set the position of the object
    meshShaft->setLocalPos(0.0, 0.0, 0.0);


    /////////////////////////////////////////////////////////////////////////
    // MESH METAL BEAM:
    /////////////////////////////////////////////////////////////////////////

    // create a mesh
    meshBeam = new cMesh();

    // add object to world
    world->addChild(meshBeam);

    // build mesh using a cylinder primitive
    cCreateBox(meshBeam, 0.65, 0.1, 0.03);
    cCreateBox(meshBeam, 0.7, 0.03, 0.1, cVector3d(0.0,-0.05, 0.0));
    cCreateBox(meshBeam, 0.7, 0.03, 0.1, cVector3d(0.0, 0.05, 0.0));

    // setup texture
    meshBeam->m_texture = cTexture2d::create();
    bool fileload = meshBeam->m_texture->loadFromFile(RESOURCE_PATH("../resources/images/spheremap-3.jpg"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = meshBeam->m_texture->loadFromFile("../../../bin/resources/images/spheremap-3.jpg");
        #endif
    }
    if (!fileload)
    {
        cout << "Error - Texture image failed to load correctly." << endl;
        close();
        return (-1);
    }

    // enable material and texture properties for graphic rendering
    meshBeam->setUseMaterial(true);
    meshBeam->setUseTexture(true);

    // enable spherical mapping
    meshBeam->m_texture->setSphericalMappingEnabled(true);

    // set material color
    meshBeam->m_material->setWhite();

    // set haptic properties
    meshBeam->m_material->setStiffness(1.3 * maxStiffness);
    meshBeam->m_material->setStaticFriction(0.4);
    meshBeam->m_material->setDynamicFriction(0.4);
    meshBeam->m_material->setHapticTriangleSides(true, false);
    meshBeam->setUseMaterial(true);

    // set audio properties
    meshBeam->m_material->setAudioFrictionBuffer(audioBuffer1);
    meshBeam->m_material->setAudioFrictionGain(0.8);
    meshBeam->m_material->setAudioFrictionPitchGain(0.2);
    meshBeam->m_material->setAudioFrictionPitchOffset(0.8);
    meshBeam->m_material->setAudioImpactBuffer(audioBuffer2);
    meshBeam->m_material->setAudioImpactGain(0.2);

    // create collision detector
    meshBeam->createAABBCollisionDetector(toolRadius);

    // set the position of the object
    meshBeam->setLocalPos(0.0, 0.0, 0.15);
    meshBeam->rotateAboutGlobalAxisDeg(0.0, 0.0, 1.0, 50);


    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    // create a font
    cFont *font = NEW_CFONTCALIBRI20();
    
    // create a label to display the haptic rate of the simulation
    labelHapticRate = new cLabel(font);
    camera->m_frontLayer->addChild(labelHapticRate);
    labelHapticRate->m_fontColor.setBlack();

    // create a background
    cBackground* background = new cBackground();
    camera->m_backLayer->addChild(background);

    // set background properties
    background->setCornerColors(cColorf(1.0f, 1.0f, 1.0f),
                                cColorf(1.0f, 1.0f, 1.0f),
                                cColorf(0.8f, 0.8f, 0.8f),
                                cColorf(0.8f, 0.8f, 0.8f));


    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    cThread* hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    atexit(close);

    // start the main graphics rendering loop
    glutTimerFunc(50, graphicsTimer, 0);
    glutMainLoop();

    // exit
    return (0);
}

//------------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
    windowW = w;
    windowH = h;
}

//------------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
    if ((key == 27) || (key == 'x'))
    {
        // exit application
        exit(0);
    }

    // option f: toggle fullscreen
    if (key == 'f')
    {
        if (fullscreen)
        {
            windowPosX = glutGet(GLUT_INIT_WINDOW_X);
            windowPosY = glutGet(GLUT_INIT_WINDOW_Y);
            windowW = glutGet(GLUT_INIT_WINDOW_WIDTH);
            windowH = glutGet(GLUT_INIT_WINDOW_HEIGHT);
            glutPositionWindow(windowPosX, windowPosY);
            glutReshapeWindow(windowW, windowH);
            fullscreen = false;
        }
        else
        {
            glutFullScreen();
            fullscreen = true;
        }
    }

    // option m: toggle vertical mirroring
    if (key == 'm')
    {
        mirroredDisplay = !mirroredDisplay;
        camera->setMirrorVertical(mirroredDisplay);
    }
}

//------------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close/stop streaming objects
    delete audioDevice;

    // close haptic device
    tool->stop();
}

//------------------------------------------------------------------------------

void graphicsTimer(int data)
{
    if (simulationRunning)
    {
        glutPostRedisplay();
    }

    glutTimerFunc(50, graphicsTimer, 0);
}

//------------------------------------------------------------------------------

void updateGraphics(void)
{
    /////////////////////////////////////////////////////////////////////
    // UPDATE WIDGETS
    /////////////////////////////////////////////////////////////////////

    // update haptic rate data
    labelHapticRate->setText(cStr(frequencyCounter.getFrequency(), 0) + " Hz");

    // update position of label
    labelHapticRate->setLocalPos((int)(0.5 * (windowW - labelHapticRate->getWidth())), 15);


    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    world->updateShadowMaps(false, mirroredDisplay);

    // render world
    camera->renderView(windowW, windowH);

    // swap buffers
    glutSwapBuffers();

    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
    // angular velocity of object
    cVector3d angVel(0.0, 0.2, 0.3);

    // reset clock
    cPrecisionClock clock;
    clock.reset();

    // simulation in now running
    simulationRunning  = true;
    simulationFinished = false;

    // velocity of drill
    double drillVelocity = 0.0;

    // main haptic simulation loop
    while(simulationRunning)
    {
        /////////////////////////////////////////////////////////////////////
        // SIMULATION TIME
        /////////////////////////////////////////////////////////////////////

        // stop the simulation clock
        clock.stop();

        // read the time increment in seconds
        double timeInterval = clock.getCurrentTimeSeconds();

        // restart the simulation clock
        clock.reset();
        clock.start();

        // update frequency counter
        frequencyCounter.signal(1);


        /////////////////////////////////////////////////////////////////////
        // HAPTIC FORCE COMPUTATION
        /////////////////////////////////////////////////////////////////////

        // compute global reference frames for each object
        world->computeGlobalPositions(true);

        // update position and orientation of tool
        tool->updateFromDevice();

        // compute interaction forces
        tool->computeInteractionForces();


        /////////////////////////////////////////////////////////////////////
        // DRILLING
        /////////////////////////////////////////////////////////////////////

        // settings
        const double DRILL_STARTUP_TIME = 10.0;
        const double DRILL_FREQUENCY = 300.0;
        const double DRILL_FORCE_MAG = 0.5;
        const double DRILL_AUDIO_GAIN = 4.0;

        // get user switch
        bool userButton = tool->getUserSwitch(0);

        // compute audio level according to drill speed rize
        if (userButton)
        {
            drillVelocity = cClamp(drillVelocity + DRILL_STARTUP_TIME * timeInterval, 0.0, 1.0);
        }
        else
        {
            drillVelocity = cClamp(drillVelocity - DRILL_STARTUP_TIME * timeInterval, 0.0, 1.0);
        }

        // set color material of tool according to drill speed level.
        tool->m_hapticPoint->m_sphereProxy->m_material->setColorf(1.0, 1.0 - drillVelocity, 1.0 - drillVelocity);
        
        // set audio gain
        audioSourceDrill->setGain(DRILL_AUDIO_GAIN * drillVelocity);

        // compute vibration force according to drill speed
        cVector3d forceDrill(DRILL_FORCE_MAG * cSqr(drillVelocity) * sin((2.0 * C_PI * DRILL_FREQUENCY) * clock.getCPUTimeSeconds()), 0.0, 0.0);
        
        // apply force to haptic device
        tool->addDeviceLocalForce(forceDrill);

        // send forces to haptic device
        tool->applyToDevice();


        /////////////////////////////////////////////////////////////////////
        // DYNAMIC SIMULATION
        /////////////////////////////////////////////////////////////////////

        // some constants
        const double INERTIA = 0.4;
        const double MAX_ANG_VEL = 10.0;
        const double DAMPING = 0.1;

        // get position of cursor in global coordinates
        cVector3d toolPos = tool->getDeviceGlobalPos();

        // get position of object in global coordinates
        cVector3d objectPos = meshBeam->getGlobalPos();

        // compute a vector from the center of mass of the object (point of rotation) to the tool
        cVector3d v = cSub(toolPos, objectPos);

        // compute angular acceleration based on the interaction forces
        // between the tool and the object
        cVector3d angAcc(0,0,0);
        if (v.length() > 0.0)
        {
            // get the last force applied to the cursor in global coordinates
            // we negate the result to obtain the opposite force that is applied on the
            // object
            cVector3d toolForce = -tool->getDeviceGlobalForce();

            // compute the effective force that contributes to rotating the object.
            cVector3d force = toolForce - cProject(toolForce, v);

            // compute the resulting torque
            cVector3d torque = cMul(v.length(), cCross( cNormalize(v), force));

            // update rotational acceleration
            angAcc = (1.0 / INERTIA) * torque;
        }

        // update rotational velocity
        angVel.add(timeInterval * angAcc);

        // set a threshold on the rotational velocity term
        double vel = angVel.length();
        if (vel > MAX_ANG_VEL)
        {
            angVel.mul(MAX_ANG_VEL / vel);
        }

        // add some damping too
        angVel.mul(1.0 - DAMPING * timeInterval);

        // compute the next rotation configuration of the object
        if (angVel.length() > C_SMALL)
        {
            angVel.x(0.0);
            angVel.y(0.0);

            meshBeam->rotateAboutGlobalAxisRad(cNormalize(angVel), timeInterval * angVel.length());
            meshBase->rotateAboutGlobalAxisRad(cNormalize(angVel), timeInterval * angVel.length());
        }
    }
    
    // exit haptics thread
    simulationFinished = true;
}

//------------------------------------------------------------------------------
