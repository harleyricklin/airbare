//
//  InfoBubble.h
//  airBare_test
//
//  Created by Roopa Vasudevan on 1/25/15.
//
//

#ifndef __airBare_test__InfoBubble__
#define __airBare_test__InfoBubble__

#include <stdio.h>
#include "ofMain.h"

class InfoBubble {
    
    public:
    
        float x;
        float y;
    
        float d;
        float targetD;
        
        bool expand;
        bool contract;
        bool on;
    
        ofColor fill;
    
        void update();
        void render();
        void checkPosition();
    
    InfoBubble(float _targetD);
};

#endif /* defined(__airBare_test__InfoBubble__) */
