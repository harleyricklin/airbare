//
//  Answer.cpp
//  airBare_v5
//
//  Created by Roopa Vasudevan on 3/4/15.
//
//

#include "Answer.h"

Answer::Answer(string _text, bool _correct) {
    text = _text;
    correct = _correct;
    select = false;
    pressed = false;
    
    
}

void Answer::renderBackground(float _x, float _y){
    
    x = _x;
    y = _y;
    
    ofFill();
    ofSetColor(0, 0, 0);
    ofCircle(x, y, 100);
    

    
}

void Answer::checkAnswer() {
    if (ofDist(ofGetMouseX(), ofGetMouseY(), x, y) <= 100) {
        select = true;
        
    } else {
        select = false;
    }
    
}