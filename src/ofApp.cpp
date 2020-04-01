#include "ofApp.h"

using namespace glm;
using namespace ofxHTTP;
using namespace cv;
using namespace ofxCv;

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
    }
    else { // otherwise make a new one
        compname += "_" + ofGetTimestampString("%y%m%d%H%M%S%i");
        ofStringReplace(compname, "\n", "");
        ofStringReplace(compname, "\r", "");
        buff.set(compname.c_str(), compname.size());
        ofBufferToFile("compname.txt", buff);
    }
    std::cout << compname << endl;  

    cam.setup(width, height, (bool) settings.getValue("settings:video_color", 1)); // color/gray;

    // ~ ~ ~   cam settings   ~ ~ ~
    camSharpness = settings.getValue("settings:sharpness", 0);
    camContrast = settings.getValue("settings:contrast", 0);
    camBrightness = settings.getValue("settings:brightness", 50);
    camIso = settings.getValue("settings:iso", 300);
    camExposureMode = settings.getValue("settings:exposure_mode", 0);
    camExposureCompensation = settings.getValue("settings:exposure_compensation", 0);
    camShutterSpeed = settings.getValue("settings:shutter_speed", 0);

    cam.setSharpness(camSharpness);
    cam.setContrast(camContrast);
    cam.setBrightness(camBrightness);
    cam.setISO(camIso);
    cam.setExposureMode((MMAL_PARAM_EXPOSUREMODE_T)camExposureMode);
    cam.setExposureCompensation(camExposureCompensation);
    cam.setShutterSpeed(camShutterSpeed);

    // https://github.com/jvcleave/ofxOMXCamera/blob/master/example-still/src/ofApp.cpp
    stillCamSettings.sensorWidth = 2592;
    stillCamSettings.sensorHeight = 1944;    
    stillCamSettings.stillPreviewWidth = 1280;
    stillCamSettings.stillPreviewHeight = 720;
    //stillCamSettings.stillPreviewWidth = stillCamSettings.sensorWidth;
    //stillCamSettings.stillPreviewHeight = stillCamSettings.height;
    stillCamSettings.saturation = -100;
    stillCamSettings.sharpness = 100;
    //stillCamSettings.brightness = 75;
    stillCamSettings.stillQuality = 100;
    stillCamSettings.enableStillPreview = true;
    stillCamSettings.burstModeEnabled = true;
    stillCamSettings.photoGrabberListener = this;
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

    img.allocate(width, height, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update() {
    frame = cam.grab();

    if (!frame.empty()) {	
        toOf(frame, img.getPixelsRef());	
		server.send(img.getPixels());
 	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	img.draw(0, 0, ofGetWidth(), ofGetHeight());
}

void ofApp::onTakePhotoComplete(string fileName) {
    ofLog() << "onTakePhotoComplete fileName: " << fileName;
    
    int currentCompression = stillCam.settings.stillQuality;
    
    if(currentCompression-1 > 0) {
        currentCompression--;
    } else {
        currentCompression = 100;
    }
    stillCam.setImageFilter(filterCollection.getNextFilter());
    stillCam.setJPEGCompression(currentCompression);   
}