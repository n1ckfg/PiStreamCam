#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxOMXVideoGrabber.h"
#include "ofxOMXPlayer.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

using namespace glm;

class ofApp : public ofBaseApp {

	public:	
		void setup();
		void update();
		void draw();
		
	    ofShader shader;
	    ofPlanePrimitive plane;
		ofxOMXPlayer movie;
		string movieFile;

		vec2 pos = vec2(0, 0);
		vec2 rot = vec2(0, 0);
		vec2 rotRange = vec2(25, 15);
		float rotSpeed = 0.65;
		vec2 rotOffset = vec2(0, 0);
		vec2 rotOffsetOrig = vec2(rotOffset.x, rotOffset.y);
		vec2 rotTarget = vec2(rotOffset.x, rotOffset.y);
		vec2 rotScaler = vec2(1.0, 0.1);

		float posSpeed = 0.65;
		vec2 posRange = vec2(150, 10);
		vec2 posOffset = vec2(-10, 0);
		vec2 posOffsetOrig = vec2(posOffset.x, posOffset.y);
		vec2 posTarget = vec2(posOffset.x, posOffset.y);
		vec2 posScaler = vec2(-1.0, 0.5);

		float returnSpeed = 0.001;
		float zPos = 100;
		int planeResX = 192;
		int planeResY = 108;

		// ~ ~ ~ ~ ~ ~ ~ 

		string compname;
		string host; // hostname;
		int port; // default 7110;

		bool debug; // draw to local screen, default true

		ofFile file;
		ofxXmlSettings settings;

		// https://web.archive.org/web/20130520103418/http://opencv.willowgarage.com/documentation/cpp/motion_analysis_and_object_tracking.html
		ofxCv::FlowFarneback farneback;
		ofxCv::FlowPyrLK pyrLk;
		ofxCv::Flow* curFlow;
		int width;
		int height;
		float pyrScale;
		int levels;
		int winsize;
		int iterations;
		int polyN;
		float polySigma;
		bool OPTFLOW_FARNEBACK_GAUSSIAN;
		bool useFarneback;
		bool sendMotionInfo;
		int winSize;
		int maxLevel;
		int maxFeatures;
		float qualityLevel;
		int minDistance;

		float triggerThreshold;
		float flowResetThreshold;
		int timeDelay;
		int counterDelay;
		int markTriggerTime;
		int markCounterTime;
		bool trigger;
		float motionVal;
		vec2 motionValRaw;
		bool isMoving;
		int counterOn;
		int counterMax;

		ofxOMXVideoGrabber cam;
		ofxOMXCameraSettings camSettings;
		cv::Mat frame;

		// for more camera settings, see:
		// https://github.com/orgicus/ofxCvPiCam/blob/master/example-ofxCvPiCam-allSettings/src/testApp.cpp

		int camShutterSpeed; // 0 to 330000 in microseconds, default 0
		int camSharpness; // -100 to 100, default 0
		int camContrast; // -100 to 100, default 0
		int camBrightness; // 0 to 100, default 50
		int camIso; // 100 to 800, default 300
		int camExposureCompensation; // -10 to 10, default 0;

		// 0 off, 1 auto, 2 night, 3 night preview, 4 backlight, 5 spotlight, 6 sports, 7, snow, 8 beach, 9 very long, 10 fixed fps, 11 antishake, 12 fireworks, 13 max
		string camExposureMode; // default off

		//string oscAddress;
		int framerate;
		//bool doDrawInfo;

		ofxOscSender sender;
		void sendOsc();

};
