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
    streamPort = settings.getValue("settings:stream_port", 7111);

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
        camSettings.parseJSON(jsonBuffer.getText());
    } else {
        camSettings.sensorWidth = 2592;
        camSettings.sensorHeight = 1944;       
        camSettings.stillPreviewWidth = width;
        camSettings.stillPreviewHeight = height;        
        camSettings.saturation = -100;
        camSettings.sharpness = 100;
        camSettings.brightness = 50;
        camSettings.stillQuality = 100;
        camSettings.enableStillPreview = true;
        camSettings.burstModeEnabled = true;
        camSettings.saveJSONFile();   
    }
    
    // *** override settings ***
    // https://github.com/jvcleave/ofxOMXCamera/blob/master/src/ofxOMXCameraSettings.h
    camSettings.stillPreviewWidth = width;
    camSettings.stillPreviewHeight = height;
    camSettings.enablePixels = true;
    camSettings.enableTexture = true;
    camSettings.autoISO = false;
    camSettings.autoShutter = false;
    camSettings.savedPhotosFolderName = "DocumentRoot/photos"; // default "photos"
    camSettings.photoGrabberListener = this; //not saved in JSON file
    cam.setup(camSettings);
    
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/include/ofx/HTTP/IPVideoRoute.h
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/src/IPVideoRoute.cpp
    streamSettings.setPort(streamPort);
    streamSettings.ipVideoRouteSettings.setMaxClientConnections(settings.getValue("settings:max_stream_connections", 1)); // default 5
    streamSettings.ipVideoRouteSettings.setMaxClientBitRate(settings.getValue("settings:max_stream_bitrate", 512)); // default 1024
    streamSettings.ipVideoRouteSettings.setMaxClientFrameRate(settings.getValue("settings:max_stream_framerate", 30)); // default 30
    streamSettings.ipVideoRouteSettings.setMaxClientQueueSize(settings.getValue("settings:max_stream_queue", 10)); // default 10
    streamSettings.ipVideoRouteSettings.setMaxStreamWidth(width); // default 1920
    streamSettings.ipVideoRouteSettings.setMaxStreamHeight(height); // default 1080
    streamSettings.fileSystemRouteSettings.setDefaultIndex("live_view.html");
    streamServer.setup(streamSettings);
    streamServer.start();

    img.allocate(width, height, OF_IMAGE_COLOR);

    postSettings.setPort(port);
    postServer.setup(postSettings);
    postServer.postRoute().registerPostEvents(this);
    postServer.start();
}

//--------------------------------------------------------------
void ofApp::update() {
    if (cam.isFrameNew()) {
        img.grabScreen(0,0,width, height);
        streamServer.send(img.getPixels());

        if (firstRun) {
            cam.takePhoto();
            firstRun = false;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    if (debug && cam.isTextureEnabled()) {
        cam.draw(0, 0);
    } 
}

void ofApp::onTakePhotoComplete(string fileName) {
    ofLog() << "onTakePhotoComplete fileName: " << fileName;  

    string photoIndexFileName = "DocumentRoot/result.html";
    ofFile photoIndexFile;
    string photoIndex;
    ofBuffer buff;
    photoIndexFile.open(ofToDataPath(photoIndexFileName), ofFile::ReadWrite, false);
    
    //if (photoIndexFile) { // use existing file if it's there
        //buff = photoIndexFile.readToBuffer();
        //photoIndex = buff.getText();
    //} else { // otherwise make a new one
    string shortName = ofFilePath::getFileName(fileName);
    photoIndex += "<a href=\"photos/" + shortName + "\">" + shortName + "</a>\n";
    
    buff.set(photoIndex.c_str(), photoIndex.size());
    ofBufferToFile(photoIndexFileName, buff);
    //}
}

void ofApp::onHTTPPostEvent(ofxHTTP::PostEventArgs& args) {
    ofLogNotice("ofApp::onHTTPPostEvent") << "Data: " << args.getBuffer().getText();
    cam.takePhoto();
}


void ofApp::onHTTPFormEvent(ofxHTTP::PostFormEventArgs& args) {
    ofLogNotice("ofApp::onHTTPFormEvent") << "";
    ofxHTTP::HTTPUtils::dumpNameValueCollection(args.getForm(), ofGetLogLevel());
    cam.takePhoto();
}


void ofApp::onHTTPUploadEvent(ofxHTTP::PostUploadEventArgs& args) {
    std::string stateString = "";

    switch (args.getState()) {
        case ofxHTTP::PostUploadEventArgs::UPLOAD_STARTING:
            stateString = "STARTING";
            break;
        case ofxHTTP::PostUploadEventArgs::UPLOAD_PROGRESS:
            stateString = "PROGRESS";
            break;
        case ofxHTTP::PostUploadEventArgs::UPLOAD_FINISHED:
            stateString = "FINISHED";
            break;
    }

    ofLogNotice("ofApp::onHTTPUploadEvent") << "";
    ofLogNotice("ofApp::onHTTPUploadEvent") << "         state: " << stateString;
    ofLogNotice("ofApp::onHTTPUploadEvent") << " formFieldName: " << args.getFormFieldName();
    ofLogNotice("ofApp::onHTTPUploadEvent") << "orig. filename: " << args.getOriginalFilename();
    ofLogNotice("ofApp::onHTTPUploadEvent") <<  "     filename: " << args.getFilename();
    ofLogNotice("ofApp::onHTTPUploadEvent") <<  "     fileType: " << args.getFileType().toString();
    ofLogNotice("ofApp::onHTTPUploadEvent") << "# bytes xfer'd: " << args.getNumBytesTransferred();
}