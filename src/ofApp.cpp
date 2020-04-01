#include "ofApp.h"

using namespace glm;
using namespace ofxHTTP;

//--------------------------------------------------------------
void ofApp::setup() {
    settings.loadFile("settings.xml");

    ofSetVerticalSync(false);
    ofHideCursor();

    framerate = settings.getValue("settings:framerate", 60);
    width = settings.getValue("settings:width", 640);
    height = settings.getValue("settings:height", 480);
    ofSetFrameRate(framerate);

    host = settings.getValue("settings:host", "127.0.0.1");
    port = settings.getValue("settings:port", 7110);

    debug = (bool) settings.getValue("settings:debug", 1);
   
    // ~ ~ ~   get a persistent name for this computer   ~ ~ ~
    compname = "RPi";
    file.open(ofToDataPath("compname.txt"), ofFile::ReadWrite, false);
    ofBuffer buff;
    if (file) { // use existing file if it's there
        buff = file.readToBuffer();
        compname = buff.getText();
    } else { // otherwise make a new one
        compname += "_" + ofGetTimestampString("%y%m%d%H%M%S%i");
        ofStringReplace(compname, "\n", "");
        ofStringReplace(compname, "\r", "");
        buff.set(compname.c_str(), compname.size());
        ofBufferToFile("compname.txt", buff);
    }
    cout << compname << endl;  

    ofFile settingsFile("settings.json");
    if (settingsFile.exists()) {
        ofBuffer jsonBuffer = ofBufferFromFile("settings.json");
        stillCamSettings.parseJSON(jsonBuffer.getText());
    } else {
        stillCamSettings.sensorWidth = 2592;
        stillCamSettings.sensorHeight = 1944;
        
        stillCamSettings.stillPreviewWidth = 1280;
        stillCamSettings.stillPreviewHeight = 720;
        
        //stillCamSettings.stillPreviewWidth = cameraSettings.sensorWidth;
        //stillCamSettings.stillPreviewHeight = cameraSettings.height;
        stillCamSettings.saturation = -100;
        stillCamSettings.sharpness = 100;
        //stillCamSettings.brightness = 75;
        stillCamSettings.stillQuality = 100;
        stillCamSettings.enableStillPreview = true;
        stillCamSettings.burstModeEnabled = true;
        stillCamSettings.saveJSONFile();
    }
    
    
    //stillCamSettings.photoGrabberListener = this; //not saved in JSON file
    stillCam.setup(stillCamSettings);

    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/include/ofx/HTTP/IPVideoRoute.h
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/src/IPVideoRoute.cpp
    streamSettings.setPort(port);
    streamSettings.ipVideoRouteSettings.setMaxClientConnections(settings.getValue("settings:max_stream_connections", 1)); // default 5
    streamSettings.ipVideoRouteSettings.setMaxClientBitRate(settings.getValue("settings:max_stream_bitrate", 512)); // default 1024
    streamSettings.ipVideoRouteSettings.setMaxClientFrameRate(settings.getValue("settings:max_stream_framerate", 30)); // default 30
    streamSettings.ipVideoRouteSettings.setMaxClientQueueSize(settings.getValue("settings:max_stream_queue", 10)); // default 10
    streamSettings.ipVideoRouteSettings.setMaxStreamWidth(width); // default 1920
    streamSettings.ipVideoRouteSettings.setMaxStreamHeight(height); // default 1080
    server.setup(streamSettings);
    server.start();

    //img.allocate(width, height, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update() {
    server.send(stillCam.getPixels());
}

//--------------------------------------------------------------
void ofApp::draw() {
	//img.draw(0, 0, ofGetWidth(), ofGetHeight());

    stillCam.draw(0, 0, ofGetWidth(), ofGetHeight());

    if (firstRun) {
        stillCam.takePhoto(10);
        firstRun = false;
    }
}

