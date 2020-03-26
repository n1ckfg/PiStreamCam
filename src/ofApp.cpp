#include "ofApp.h"

using namespace glm;
using namespace ofxHTTP;

//--------------------------------------------------------------
void ofApp::setup() {
    settings.loadFile("settings.xml");

    ofSetVerticalSync(false);
    ofHideCursor();

    framerate = settings.getValue("settings:framerate", 60);
    width = settings.getValue("settings:width", 160);
    height = settings.getValue("settings:height", 120);
    ofSetFrameRate(framerate);

    pixels.allocate(width, height, OF_IMAGE_COLOR);
    tex.allocate(width, height,  GL_RGBA);

    host = settings.getValue("settings:host", "127.0.0.1");
    port = settings.getValue("settings:port", 7110);

    debug = (bool)settings.getValue("settings:debug", 1);
   
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
    camSettings.enableTexture = false;//true;
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

    streamSettings.setPort(port);
    streamSettings.ipVideoRouteSettings.setMaxClientConnections(1); // default 5
    server.setup(settings);
    server.start();

}

//--------------------------------------------------------------
void ofApp::update() {
	if (cam.isFrameNew()) {
 		pixels = cam.getPixels();
 		tex.loadData(pixels, settings.sensorWidth, settings.sensorHeight, GL_RGBA);
 		server.send(pixels);
 	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	tex.draw(0, 0, ofGetWidth(), ofGetHeight());
}

