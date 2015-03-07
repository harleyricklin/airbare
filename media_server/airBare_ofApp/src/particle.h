//
//  particle.h
//  airBare_test
//
//  Created by Roopa Vasudevan on 12/22/14.
//
//

#ifndef __airBare_test__particle__
#define __airBare_test__particle__

#include <stdio.h>

#include "ofMain.h"

class particle {
    
public:
    
    //methods
    void render();
    void update();
    void checkPosition(int currentX, int currentY);
//    bool pop();
    
    //variables
    float x;
    float y;
    float targetX;
    float targetY;
    
    float d;
    
    float a;
    
    float temp;
    
    int type;
    
    bool on;
    bool pop;
    bool prevPop;
    bool expand;
    
    int dustRed;
    
    ofColor coColor;
    ofColor dustColor;
    ofColor popped;
    
    //constructor
    particle(float _x, float _y, float _targetX, float _targetY, int _type);
    
private:
    
    
};

#endif /* defined(__airBare_test__particle__) */

