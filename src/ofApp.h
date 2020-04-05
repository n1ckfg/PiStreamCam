#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ofxOMXPhotoGrabber.h"
#include "ofxXmlSettings.h"
#include "ofxHTTP.h"
#include "ofxJSONElement.h"

#define NUM_MESSAGES 30 // how many past ws messages we want to keep

class ofApp : public ofBaseApp, public ofxOMXPhotoGrabberListener {

	public:	
		void setup();
		void update();
		void draw();
		
		int width, height;
		
		string compname;
		string host; // hostname;
		int postPort, streamPort, wsPort;

		bool debug; // draw to local screen, default true

		ofFile file;
		ofxXmlSettings settings;

		ofxOMXPhotoGrabber cam;
 	    ofxOMXCameraSettings camSettings;
		ofFbo fbo;
		ofPixels pixels;
		ofShader shader;
		bool doShader = false;
		int framerate;

	    void createResultHtml(string fileName);
	    void beginTakePhoto();
	    void endTakePhoto(string fileName);

		ofxHTTP::SimpleIPVideoServer streamServer;
    	ofxHTTP::SimpleIPVideoServerSettings streamSettings;
	    vector<string> photoFiles;
    	void onTakePhotoComplete(string fileName) override;
	    void onPhotoGrabberEngineStart()override 
	    {
	        
	    };

	    ofxHTTP::SimplePostServer postServer;
		ofxHTTP::SimplePostServerSettings postSettings;
	    void onHTTPPostEvent(ofxHTTP::PostEventArgs& evt);
	    void onHTTPFormEvent(ofxHTTP::PostFormEventArgs& evt);
	    void onHTTPUploadEvent(ofxHTTP::PostUploadEventArgs& evt);
        
        ofxHTTP::SimpleWebSocketServer wsServer;  
		ofxHTTP::SimpleWebSocketServerSettings wsSettings;
	    void onWebSocketOpenEvent(ofxHTTP::WebSocketEventArgs& evt);
	    void onWebSocketCloseEvent(ofxHTTP::WebSocketCloseEventArgs& evt);
	    void onWebSocketFrameReceivedEvent(ofxHTTP::WebSocketFrameEventArgs& evt);
	    void onWebSocketFrameSentEvent(ofxHTTP::WebSocketFrameEventArgs& evt);
	    void onWebSocketErrorEvent(ofxHTTP::WebSocketErrorEventArgs& evt);

};
