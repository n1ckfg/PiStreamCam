#pragma once

#include "ofMain.h"
#include "ofxOMXVideoGrabber.h"
#include "ofxXmlSettings.h"

using namespace glm;

class ofApp : public ofBaseApp {

	public:	
		void setup();
		void update();
		void draw();
		
		int width, height;

		string compname;
		string host; // hostname;
		int port; // default 7110;

		bool debug; // draw to local screen, default true

		ofFile file;
		ofxXmlSettings settings;

		ofxOMXVideoGrabber cam;
		ofxOMXCameraSettings camSettings;

		int camShutterSpeed; // 0 to 330000 in microseconds, default 0
		int camSharpness; // -100 to 100, default 0
		int camContrast; // -100 to 100, default 0
		int camBrightness; // 0 to 100, default 50
		int camIso; // 100 to 800, default 300
		int camExposureCompensation; // -10 to 10, default 0;

		// 0 off, 1 auto, 2 night, 3 night preview, 4 backlight, 5 spotlight, 6 sports, 7, snow, 8 beach, 9 very long, 10 fixed fps, 11 antishake, 12 fireworks, 13 max
		string camExposureMode; // default off

		int framerate;

};
