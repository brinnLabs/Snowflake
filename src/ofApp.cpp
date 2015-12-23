#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(70, 70, 200);
	ofEnableSmoothing();
	ofSetEscapeQuitsApp(false);

	ofSetPolyMode(ofPolyWindingMode::OF_POLY_WINDING_NONZERO);
	fR = SVG_FILL_RULE::NONZERO;

	ofSetLogLevel(OF_LOG_VERBOSE);

	dragging = false;
	savedF = false;

	xValues.clear();
	xValues.push_back(0);
	xValues.push_back(0);
	yValues.clear();
	yValues.push_back(0);
	yValues.push_back(0);

	symColor.set(255, 255, 255, 176);
	CircleDia = 15;
	circleColor.set(255, 128, 0);

	sidelen = ofGetHeight() - 50;
	SideLength = sidelen * 9 / 20;

	clearShape();

	ofSetLineWidth(1);

	f.load(OF_TTF_SANS, 24);

	saveBut.setPosition(20, 700);
	saveBut.setFont(f);
	saveBut.setTitle("Save");
	saveBut.setButtonStyle(UI_BUTTON_RECT);
	saveBut.setWidth(80);
	saveBut.setID(1);
	saveBut.setHeight(40);

	resetBut.setPosition(20, 600);
	resetBut.setFont(f);
	resetBut.setTitle("Clear");
	resetBut.setButtonStyle(UI_BUTTON_RECT);
	resetBut.setWidth(80);
	resetBut.setHeight(40);
	resetBut.setID(2);

	sideSlider.setup(6, 3, 20, 20, 400);

	windMenu.addMenuItem("Non-Zero Winding");
	windMenu.addMenuItem("Even-Odd Winding");
	windMenu.setButtonFont(f);
	windMenu.setPosition(20, 20);
	windMenu.setAutoSizing(true);
	windMenu.setHeight(35);

	windMenu.setButtonTitle("Non-Zero Winding");

	multip = 4;
	colorImg.allocate(sidelen * multip, sidelen * multip);
	grayImage.allocate(sidelen * multip, sidelen * multip);
	myFbo.allocate(sidelen * multip, sidelen * multip, GL_RGB);

	scolorImg.allocate(sidelen, sidelen);
	sgrayImage.allocate(sidelen, sidelen);
	smFbo.allocate(sidelen, sidelen, GL_RGB);

	ofAddListener(resetBut.buttonEvent, this, &ofApp::UIButPressed);
	ofAddListener(saveBut.buttonEvent, this, &ofApp::UIButPressed);
	ofAddListener(sideSlider.sliderEvent, this, &ofApp::UISliderMoved);
	ofAddListener(windMenu.menuEvent, this, &ofApp::DropDown);
}

//--------------------------------------------------------------
void ofApp::update() {
	shapeL.clear();
	float angle;
	int j;

	for (int i = 0; i < sides; i++) {
		angle = -i*TWO_PI / sides;

		if (ReflectMode) {
			std::reverse(xValues.begin(), xValues.end());
			std::reverse(yValues.begin(), yValues.end());
			j = 0;
			while (j < xValues.size())
			{
				addVertex(-1 * xValues[j], yValues[j], angle);
				j++;
			}

			std::reverse(xValues.begin(), xValues.end());
			std::reverse(yValues.begin(), yValues.end());

			j = 0;

			while (j < xValues.size())
			{
				addVertex(xValues[j], yValues[j], angle);
				j++;
			}
		}
		else {
			j = 0;
			while (j < xValues.size())
			{
				addVertex(xValues[j], yValues[j], angle);
				j++;
			}
		}
	}
	if (fR == SVG_FILL_RULE::NONZERO) {
		smFbo.begin();
		ofSetColor(0);
		ofRect(0, 0, smFbo.getWidth(), smFbo.getHeight());
		ofSetColor(255);
		ofTranslate(smFbo.getWidth() / 2, smFbo.getHeight() / 2);
		drawShape(smFbo.getWidth(), smFbo.getHeight(), 1);
		smFbo.end();

		ofPixels p;
		p.allocate(smFbo.getWidth(), smFbo.getHeight(), ofImageType::OF_IMAGE_COLOR);
		smFbo.readToPixels(p);
		scolorImg.setFromPixels(p);

		sgrayImage = scolorImg;

		sgrayImage.threshold(80);


		contourFinder.findContours(sgrayImage, 1, (smFbo.getWidth() * smFbo.getHeight()), 200, true);	// find holes

	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	int i, xt, yt;

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	ofSetLineWidth(1);
	drawShape(sidelen, sidelen, 1);
	ofPopMatrix();

	ofSetLineWidth(1);

	ofPushStyle();
	i = 0;
	while (i < xValues.size())
	{
		{

			ofPushMatrix();
			ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
			ofSetColor(circleColor);
			ofEllipse(xValues[i], yValues[i], CircleDia, CircleDia);
			ofPopMatrix();
		}
		if ((i + 1) < xValues.size())
		{
			xt = (int)(xValues[i] + xValues[i + 1]) / 2;
			yt = (int)(yValues[i] + yValues[i + 1]) / 2;
			{
				ofPushMatrix();
				ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
				ofEllipse(xt, yt, CircleDia, CircleDia);
				ofPopMatrix();
			}
		}
		i++;
	}
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::red);
	ofPushMatrix();
	if (fR == SVG_FILL_RULE::EVEN_ODD) {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
		shapeL.draw();
	}
	else {
		ofTranslate(ofGetWidth() / 2 - sidelen/2, ofGetHeight() / 2 - sidelen/2);
		for (auto b : contourFinder.blobs) {
			ofSetColor(ofColor::red);
			ofPolyline tempP;
			tempP.addVertices(b.pts);
			tempP.draw();
		}
	}
	ofPopMatrix();

	if (savedF) {
		ofSetColor(255, 100, 100, 255 - (ofGetElapsedTimeMillis() - fadetime) / 18);
		f.drawString("Saved SVG", 30, 680);
		if ((ofGetElapsedTimeMillis() - fadetime) / 1000.0 > 5)
			savedF = false;
	}


	saveBut.draw();
	resetBut.draw();
	windMenu.draw();

	ofSetColor(symColor);
	ofDrawBitmapString("Sides: " + ofToString(sides), 20, 390);
	sideSlider.draw();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	int xt, yt;
	float xf, yf;
	if (dragging)
	{
		xValues[MovePoint] = mouseX - ofGetWidth() / 2;
		yValues[MovePoint] = mouseY - ofGetHeight() / 2;
		xt = mouseX;
		yt = mouseY;

	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	int i, xt, len, yt;
	dragging = false;

	len = xValues.size();

	i = 0;
	while (i < len)
	{
		if (overCircle((int)xValues[i] + ofGetWidth() / 2, (int)yValues[i] + ofGetHeight() / 2, CircleDia))
		{
			dragging = true;
			MovePoint = i;
			i = len; // break
		}
		i++;
	}



	if (dragging == false)
	{
		i = 0;
		while ((i + 1) < len)
		{
			xt = (int)(xValues[i] + xValues[i + 1]) / 2;
			yt = (int)(yValues[i] + yValues[i + 1]) / 2;

			if (overCircle(xt + ofGetWidth() / 2, yt + ofGetHeight() / 2, CircleDia))
			{
				dragging = true;
				MovePoint = i + 1;

				xValues.insert(xValues.begin() + i + 1, xt);
				yValues.insert(yValues.begin() + i + 1, yt);
				i = len; // break    
			}
			i++;
		}
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	dragging = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}


void ofApp::drawVertex(float x, float y, float angle)
{    // Angle is in radians

	ofVertex(x*cos(angle) + y*sin(angle), y*cos(angle) - x*sin(angle));
}

bool ofApp::overCircle(int x, int y, int diameter)
{
	float disX = x - mouseX;
	float disY = y - mouseY;
	if (sqrt(pow(disX, 2) + pow(disY, 2)) < diameter / 2) {
		return true;
	}
	else {
		return false;
	}
}

void ofApp::clearShape() {

	// This sets the initial shape and draws it on the screen,
	// with a triangular section highlighted.

	xValues.clear();
	xValues.push_back(0);
	xValues.push_back(0);
	yValues.clear();
	yValues.push_back(0);
	yValues.push_back(0);

	xValues[0] = 0;
	xValues[1] = SideLength*sqrt(3) / 4; //hexagon
	//xValues[1] = SideLength*sqrt(2) / 4; //pentagon
	//xValues[1] = SideLength*1 / 4;

	yValues[0] = -SideLength;
	yValues[1] = -0.75*SideLength;
	//yValues[1] = -0.25*SideLength;
}



void ofApp::drawShape(int width, int height, float multiplyer)
{

	float angle;
	int i, j;
	ofSetColor(symColor);
	ofBeginShape();
	i = 0;
	while (i < sides)
	{

		angle = -i*TWO_PI / sides;

		if (ReflectMode) {
			std::reverse(xValues.begin(), xValues.end());
			std::reverse(yValues.begin(), yValues.end());
			j = 0;
			while (j < xValues.size())
			{
				drawVertex(-1 * xValues[j] * multiplyer, yValues[j] * multiplyer, angle);
				j++;
			}

			std::reverse(xValues.begin(), xValues.end());
			std::reverse(yValues.begin(), yValues.end());

			j = 0;

			while (j < xValues.size())
			{
				drawVertex(xValues[j] * multiplyer, yValues[j] * multiplyer, angle);
				j++;
			}
			i++;
		}
		else {
			j = 0;
			while (j < xValues.size())
			{
				drawVertex(xValues[j] * multiplyer, yValues[j] * multiplyer, angle);
				j++;
			}
			i++;
		}


	}
	ofEndShape();

}

void ofApp::UIButPressed(const pair<bool, int> & state) {

	if (state.second == resetBut.getID()) {
		clearShape();
	}
	if (state.second == saveBut.getID() && state.first == false) {

		ofxSVGPlus _svg;
		_svg.pushGroup();
		//_svg.fillRule(fR);
		_svg.fill("none");
		_svg.stroke("#000000", 1);
		if (fR == SVG_FILL_RULE::EVEN_ODD) {
			_svg.beginPolygon();
			float hx = ofGetWidth() / 2.0;
			float hy = ofGetHeight() / 2.0;
			for (auto v : shapeL.getVertices())
				_svg.vertex(v.x + hx, v.y + hy);
			_svg.endPolygon();
		}
		else {
			myFbo.begin();
			ofSetColor(0);
			ofRect(0, 0, myFbo.getWidth(), myFbo.getHeight());
			ofSetColor(255);
			ofTranslate(myFbo.getWidth() / 2, myFbo.getHeight() / 2);
			drawShape(myFbo.getWidth(), myFbo.getHeight(), multip);
			myFbo.end();

			ofPixels p;
			p.allocate(myFbo.getWidth(), myFbo.getHeight(), ofImageType::OF_IMAGE_COLOR);
			myFbo.readToPixels(p);
			colorImg.setFromPixels(p);

			grayImage = colorImg;

			grayImage.threshold(80);

			contourFinder.findContours(grayImage, 1, (myFbo.getWidth() * myFbo.getHeight()), 200, true);	// find holes

			for (auto b : contourFinder.blobs) {
				ofPolyline tempP;
				tempP.addVertices(b.pts);
				_svg.beginPolygon();
				for (auto v : tempP.getVertices())
					_svg.vertex(v.x, v.y);
				_svg.endPolygon();
			}
		}
		string path = ofGetTimestampString("%m-%d");
		ofDirectory dir(path);
		if (!dir.exists()) {
			dir.create(true);
		}
		_svg.saveToFile(path + "/snowflake-" + ofGetTimestampString("%H.%M.%S") + ".svg");
		savedF = true;
		fadetime = ofGetElapsedTimeMillis();
	}
}

void ofApp::addVertex(float x, float y, float angle)
{    // Angle is in radians

	shapeL.addVertex(x*cos(angle) + y*sin(angle), y*cos(angle) - x*sin(angle));
}

void ofApp::UISliderMoved(const int & id)
{
	sides = sideSlider.getValue();
}

void ofApp::DropDown(const pair<string, int> & item)
{
	windMenu.setButtonTitle(item.first);
	if (item.first == "Non-Zero Winding") {
		ofSetPolyMode(ofPolyWindingMode::OF_POLY_WINDING_NONZERO);
		fR = SVG_FILL_RULE::NONZERO;
	}
	if (item.first == "Even-Odd Winding") {
		ofSetPolyMode(ofPolyWindingMode::OF_POLY_WINDING_ODD);
		fR = SVG_FILL_RULE::EVEN_ODD;
	}
}

float ofApp::angleOfLine(ofPoint P1, ofPoint P2) {
	float result;

	result = ofRadToDeg(atan2(P2.y - P1.y, P2.x - P1.x));

	if (result < 0)
		result += 360;
	return result;
}
