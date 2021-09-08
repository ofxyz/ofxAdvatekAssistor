#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    AdvatekAssist.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    AdvatekAssist.update();
	devices = AdvatekAssist.getDevices();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDrawBitmapString("Press Any Key to Poll...", 50, 50);
	ofDrawBitmapString(to_string(devices.size()) + " devices connected", 50, 80);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    AdvatekAssist.poll();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
