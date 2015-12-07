#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofBackground(70, 70, 200);
	ofEnableSmoothing();

	ofSetPolyMode(ofPolyWindingMode::OF_POLY_WINDING_NONZERO);
	fR = SVG_FILL_RULE::NONZERO;

	dragging = false;
	savedF = false;

	xValues.clear();
	xValues.push_back(0);
	xValues.push_back(0);
	yValues.clear();
	yValues.push_back(0);
	yValues.push_back(0);

	symColor = 0;
	segmentColor = 255;
	CircleDia = 10;
	circleColor.set(255, 128, 0);

	mtCircleDia = 10;
	emptyCircle.set(255, 128, 0, 0);

	SideLength = ofGetWidth() * 9 / 20;

	clearShape();

	circleStroke = 255;
	ofSetLineWidth(1);

	f.load(OF_TTF_SANS, 14);

	saveBut.setPosition(575, 600);
	saveBut.setFont(f);
	saveBut.setTitle("Save");
	saveBut.setButtonStyle(UI_BUTTON_ROUNDED_RECT);
	saveBut.setWidth(40);
	saveBut.setID(1);
	saveBut.setHeight(25);

	resetBut.setPosition(500, 600);
	resetBut.setFont(f);
	resetBut.setTitle("Clear");
	resetBut.setButtonStyle(UI_BUTTON_ROUNDED_RECT);
	resetBut.setWidth(40);
	resetBut.setHeight(25);
	resetBut.setID(2);

	sideSlider.setup(6, 3, 20, 30, 600);

	windMenu.addMenuItem("Non-Zero Winding");
	windMenu.addMenuItem("Even-Odd Winding");
	windMenu.setButtonFont(f);
	windMenu.setPosition(455, 30);
	windMenu.setButtonColor(ofColor::beige);
	windMenu.setButtonHoverColor(ofColor::sienna);
	windMenu.setAutoSizing(true);
	windMenu.setHeight(28);

	windMenu.setButtonTitle("Non-Zero Winding");

	colorImg.allocate(640, 640);
	grayImage.allocate(640, 640);
	myFbo.allocate(640, 640, GL_RGB);

	ofAddListener(resetBut.buttonEvent, this, &ofApp::UIButPressed);
	ofAddListener(saveBut.buttonEvent, this, &ofApp::UIButPressed);
	ofAddListener(sideSlider.sliderEvent, this, &ofApp::UISliderMoved);
	ofAddListener(windMenu.menuEvent, this, &ofApp::DropDown);

	threshold = 1.2;
	bSimplify = false;
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
		myFbo.begin();
		ofSetColor(0);
		ofRect(0, 0, 640, 640);
		ofSetColor(255);
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
		drawShape();
		myFbo.end();

		ofPixels p;
		p.allocate(640, 640, ofImageType::OF_IMAGE_COLOR);
		myFbo.readToPixels(p);
		colorImg.setFromPixels(p);

		grayImage = colorImg;

		grayImage.threshold(80);


		contourFinder.findContours(grayImage, 1, (640 * 640), 200, true);	// find holes

		myFbo.begin();
		ofSetColor(0);
		ofRect(0, 0, 640, 640);
		ofSetColor(255);
		for (auto b : contourFinder.blobs) {
			ofPolyline tempP;
			tempP.addVertices(b.pts);
			ofSetLineWidth(2);
			tempP.draw();
		}
		myFbo.end();

		myFbo.readToPixels(p);
		colorImg.setFromPixels(p);

		grayImage = colorImg;

		grayImage.threshold(80);

		houghLines.findHoughLines(grayImage);

	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	int i, xt, yt;

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	ofSetLineWidth(1);
	drawShape();
	ofPopMatrix();



	symColor.set(255, 255, 255, 176);
	segmentColor = symColor;

	ofSetLineWidth(1);

	i = 0;
	while (i < xValues.size())
	{
		{

			ofPushMatrix();
			ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
			ofSetColor(circleColor.r, circleColor.g, circleColor.b, 255);
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

	ofPushStyle();
	ofSetColor(ofColor::red);
	ofPushMatrix();
	if (fR == SVG_FILL_RULE::EVEN_ODD) {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
		shapeL.draw();
	}
	else {
		for (auto b : contourFinder.blobs) {
			ofSetColor(ofColor::red);
			ofPolyline tempP;
			tempP.addVertices(b.pts);
			tempP.draw();
			/*ofSetColor(ofColor::aqua);
			for (auto v : tempP.getVertices()) {
				ofCircle(v, 2);
			}*/
		}
		//houghLines.drawHoughLines(colorImg);
		//colorImg.draw(0, 0);
	}
	ofPopMatrix();

	if (savedF) {
		ofSetColor(255, 100, 100, 255 - (ofGetElapsedTimeMillis() - fadetime) / 18);
		f.drawString("Saved SVG", 520, 590);
		if ((ofGetElapsedTimeMillis() - fadetime) / 1000.0 > 5)
			savedF = false;
	}


	saveBut.draw();
	resetBut.draw();
	windMenu.draw();

	ofSetColor(symColor);
	ofDrawBitmapString("Sides: " + ofToString(sides), 30, 590);
	sideSlider.draw();
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case '1':
		threshold += .1;
		cout << threshold << endl;
		break;
	case '2':
		threshold -= .1;
		cout << threshold << endl;
		break;
	default:
		break;
	}
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
		if (MovePoint == 0)
		{
			xt = ofGetWidth() / 2;

			if (y < ofGetHeight() / 2)
			{
				yValues[MovePoint] = y - ofGetHeight() / 2;
				yt = y;
			}
			else
			{
				yValues[MovePoint] = 0;
				yt = ofGetHeight() / 2;
			}


		}
		else if (MovePoint == xValues.size() - 1)    // Last point in array.
		{

			xf = mouseX - ofGetWidth() / 2;
			yf = mouseY - ofGetHeight() / 2;

			xf = -yf / sqrt(3);

			if (yf > 0)
			{
				xf = 0;
				yf = 0;
			}

			xValues[MovePoint] = (int)xf;
			yValues[MovePoint] = (int)yf;

			xt = (int)(ofGetWidth() / 2 + xf);
			yt = (int)(ofGetHeight() / 2 + yf);

		}
		else
		{
			xValues[MovePoint] = mouseX - ofGetWidth() / 2;
			yValues[MovePoint] = mouseY - ofGetHeight() / 2;
			xt = mouseX;
			yt = mouseY;

		}
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
	xValues[1] = SideLength*sqrt(3) / 4;

	yValues[0] = -SideLength;
	yValues[1] = -0.75*SideLength;
}



void ofApp::drawShape()
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
				drawVertex(-1 * xValues[j], yValues[j], angle);
				j++;
			}

			std::reverse(xValues.begin(), xValues.end());
			std::reverse(yValues.begin(), yValues.end());

			j = 0;

			while (j < xValues.size())
			{
				drawVertex(xValues[j], yValues[j], angle);
				j++;
			}
			i++;
		}
		else {
			j = 0;
			while (j < xValues.size())
			{
				drawVertex(xValues[j], yValues[j], angle);
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

		ofxSVG _svg;
		_svg.noStroke();
		_svg.pushGroup();
		_svg.fillRule(fR);
		_svg.fill("#4646C8");
		_svg.rect(0, 0, ofGetWidth(), ofGetHeight());
		_svg.stroke("#B2ACB7", 1);
		_svg.fill("#BACDE8");
		if (fR == SVG_FILL_RULE::EVEN_ODD) {
			_svg.beginPolygon();
			float hx = ofGetWidth() / 2.0;
			float hy = ofGetHeight() / 2.0;
			for (auto v : shapeL.getVertices())
				_svg.vertex(v.x + hx, v.y + hy);
			_svg.endPolygon();
		}
		else {
			//for (auto b : contourFinder.blobs) {
			//	ofPolyline tempP;
			//	tempP.addVertices(b.pts);
			//	if (bSimplify)
			//		tempP.simplify(threshold);
			//	if (b.hole)
			//		_svg.fill("#4646C8");
			//	else
			//		_svg.fill("#BACDE8");
			//	_svg.beginPolyline();
			//	ofPoint p0, p1, p2, p3; // we need to first point and then to be able to compare two points
			//	bool first, fd;
			//	float ang;
			//	int count = 0;
			//	first = fd = true;
			//	vector<float> angles;
			//	for (auto v : tempP.getVertices()) {
			//		count++;
			//		if (first) {
			//			p2 = p1 = p0 = v;
			//			first = false;
			//			_svg.vertex(v.x, v.y);
			//		}
			//		else {
			//			p3 = v;
			//			if (fd) {
			//				ang = angleOfLine(p1, p3);
			//				fd = false;
			//			}
			//			else {
			//				/*if (ang != angleOfLine(p1, p3))
			//					pnts.push_back(p2);*/
			//				if (ang != angleOfLine(p1, p3)) { //the angle has moved lets get the new angle
			//					_svg.vertex(p2.x, p2.y); //this was the previous point
			//					p1 = p2; //set the new corner point
			//					ang = angleOfLine(p1, p3); //whats the new angle
			//					p2 = p3;
			//				}
			//				else { //the angle is the same move on to the next point
			//					p2 = p3;
			//				}
			//			}
			//		}
			//	}
			//	_svg.vertex(p0.x, p0.y);
			//	_svg.endPolyline();
			//}
			for (auto b : contourFinder.blobs) {
				ofPolyline tempP;
				tempP.addVertices(b.pts);
				/*if (bSimplify)
					tempP.simplify(threshold);*/
				if (b.hole)
					_svg.fill("#4646C8");
				else
					_svg.fill("#BACDE8");
				_svg.beginPolyline();
				for (auto v : tempP.getVertices())
					_svg.vertex(v.x, v.y);
				_svg.endPolyline();
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
