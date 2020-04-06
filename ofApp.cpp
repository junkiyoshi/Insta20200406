#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(39);
	ofSetLineWidth(2);

	ofTrueTypeFontSettings font_settings("fonts/msgothic.ttc", 20);
	font_settings.antialiased = true;
	font_settings.addRanges(ofAlphabet::Japanese);
	this->font.load(font_settings);

	this->words = {

		u8"零", u8"壱", u8"弐", u8"参", u8"肆",
		u8"伍", u8"陸", u8"漆", u8"捌", u8"玖",
		u8"多",
	};

	this->radius = 20;

	this->box2d.init();
	this->box2d.setGravity(0, 0);
	this->box2d.createBounds();
	this->box2d.setFPS(60);
	this->box2d.registerGrabbing();

	for (int i = 0; i < 80; i++) {

		auto circle = make_shared<ofxBox2dCircle>();
		circle->setPhysics(0.5, 0.63, 0.1);
		circle->setup(this->box2d.getWorld(), ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()), this->radius);
		this->circles.push_back(circle);
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	for (int i = 0; i < this->circles.size(); i++) {

		for (int j = i + 1; j < this->circles.size(); j++) {

			float distance = this->circles[i]->getPosition().distance(this->circles[j]->getPosition());
			if (distance < this->radius * 4) {

				this->circles[i]->addForce(this->circles[i]->getPosition() - this->circles[j]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 1.2, 0.002));
				this->circles[j]->addForce(this->circles[j]->getPosition() - this->circles[i]->getPosition(), ofMap(distance, this->radius, this->radius * 4, 1.2, 0.002));
			}
		}
	}

	this->box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	auto threshold = 80;

	vector<glm::vec2> locations;
	for (int i = 0; i < this->circles.size(); i++) {

		glm::vec2 location = this->circles[i]->getPosition();
		locations.push_back(location);
	}

	for (auto& location : locations) {

		int word_index = 0;
		for (auto& other : locations) {

			if (location == other) { continue; }

			auto distance = glm::distance(location, other);
			if (distance < threshold) {

				if (word_index < words.size() - 1) { word_index++; }

				auto direction_rad = std::atan2(other.y - location.y, other.x - location.x);
				auto direction = direction_rad * RAD_TO_DEG;
				auto width = ofMap(distance, 0, threshold, 360, 0);

				ofNoFill();
				ofBeginShape();
				for (auto deg = direction - width * 0.5; deg <= direction + width * 0.5; deg++) {

					ofVertex(location.x + radius * cos(deg * DEG_TO_RAD), location.y + radius * sin(deg * DEG_TO_RAD));
				}
				ofEndShape();

				ofDrawLine(location + glm::vec2(radius * cos(direction * DEG_TO_RAD), radius * sin(direction * DEG_TO_RAD)),
					other + glm::vec2(radius * cos((180 + direction) * DEG_TO_RAD), radius * sin((180 + direction) * DEG_TO_RAD)));
			}
		}

		this->font.drawString(this->words[word_index], location.x - 12, location.y + 10);
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}