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
        playgame.loadImage("playgame.png");
        
    } else if (type == 1) { //show more photos for intro screen
        d = ofGetHeight()/15;
        x = ofGetWidth() + d/4;
        y = ofGetHeight()/2;
        morephotos.loadImage("morephotos.png");
        
    } else if (type == 2) { //question mark for about screen
        d = ofGetHeight()/25;
        x = ofGetWidth()/2 + 2*d;
        y = ofGetHeight() - 2*d;
        about.loadImage("about.png");
        
    } else { //exit for about screen
        d = ofGetHeight()/25;
        x = ofGetWidth()/2 - 2*d;
        y = ofGetHeight() - 2*d;
        home.loadImage("exit.png");
    }
}

void Button::update() {
    if (type == 0) {
        if (ofGetMouseX() >= x-(3*d/2) && ofGetMouseX() <= (x-(3*d/2)) + 3*d) {
            if (ofGetMouseY() >= y-d/2 && ofGetMouseY() <= (y-d/2) + d) {
                on = true;
                
            } else {
                on = false;
            }
        }
    } else {
        if (ofDist(ofGetMouseX(), ofGetMouseY(), x, y) <= d) {
            on = true;
            
        } else {
            on = false;
        }
    }
    
    prevOn = on;
    
}

void Button::render() {
    
    if (type == 0) { // play game for intro screen
        
        ofFill();
        ofSetColor(92, 50, 111, a);
        //        ofSetRectMode(OF_RECTMODE_CENTER);
        ofRect(x-(3*d/2), y-d/2, 3*d, d);
        ofNoFill();
        ofSetColor(175, 175, 175, a);
        ofRect(x-(3*d/2), y-d/2, 3*d, d);
        playgame.draw(x-(3*d/2), y-d/2);
        
    } else if (type == 1) { // show more photos on intro screen
        
        ofFill();
        ofSetColor(0, 111, 100, a);
        ofCircle(x, y, d);
        ofNoFill();
        ofSetColor(175, 175, 175, a);
        ofCircle(x, y, d);
        morephotos.draw(x-(d - 10), y-d);
        
    } else { //question mark / exit sign for about screen
        
        ofFill();
        ofSetColor(75, 75, 75, a);
        ofCircle(x, y, d);
        ofNoFill();
        ofSetColor(175, 175, 175);
        ofCircle(x, y, d);
        
        if (type == 2) {
            about.draw(x-d+25, y-d+25, 100, 100);
        } else {
            home.draw(x-d+25, y-d+25, 100, 100);
        }
    }
}