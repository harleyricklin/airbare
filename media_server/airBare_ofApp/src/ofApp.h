#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxJSON.h"
#include "ofxTextSuite.h"
#include "particle.h"
#include "DataThread.h"
#include "InfoBubble.h"
#include "Button.h"
#include "Answer.h"
#include <fstream>

class ofApp : public ofBaseApp{
    // OSC variables for ambient display
    #define HOST "aribareled"
        
    #define PORT 1234

    // number for max end of inhaler usage scale
    #define MAXINHALER 20
    
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
        void loadQuiz(string filename, int index);
        void displayInfoWindow();
        void loadText();
        void calculateInhalerData();
        void header();
        void exit();
    
    
    float temperature;
    float humidity;
    float dust;
    float co;
    
    int state;
    int prevState;
    int timer;
    int beginTimer;
    int coCount;
    int dustCount;
    int attractorText;
    int factSelect;
    int textAlpha;
    int introp2Alpha;
    int imageAlpha;
    int a;
    
    int imageCount;
    int clickCount;
    int answerCount;
    int quizNumber;
    int correctTracker;
    
    int idleTimer;
    int beginIdle;
    
    int factType;
    int totalScore;
    int highScore;
    
    int previous;
    float textWidth;
    float totalHeight;
    
    float usage;
    int currentRed;
    
    bool intro;
    bool infoWindow;
    bool quizTime;
    bool next;
    bool beginning;
    
    string score;
    string api;
    
    vector<particle> particles;
    vector<ofImage> introImages;
    vector<string> introText;
    vector<string> introScreen;
    vector<string> explainScreen;
    vector<string> infoText;
    string introscreen;
    
    vector<string> facts[3];
    vector<string> quiz[3];
    vector<Answer> answers[3];
    InfoBubble info = InfoBubble(ofGetWidth()/3);
    vector<Button> buttons;
    vector<ofxTextBlock> captions;
    
    DataThread thread;
    
    ofVideoGrabber video;
    ofImage intro0;
    ofImage intro1;
    ofImage introOverlay;
    
    ofTrueTypeFont myFont;
    ofTrueTypeFont callFont;
    ofTrueTypeFont scoreText;
    ofTrueTypeFont headerText;
    ofTrueTypeFont inhalerText;
    
    ofxTextBlock text;
    ofxTextBlock introP2;
    ofxTextBlock moreInfo;
    ofxTextBlock answer1;
    ofxTextBlock answer2;
    ofxTextBlock answer3;
    ofxTextBlock infoWindowText1;
    ofxTextBlock infoWindowText2;
    ofxTextBlock infoWindowText3;
    TextBlockAlignment alignment;
    
    // define the OSC sender
    ofxOscSender sender;
		
};
