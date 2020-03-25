#include "ofApp.h"

using namespace cv;
using namespace ofxCv;
using namespace glm;

//--------------------------------------------------------------
void ofApp::setup() {
    settings.loadFile("settings.xml");

    ofSetVerticalSync(false);
    ofHideCursor();

    //movie.load("Comp_8.mp4");
    //movie.setLoopState(OF_LOOP_NORMAL);
    //movie.play();

    movieFile = settings.getValue("settings:movie_file", "test.mp4");

    string videoPath = ofToDataPath("./" + movieFile, true);
    ofxOMXPlayerSettings omxSettings;
    omxSettings.videoPath = videoPath;
    omxSettings.useHDMIForAudio = true;    //default true
    omxSettings.enableTexture = true;      //default true
    omxSettings.enableLooping = true;      //default true
    omxSettings.enableAudio = false;        //default true, save resources by disabling
    //omxSettings.doFlipTexture = true;        //default false
    
    movie.setup(omxSettings);
    //movie.start();

    framerate = settings.getValue("settings:framerate", 60);
    width = settings.getValue("settings:width", 160);
    height = settings.getValue("settings:height", 120);
    ofSetFrameRate(framerate);

    host = settings.getValue("settings:host", "127.0.0.1");
    port = settings.getValue("settings:port", 7110);

    debug = (bool)settings.getValue("settings:debug", 1);

    if (sendMotionInfo) sender.setup(host, port);

    float rx = settings.getValue("settings:rot_scale_x", 1.0);
    float ry = settings.getValue("settings.rot_scale_y", 0.1);
    float px = settings.getValue("settings:pos_scale_x", -1.0);
    float py = settings.getValue("settings.pos_scale_y", 0.5);  
    rotScaler = vec2(rx, ry);
    posScaler = vec2(px, py);
    
    // ~ ~ ~   get a persistent name for this computer   ~ ~ ~
    compname = "RPi";
    file.open(ofToDataPath("compname.txt"), ofFile::ReadWrite, false);
    ofBuffer buff;
    if (file) { // use existing file if it's there
        buff = file.readToBuffer();
        compname = buff.getText();
    }
    else { // otherwise make a new one
        compname += "_" + ofGetTimestampString("%y%m%d%H%M%S%i");
        ofStringReplace(compname, "\n", "");
        ofStringReplace(compname, "\r", "");
        buff.set(compname.c_str(), compname.size());
        ofBufferToFile("compname.txt", buff);
    }
    std::cout << compname << endl;  

    triggerThreshold = settings.getValue("settings:trigger_threshold", 0.05);
    sendMotionInfo = (bool)settings.getValue("settings:send_motion_info", 1);
    counterMax = settings.getValue("settings:trigger_frames", 3);
    timeDelay = settings.getValue("settings:time_delay", 5000);
    counterDelay = settings.getValue("settings:counter_reset", 1000);

    // ~ ~ ~   cam settings   ~ ~ ~
    camSharpness = settings.getValue("settings:sharpness", 0);
    camContrast = settings.getValue("settings:contrast", 0);
    camBrightness = settings.getValue("settings:brightness", 50);
    camIso = settings.getValue("settings:iso", 300);
    camExposureMode = settings.getValue("settings:exposure_mode", 0);
    camExposureCompensation = settings.getValue("settings:exposure_compensation", 0);
    camShutterSpeed = settings.getValue("settings:shutter_speed", 0);

    camSettings.sensorWidth = width;
    camSettings.sensorHeight = height;
    camSettings.framerate = framerate;
    camSettings.enableTexture = true;
    camSettings.enablePixels = true;
    camSettings.autoISO = false;
    camSettings.autoShutter = false;
    camSettings.brightness = camBrightness;
    camSettings.sharpness = camSharpness;
    camSettings.contrast = camContrast;
    camSettings.ISO = camIso;
    camSettings.exposurePreset = camExposureMode;
    camSettings.evCompensation = camExposureCompensation;
    camSettings.shutterSpeed = camShutterSpeed;
    cam.setup(camSettings); 

    // ~ ~ ~   optical flow settings   ~ ~ ~
    useFarneback = (bool)settings.getValue("settings:dense_flow", 1);
    flowResetThreshold = settings.getValue("settings:flow_reset_threshold", 1.0);
    pyrScale = settings.getValue("settings:pyr_scale", 0.5);   // 0 to 1, default 0.5
    levels = settings.getValue("settings:levels", 4);   // 1 to 8, default 4
    winsize = settings.getValue("settings:win_size", 8);   // 4 to 64, default 8
    iterations = settings.getValue("settings:iterations", 2);   // 1 to 8, default 2
    polyN = settings.getValue("settings:poly_n", 7);   // 5 to 10, default 7
    polySigma = settings.getValue("settings:poly_sigma", 1.5);   // 1.1 to 2, default 
    OPTFLOW_FARNEBACK_GAUSSIAN = (bool)settings.getValue("settings:optflow_farneback_gaussian", 0); // default false
    winSize = settings.getValue("settings:win_size", 32);   // 4 to 64, default 32
    maxLevel = settings.getValue("settings:max_level", 3);   // 0 to 8, default 3
    maxFeatures = settings.getValue("settings:max_features", 200);   // 1 to 1000, default 200
    qualityLevel = settings.getValue("settings:quality_level", 0.01);   // 0.001 to 0.02, default 0.01
    minDistance = settings.getValue("settings:min_distance", 4);   // 1 to 16, default 4

    motionVal = 0;
    counterOn = 0;
    markTriggerTime = 0;
    trigger = false;
    isMoving = false;

    // ~ ~ ~ ~ ~ ~ ~ 

    //ofEnableDepthTest();
    shader.load("shader");

    //rgb.allocate(2048, 2048, OF_IMAGE_COLOR);
    //rgb.loadImage("rgb.jpg");
    //depth.allocate(512, 512, OF_IMAGE_GRAYSCALE);
    //depth.loadImage("depth.png");
    
    plane.set(ofGetWidth(), ofGetHeight(), planeResX, planeResY);
    //plane.mapTexCoords(movie.getWidth(), movie.getHeight(), 1, 1);
    plane.mapTexCoordsFromTexture(movie.getTextureReference());

    posOffset = vec2(ofGetWidth() / 2, ofGetHeight() / 2);
    pos = vec2(posOffset.x, posOffset.y);
}

//--------------------------------------------------------------
void ofApp::update() {
    frame =  ofxCv::toCv(cam.getPixels());

    if (!frame.empty()) {
        if (useFarneback) {
            curFlow = &farneback;
            farneback.setPyramidScale(pyrScale);
            farneback.setNumLevels(levels);
            farneback.setWindowSize(winsize);
            farneback.setNumIterations(iterations);
            farneback.setPolyN(polyN);
            farneback.setPolySigma(polySigma);
            farneback.setUseGaussian(OPTFLOW_FARNEBACK_GAUSSIAN);
        } else {
            curFlow = &pyrLk;
            pyrLk.setMaxFeatures(maxFeatures);
            pyrLk.setQualityLevel(qualityLevel);
            pyrLk.setMinDistance(minDistance);
            pyrLk.setWindowSize(winSize);
            pyrLk.setMaxLevel(maxLevel);
        }

        // you use Flow polymorphically
        curFlow->calcOpticalFlow(frame);

        if (useFarneback) {
            motionValRaw = farneback.getAverageFlow();
        } else {
            motionValRaw = glm::vec2(0, 0);
            auto points = pyrLk.getMotion();

            for (int i = 0; i < points.size(); i++) {
                motionValRaw.x += points[i].x;
                motionValRaw.y += points[i].y;
            }
            motionValRaw.x /= (float)points.size();
            motionValRaw.y /= (float)points.size();
        }

        motionVal = (abs(motionValRaw.x) + abs(motionValRaw.y)) / 2.0;

        isMoving = motionVal > triggerThreshold;

        if (debug) std::cout << "val: " << motionVal << " motion: " << isMoving << endl;

        int t = ofGetElapsedTimeMillis();

        // reset count if too much time has elapsed since the last change
        if (t > markCounterTime + counterDelay) counterOn = 0;

        // motion detection logic
        // 1. motion detected, but not triggered yet
        if (!trigger && isMoving) {
            if (counterOn < counterMax) { // start counting the ON frames
                counterOn++;
                markCounterTime = t;
            } else { // trigger is ON
                markTriggerTime = t;
                trigger = true;
            }
            // 2. motion is triggered
        } else if (trigger && isMoving) { // keep resetting timer as long as motion is detected
            markTriggerTime = t;
            // 3. motion no longer detected
        } else if (trigger && !isMoving && t > markTriggerTime + timeDelay) {
            trigger = false;
        }

        if (sendMotionInfo) sendOsc();
    }

    // optical flow can get stuck in feedback loops
    if (motionVal > flowResetThreshold) {
        curFlow->resetFlow();
        trigger = false;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    //if (!movie.isTextureEnabled()) return;

    ofBackground(0);

    // bind our texture. in our shader this will now be tex0 by default
    // so we can just go ahead and access it there.
    movie.getTextureReference().bind();

    shader.begin();

    ofPushMatrix();

    posTarget += motionValRaw * posScaler;

    //float px = ofClamp(ofLerp(pos.x, posTarget.x, posSpeed), -posRange.x, posRange.x);
    //float py = ofClamp(ofLerp(pos.y, posTarget.y, posSpeed), -posRange.y, posRange.y);
    //pos = vec2(px, py);
    pos = glm::clamp(glm::lerp(pos, posTarget, posSpeed), -posRange, posRange);

    // translate plane into center screen.
    ofTranslate(pos.x + posOffset.x, pos.y + posOffset.y, zPos);
    ofScale(1, -1, 1);

    rotTarget += motionValRaw * rotScaler;

    //float rx = ofClamp(ofLerp(rot.x, rotTarget.x, rotSpeed), -rotRange.x, rotRange.x);
    //float ry = ofClamp(ofLerp(rot.y, rotTarget.y, rotSpeed), -rotRange.y, rotRange.y);
    //rot = vec2(rx, ry);
    rot = glm::clamp(glm::lerp(rot, rotTarget, rotSpeed), -rotRange, rotRange);

    ofRotateDeg(rot.x + rotOffset.x, 0, 1, 0);
    ofRotateDeg(rot.y + rotOffset.y, 1, 0, 0);

    plane.draw(); //drawWireframe();

    ofPopMatrix();

    posTarget = glm::lerp(posTarget, posOffsetOrig, returnSpeed);
    rotTarget = glm::lerp(rotTarget, rotOffsetOrig, returnSpeed);

    shader.end();

    if (debug) {
        ofSetColor(255);

        if (!frame.empty()) {
            curFlow->draw(0, 0);
        }

        stringstream info;
        info << "FPS: " << ofGetFrameRate() << endl;
        ofDrawBitmapStringHighlight(info.str(), 10, 10, ofColor::black, ofColor::yellow);
    }
}

void ofApp::sendOsc() {
    ofxOscMessage msg;

    msg.setAddress("/pihole");
    msg.addStringArg(compname);
    msg.addIntArg((int)trigger);

    msg.addFloatArg(motionVal); // total motion, always positive
    msg.addFloatArg(motionValRaw.x); // x change
    msg.addFloatArg(motionValRaw.y); // y change
    
    sender.sendMessage(msg);
    if (debug) std:cout << "*** SENT: " << trigger << " ***\n";
}
