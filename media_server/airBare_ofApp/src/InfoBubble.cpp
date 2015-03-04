//
//  InfoBubble.cpp
//  airBare_test
//
//  Created by Roopa Vasudevan on 1/25/15.
//
//

#include "InfoBubble.h"

InfoBubble::InfoBubble() {
    
    d = 0;
    targetD = 200;
    x = ofGetWidth()/2;
    y = ofGetHeight()/2;
    
    
}

void InfoBubble::update() {

    
    if (d < targetD && expand == TRUE) {
        
        d = ofLerp(d, targetD, 0.5);
//        cout << "expand is ON" << endl;
        
    } else if (d == targetD && expand == TRUE){
        
        expand = FALSE;
//        cout << "expand is OFF" << endl;

    }
    
    if (d >= 10 && contract == TRUE) {
        d = ofLerp(d, 0, 0.5);

    } else if (d <= 10 && contract == TRUE) {
        d = 0;
        contract = FALSE;
    }
    
    
}

void InfoBubble::render() {
    
    ofSetColor(fill);
    ofFill();
    ofSetCircleResolution(80);
    ofCircle(x, y, d);
    
    
}

void InfoBubble::checkPosition() {
    
    if (ofDist(ofGetMouseX(), ofGetMouseY(), x, y) <= d/2) {
        on = TRUE;
    } else {
        on = FALSE;
    }
}