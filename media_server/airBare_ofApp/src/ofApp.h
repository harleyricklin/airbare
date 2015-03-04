#pragma once

#include "ofMain.h"
#include "ofxJSON.h"
#include "ofxTextSuite.h"
#include "particle.h"
#include "DataThread.h"
#include "InfoBubble.h"
#include <fstream>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void checkEdges();
        void exit();
    
    
    float temperature;
    float humidity;
    float dust;
    float co2;
    
    int sensors;
    int sensorNum;
    int displayedNum;
    float displayedTemp;
    
    int state;
    int prevState;
    int timer;
    int beginTimer;
    int co2Count;
    int dustCount;
    int attractorText;
    
    int textAlpha;
    int imageAlpha;
    int a;
    
    int explainCount;
    int clickCount;
    
    int factType;
    
    string score;
    
    vector<particle> particles;
    vector<string> introText;
    vector<string> introScreen;
    vector<string> explainScreen;
    string introscreen;
    
    vector<string> factsTips[5];
    InfoBubble info;
    
    DataThread thread;
    
    ofVideoGrabber video;
    ofImage introImage;
    
    ofTrueTypeFont myFont;
    ofTrueTypeFont callFont;
    
    ofxTextBlock text;
    ofxTextBlock scoreText;
    ofxTextBlock moreInfo;
    TextBlockAlignment alignment;
    
		
};
