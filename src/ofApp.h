#pragma once

#include "ofMain.h"
#include "ofxUIUtils.h"
#include "ofxSVGPlus.h"
#include "ofxOpenCv.h"

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
		void drawShape(int width, int height, float multiplyer);
		void addVertex(float x, float y, float angle);
		void gotMessage(ofMessage msg);

		void UIButPressed(const pair<bool, int>& state);
		void UISliderMoved(const int & id);
		void DropDown(const pair<string, int> & item);

		float angleOfLine(ofPoint P1, ofPoint P2);

		ofxUIButton saveBut, resetBut;
		ofxUISlider<int> sideSlider;
		ofxDropDownMenu windMenu;

		ofPolyline shapeL;

		int sides = 6;
		bool ReflectMode = true;
		int sidelen;

		vector<float> xValues;
		vector<float> yValues;
		int SideLength;
		ofColor circleColor, symColor;
		int CircleDia;
		bool dragging;
		int MovePoint;
		
		ofxSVGPlus dSvg;
		ofTrueTypeFont f;

		SVG_FILL_RULE fR;
		bool savedF;

		long fadetime;

		ofxCvColorImage			colorImg, scolorImg;

		ofxCvGrayscaleImage 	grayImage, sgrayImage;

		ofxCvContourFinder 	contourFinder;

		ofFbo myFbo, smFbo;

		float multip;
};
