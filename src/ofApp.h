#pragma once

#include "ofMain.h"
#include "ofxUIUtils.h"
#include "ofxSVG.h"
#include "ofxOpenCv.h"
#include "ofxCvHoughLines.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void drawVertex(float x, float y, float angle);
		bool overCircle(int x, int y, int diameter);
		void clearShape();
		void drawShape();
		void UIButPressed(const pair<bool, int>& state);
		void addVertex(float x, float y, float angle);
		void gotMessage(ofMessage msg);
		void UISliderMoved(const int & id);
		void DropDown(const pair<string, int> & item);

		float angleOfLine(ofPoint P1, ofPoint P2);

		ofxUIButton saveBut, resetBut;
		ofxUISlider<int> sideSlider;
		ofxDropDownMenu windMenu;

		ofPolyline shapeL;

		int sides = 6;
		bool ReflectMode = true;

		vector<float> xValues;
		vector<float> yValues;
		int SideLength;
		ofColor circleColor, circleStroke, bgColor, symColor, segmentColor, emptyCircle;
		int CircleDia;
		int mtCircleDia;
		bool dragging;
		int MovePoint;
		
		ofxSVG dSvg;
		ofTrueTypeFont f;

		SVG_FILL_RULE fR;
		bool savedF, bSimplify;

		long fadetime;

		ofxCvColorImage			colorImg;

		ofxCvGrayscaleImage 	grayImage;

		ofxCvContourFinder 	contourFinder;

		ofFbo myFbo;
		float threshold;

		ofxCvHoughLines houghLines;
};
