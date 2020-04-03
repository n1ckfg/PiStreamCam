#pragma once

#include "ofMain.h"
#include "ofAppEGLWindow.h"
#include "ofxOMXPhotoGrabber.h"
#include "ofxXmlSettings.h"
#include "ofxHTTP.h"
#include "ofxLibwebsockets.h"

class ofApp : public ofBaseApp, public ofxOMXPhotoGrabberListener {

	public:	
		void setup();
		void update();
		void draw();
		
		int width, height;
		
		string compname;
		string host; // hostname;
		int port, streamPort; // default 7110;

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

	    void createResultHtml(string filename);

};
