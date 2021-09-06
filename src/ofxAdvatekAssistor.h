/*
 * ofxAdvatekAssistor.h
 *
 * OpenFrameworks addon to get data from and to Advatek Pixel Controllers.
 *
 * Contributors: Bruno Herfst, Adam Templeton
 */

#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

class ofxAdvatekAssistor {
    
public:
    
    ofxAdvatekAssistor(){};
    ~ofxAdvatekAssistor(){};
    
    void setup();
    void poll();

private:

    ofxUDPManager udpConnection;
    
};
