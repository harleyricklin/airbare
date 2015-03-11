//
//  Answer.h
//  airBare_v5
//
//  Created by Roopa Vasudevan on 3/4/15.
//
//

#ifndef __airBare_v5__Answer__
#define __airBare_v5__Answer__

#include <stdio.h>
#include "ofMain.h"

class Answer {
public:
    
    string text;
    bool correct;
    bool select;
    bool pressed;
    bool reveal;
    
    float x;
    float y;
    float w;
    float h;
    
    ofColor fill;
    
    Answer(string _text, bool _correct);
    
    void renderBackground(float _x, float _y);
    void checkAnswer();
};

#endif /* defined(__airBare_v5__Answer__) */
