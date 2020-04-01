#pragma once

#include "ofMain.h"
#include "ofxOMXPhotoGrabber.h"
#include "ofxXmlSettings.h"
#include "ofxHTTP.h"

using namespace glm;
using namespace ofxHTTP;

class ofApp : public ofBaseApp {

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

		SimpleIPVideoServer server;
    	SimpleIPVideoServerSettings streamSettings;

		int framerate;

};
