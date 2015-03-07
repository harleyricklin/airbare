//
//  Button.cpp
//  airBare_v5
//
//  Created by Roopa Vasudevan on 3/4/15.
//
//

#include "Button.h"

Button::Button(int _type) {
    type = _type;
    
    if (type == 0) { //play game button for intro screen
        d = ofGetHeight()/15;
        x = ofGetWidth()/2;
        y = ofGetHeight()- d;
        
    } else if (type == 1) { //show more photos for intro screen
        d = ofGetHeight()/15;
        x = ofGetWidth() + d/3;
        y = ofGetHeight()/2;
        
    } else if (type == 2) {
        d = ofGetHeight()/25;
        x = ofGetWidth()/2 + 2*d;
        y = ofGetHeight() - 2*d;
        
    } else { //question mark for about screen
        d = ofGetHeight()/25;
        x = ofGetWidth()/2 - 2*d;
        y = ofGetHeight() - 2*d;
    }
}

void Button::update() {
    
    if (ofDist(ofGetMouseX(), ofGetMouseY(), x, y) <= d/2) {
        on = true;
        
    } else {
        on = false;
    }
    
    
    if (on == true && prevOn == false) {
        pressed = true;
    } else {
        pressed = false;
    }
    
    prevOn = on;
    
}

void Button::render() {
    
    if (type == 0) { // play game for intro screen
        
        ofFill();
        ofSetColor(92, 50, 111, 225);
        //        ofSetRectMode(OF_RECTMODE_CENTER);
        ofRect(x-(3*d/2), y-d/2, 3*d, d);
        ofNoFill();
        ofSetColor(175, 175, 175);
        ofRect(x-(3*d/2), y-d/2, 3*d, d);
        
    } else if (type == 1) { // show more photos on intro screen
        
        ofFill();
        ofSetColor(0, 111, 100, 200);
        ofCircle(x, y, d);
        ofNoFill();
        ofSetColor(175, 175, 175);
        ofCircle(x, y, d);
        
    } else { //question mark / exit sign for about screen
        
        ofFill();
        if(pressed) {
            ofSetColor(75);
        } else {
        ofSetColor(75, 75, 75, 225);
        }
        ofCircle(x, y, d);
        ofNoFill();
        ofSetColor(175, 175, 175);
        ofCircle(x, y, d);
    }
}