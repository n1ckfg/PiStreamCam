#include "ofApp.h"

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
        stillCamSettings.stillPreviewWidth = width;
        stillCamSettings.stillPreviewHeight = height;        
        //stillCamSettings.saturation = -100;
        stillCamSettings.sharpness = 100;
        //stillCamSettings.brightness = 75;
        stillCamSettings.stillQuality = 100;
        stillCamSettings.enableStillPreview = true;
        stillCamSettings.burstModeEnabled = true;
        stillCamSettings.saveJSONFile();
    }
    
    // override settings
    // https://github.com/jvcleave/ofxOMXCamera/blob/master/src/ofxOMXCameraSettings.h
    stillCamSettings.stillPreviewWidth = width;
    stillCamSettings.stillPreviewHeight = height;
    stillCamSettings.enablePixels = true;
    stillCamSettings.enableTexture = true;
    stillCamSettings.savedPhotosFolderName = "DocumentRoot/photos"; // default "photos"
    stillCamSettings.photoGrabberListener = this; //not saved in JSON file
    
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
    if (stillCam.isFrameNew()) {
        img.grabScreen(0,0,width, height);
        server.send(img.getPixels());

        if (firstRun) {
            stillCam.takePhoto();
            firstRun = false;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    if (debug && stillCam.isTextureEnabled()) {
        stillCam.draw(0, 0);
    } 
}

void ofApp::onTakePhotoComplete(string fileName) {
    ofLog() << "onTakePhotoComplete fileName: " << fileName;  

    string photoIndexFileName = "DocumentRoot/result.html";
    ofFile photoIndexFile;
    string photoIndex;
    ofBuffer buff;
    photoIndexFile.open(ofToDataPath(photoIndexFileName), ofFile::ReadWrite, false);
    
    if (photoIndexFile) { // use existing file if it's there
        buff = photoIndexFile.readToBuffer();
        photoIndex = buff.getText();
    } else { // otherwise make a new one
        string shortName = ofFilePath::getFileName(fileName);
        photoIndex += "<a href=\"" + fileName + "\">" + shortName + "</a>\n";
        
        buff.set(photoIndex.c_str(), photoIndex.size());
        ofBufferToFile(photoIndexFileName, buff);
    }
}