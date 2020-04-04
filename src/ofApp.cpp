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
    postPort = settings.getValue("settings:post_port", 7110);
    streamPort = settings.getValue("settings:stream_port", 7111);
    wsPort = settings.getValue("settings:stream_port", 7112);

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
    //camSettings.enablePixels = true;
    camSettings.enableTexture = true;
    camSettings.autoISO = false;
    camSettings.autoShutter = false;
    camSettings.savedPhotosFolderName = "DocumentRoot/photos"; // default "photos"
    camSettings.photoGrabberListener = this; //not saved in JSON file
    cam.setup(camSettings);
    
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/include/ofx/HTTP/IPVideoRoute.h
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/src/IPVideoRoute.cpp
    streamSettings.setPort(streamPort);
    streamSettings.ipVideoRouteSettings.setMaxClientConnections(settings.getValue("settings:max_stream_connections", 5)); // default 5
    streamSettings.ipVideoRouteSettings.setMaxClientBitRate(settings.getValue("settings:max_stream_bitrate", 512)); // default 1024
    streamSettings.ipVideoRouteSettings.setMaxClientFrameRate(settings.getValue("settings:max_stream_framerate", 30)); // default 30
    streamSettings.ipVideoRouteSettings.setMaxClientQueueSize(settings.getValue("settings:max_stream_queue", 10)); // default 10
    streamSettings.ipVideoRouteSettings.setMaxStreamWidth(width); // default 1920
    streamSettings.ipVideoRouteSettings.setMaxStreamHeight(height); // default 1080
    streamSettings.fileSystemRouteSettings.setDefaultIndex("live_view.html");
    streamServer.setup(streamSettings);
    streamServer.start();

    shader.load("shaders/es/invert");
    fbo.allocate(width, height, GL_RGBA);
    pixels.allocate(width, height, OF_IMAGE_COLOR);

    // https://bakercp.github.io/ofxHTTP/classofx_1_1_h_t_t_p_1_1_simple_post_server_settings.html
    // https://github.com/bakercp/ofxHTTP/blob/master/libs/ofxHTTP/src/PostRoute.cpp
    postSettings.setPort(postPort);
    postSettings.postRouteSettings.setUploadRedirect("result.html");
    postServer.setup(postSettings);
    postServer.postRoute().registerPostEvents(this);
    postServer.start();

    ofSystem("cp /etc/hostname " + ofToDataPath("DocumentRoot/js/"));

    // websockets
    wsSettings.setPort(wsPort);
    wsServer.setup(wsSettings);
    wsServer.start();
}

//--------------------------------------------------------------
void ofApp::update() {
    if (cam.isFrameNew() && cam.isTextureEnabled()) {
        fbo.begin();
        if (doShader) shader.begin();
        cam.draw(0,0);
        if (doShader) shader.end();
        fbo.end();
        fbo.readToPixels(pixels);
        streamServer.send(pixels);

        //if (firstRun) {
            //cam.takePhoto();
            //firstRun = false;
        //}
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    if (debug && cam.isTextureEnabled()) {
        fbo.draw(0, 0);
    } 
}

void ofApp::onTakePhotoComplete(string fileName) {
    ofLog() << "onTakePhotoComplete fileName: " << fileName;  

    createResultHtml(fileName);
    doShader = false;
}

void ofApp::onHTTPPostEvent(ofxHTTP::PostEventArgs& args) {
    ofLogNotice("ofApp::onHTTPPostEvent") << "Data: " << args.getBuffer().getText();
    cam.takePhoto();
    createResultHtml("none");
    doShader = true;
}


void ofApp::onHTTPFormEvent(ofxHTTP::PostFormEventArgs& args) {
    ofLogNotice("ofApp::onHTTPFormEvent") << "";
    ofxHTTP::HTTPUtils::dumpNameValueCollection(args.getForm(), ofGetLogLevel());
    cam.takePhoto();
    createResultHtml("none");
    doShader = true;
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

void ofApp::createResultHtml(string fileName) {
    string photoIndexFileName = "DocumentRoot/result.html";
    ofBuffer buff;
    ofFile photoIndexFile;
    photoIndexFile.open(ofToDataPath(photoIndexFileName), ofFile::ReadWrite, false);

    string photoIndex = "<!DOCTYPE html>\n";
    
    if (fileName == "none") { // use existing file if it's there
        photoIndex += "<html><head><meta http-equiv=\"refresh\" content=\"0\"></head><body>\n";
        photoIndex += "READY\n";
    } else { // otherwise make a new one
        photoIndex += "<html><head></head><body>\n";
        string shortName = ofFilePath::getFileName(fileName);
        photoIndex += "<a href=\"photos/" + shortName + "\">" + shortName + "</a>\n";
    }

    photoIndex += "</body></html>\n";

    buff.set(photoIndex.c_str(), photoIndex.size());
    ofBufferToFile(photoIndexFileName, buff);
}