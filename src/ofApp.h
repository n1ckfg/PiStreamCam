#pragma once

#include "ofMain.h"
#include "ofxOMXPhotoGrabber.h"
#include "ofxXmlSettings.h"
#include "ofxHTTP.h"

class ofApp : public ofBaseApp, public ofxOMXPhotoGrabberListener {

	public:	
		void setup();
		void update();
		void draw();
		
		int width, height;
		bool firstRun = true;
		
		string compname;
		string host; // hostname;
		int port; // default 7110;

		bool debug; // draw to local screen, default true

		ofFile file;
		ofxXmlSettings settings;

		ofxOMXPhotoGrabber stillCam;
 	    ofxOMXCameraSettings stillCamSettings;
		ofImage img;

		ofxHTTP::SimpleIPVideoServer server;
    	ofxHTTP::SimpleIPVideoServerSettings streamSettings;

		int framerate;

	    vector<string>photoFiles;
	    void onTakePhotoComplete(string fileName) override;
	    void onPhotoGrabberEngineStart()override 
	    {
	        
	    };
	    
};
