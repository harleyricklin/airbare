//
//  Button.h
//  airBare_v5
//
//  Created by Roopa Vasudevan on 3/4/15.
//
//

#ifndef __airBare_v5__Button__
#define __airBare_v5__Button__

#include <stdio.h>

#include "ofMain.h"
#include "ofxTextSuite.h"

class Button {
public:
    void update();
    void render();
    
    bool on;
    bool prevOn;
    bool pressed;
    
    int type;
    
    int buttonTimer;
    int beginPress;
    
    float x, y, d;
    
    string text;
    
    Button(int _type);
};

#endif /* defined(__airBare_v5__Button__) */
