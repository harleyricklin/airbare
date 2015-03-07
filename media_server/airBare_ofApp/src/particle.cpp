//
//  particle.cpp
//  airBare_test
//
//  Created by Roopa Vasudevan on 12/22/14.
//
//

#include "particle.h"

particle::particle(float _x, float _y, float _targetX, float _targetY, int _type) {
    
    x = _x;
    y = _y;
    targetX = _targetX;
    targetY = _targetY;
    type = _type;
    d = ofGetWidth()/15;
    dustRed = 159;
    
    on = FALSE;
    pop = FALSE;
    prevPop = FALSE;
    
    coColor.r = 0;
    coColor.g = 200;
    coColor.b = 175;
    
    dustColor.r = dustRed;
    dustColor.g = 11;
    dustColor.b = 221;
    
    
    popped.r = 50;
    popped.g = 50;
    popped.b = 50;
    
}

void particle::update() {
    
    dustColor.r = dustRed;
    
    if (pop) {
        a = 200;
    } else if (type == 0){
        a = 100;
    }
    
    if (x + d/2 >= ofGetWidth() || x - d/2 <= 0 || y + d/2 >= ofGetHeight() || y - d/2 <= 0) {
        targetX = ofRandomWidth();
        targetY = ofRandomHeight();
    }
    
    if (pop == true) {
        x = x;
        y = y;
        
        if (d >= 1) {
            d = d-5;
        } else{
            d = 0;
            pop = FALSE;
        }
    
    } else {
        x = ofLerp(x, targetX, 0.005);
        y = ofLerp(y, targetY, 0.005);
    }
    
    if (expand == TRUE) {
        
        if (d <= 28) {
            d = d+5;
        } else {
            d = ofGetWidth()/15;
            expand = FALSE;
        }
    }
    
    
}

void particle::render() {
    
    ofFill();
    //setting color
    if (type == 0) {
        ofSetColor(coColor, a);
    } else {
        
        ofSetColor(dustColor, a);
    }

    ofEllipse(x, y, d, d);
    
}

void particle::checkPosition(int currentX, int currentY) {
    
    if (currentX >= x - d/2 && currentX <= x + d/2 && currentY >= y-d/2 && currentY <= y + d/2){
            on = true;
        } else {
            on = false;
        }
    

}