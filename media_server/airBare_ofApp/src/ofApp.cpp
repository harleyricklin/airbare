#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(24);
    ofEnableSmoothing();
    
    //establish video stream
    video.setVerbose(true);
    video.setDeviceID(1);
    video.initGrabber(1080, 1920);
    
    introImage.loadImage("intro.jpg");
    
    //read in text for intro
    ifstream fin;
    fin.open(ofToDataPath("introtext.txt").c_str());
    
    while(fin != NULL) {
        string str;
        getline(fin, str);
        introText.push_back(str);
//        cout << str << endl;
    }
    
    fin.close();
    
    ifstream f;
    f.open(ofToDataPath("introscreen.txt").c_str());
    
    while(f != NULL) {
        string s;
        getline(f, s);
//        introScreen.push_back(s);
        introscreen = introscreen + s + "\n\n";
//        cout << s << endl;
    }
    
    f.close();
    
    f.open(ofToDataPath("explaintext.txt").c_str());
    
    while(f != NULL) {
        string s;
        getline(f, s);
        explainScreen.push_back(s);
//        cout << s << endl;

    }
    
    f.close();
    
    for (int i = 0; i < 5; i++) {
        ifstream g;
        std::stringstream index;
        index << i << ".txt";
        g.open(ofToDataPath(index.str()).c_str());
        
        while(g != NULL) {
            string s;
            getline(g, s);
            factsTips[i].push_back(s);
//            cout << s << endl;
//            cout << factsTips[i].size() << endl;
        }
        
        g.close();
    }

    myFont.loadFont("Apercu Medium.otf", 30);
    callFont.loadFont("Apercu Medium.otf", 15);
    
    text.init("Apercu Medium.otf", 35);
    scoreText.init("Apercu Medium.otf", 15);
    moreInfo.init("Apercu Medium.otf", 20);
    alignment = OF_TEXT_ALIGN_CENTER;
    
    thread.start();
    
    state = 0;
    prevState = 0;
    timer = 0;
    co2Count = 0;
    dustCount = 0;
    textAlpha = 0;
    imageAlpha = 0;
    explainCount = 0;
    
    attractorText = ofRandom(6);
    text.setText(introText[attractorText]);
    text.wrapTextX(300);
    
    moreInfo.setText("Continue popping particles to learn more.");
    moreInfo.wrapTextX(450);
    
    cout << introscreen << endl;
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    video.update();
    
//    if (ofGetElapsedTimeMillis()/1000 % 10 == 0){
    thread.lock();
    
    co2 = thread.co2Value;
    dust = thread.dustValue;
    
    float totalco2 = ofMap(thread.co2Value, 0, 2, 0, 80);
    float totaldust = ofMap(thread.dustValue, 0, 218146, 0, 500);
    float totalNum = totalco2 + totaldust;
    
    float co2particles = 0;
    float dustparticles = 0;
    
    //aligning # of particles with data
    if (particles.size() > 0) {
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i].type == 0) {
                co2particles++;
            } else {
                dustparticles++;
            }
        }
        
        if (co2particles < totalco2) {
            int diff = totalco2 - co2particles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 0));
        } else if (co2particles > totalco2){
            int diff = co2particles - totalco2;
        }
        
        if (dustparticles < totaldust){
            int diff = totaldust - dustparticles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 1));
        }
        
    } else {
        
        for (int i = 0; i < totalco2; i++) {
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 0));
        }
        
        for (int i = 0; i < totaldust; i++){
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 1));
        }

    }
    
    //particles
    for (int i = 0; i < particles.size(); i++) {
        
        particles[i].update();
        checkEdges();
        particles[i].checkPosition(mouseX, mouseY);
        
        if (state == 8) {
            if (prevState == 7) {
                
                if (particles[i].d <= 0) {
                    particles[i].expand = TRUE;
                }
            }
            
            if (particles[i].pop == TRUE) {
                if (particles[i].type == 1) {
                    info.fill = particles[i].dustColor;
                } else {
                    info.fill = particles[i].co2Color;
                }
                info.expand = TRUE;
                
            } else if (info.d <= 0 && particles[i].d <= 0) {
                particles[i].expand = TRUE;
            }
            
        } else if (state == 6){
            if (prevState == 5) {
                
                if (co2Count >= dustCount) {
                    info.fill = particles[i].co2Color;
                } else {
                    info.fill = particles[i].dustColor;
                }
            }
            
        //gameplay
        } else if (state == 5) {
            
            if (particles[i].pop == true && particles[i].prevPop == FALSE) {
                if (particles[i].type == 0){
                    co2Count++;
                    cout << co2Count << " CO2 particles popped" << endl;
                } else {
                    dustCount++;
                    cout << dustCount << " particulate particles popped" << endl;
                }
            }
            
        //attractor mode
        }   else if (state == 0) {
            info.fill = particles[i].co2Color;
        }
  
        particles[i].prevPop = particles[i].pop;
        
    }
    
    
    //info bubbles
    if (state == 8) {
        if (prevState == 7) {
            cout << "PREV STATE 7" << endl;
            co2Count = 0;
            dustCount = 0;
//            beginTimer = ofGetElapsedTimeMillis();
        }
        
    } else if (state == 7) {
        if (prevState == 6) {
            
            cout << "PREV STATE 6" << endl;
            float factSelect;
            float prob = ofRandom(10);
            
            if (co2Count >= dustCount) {
                
                factSelect = ofRandom(2);
                text.setText(factsTips[1][factSelect]);
                text.wrapTextArea(300, 300);
                
            } else {
                factSelect = ofRandom(4);
                text.setText(factsTips[2][factSelect]);
                text.wrapTextArea(300, 300);
            }
            
        }
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
    } else if (state == 6){
        if (prevState == 5) {
            cout << "PREV STATE 5" << endl;
            std::stringstream convert;
            convert << "Nice job, you popped " << co2Count + dustCount << " particles!" << endl;
            
            text.setText(convert.str());
            text.wrapTextX(300);
            
            std::stringstream co2Score;
            std::stringstream dustScore;
            
            co2Score << "C02 particles popped: " << co2Count << endl;
            dustScore << "Particulate particles popped: " << dustCount << endl;
            
            score = co2Score.str() + dustScore.str() + "\n\nPop to continue.";
            
            scoreText.setText(score);
            scoreText.wrapTextX(300);
            
            info.expand = TRUE;
        }
        
        //gameplay
    } else if (state == 5) {
        if (prevState == 4) {
            cout << "PREV STATE 4" << endl;
            beginTimer = ofGetElapsedTimeMillis();
//            cout << "beginTimer: " << beginTimer << endl;
        }
        
        if (timer >= 10) {
            
            timer = 0;
        } else {
            timer = (ofGetElapsedTimeMillis() - beginTimer)/1000;
            //                cout << "Timer: " << timer << endl;
        }
    } else if (state == 4) {
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (prevState == 3) {
            cout << "PREV STATE 3" << endl;
            textAlpha = 0;
            text.setText("Pop as many particles as you can in 10 seconds.");
            text.wrapTextX(300);
        }
    } else if (state == 3) {
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (prevState == 2) {
            cout << "PREV STATE 2" << endl;
            textAlpha = 0;
            text.setText(explainScreen[explainCount]);
            text.wrapTextArea(300, 300);
        }
        
        //intro screen
    } else if (state == 1 && prevState == 0) {
            cout << "PREV STATE 0" << endl;
            textAlpha = 0;
            
            text.setText(introscreen);
            text.wrapTextArea(500, 300);
        
        //attractor mode
    }   else if (state == 0) {

        timer = (ofGetElapsedTimeMillis() - beginTimer)/1000;
        
        if (timer >= 5) {
            if (textAlpha <= -50) {
                timer = 0;
                attractorText = ofRandom(introText.size()-1);
                text.setText(introText[attractorText]);
                text.wrapTextX(300);
                text.wrapTextForceLines(3);
//                cout << attractorText << endl;
                beginTimer = ofGetElapsedTimeMillis();
            }
        } else if (timer >= 0 && info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (info.contract == TRUE && info.d <= 10 && timer >= 1) {
            state = 1;
//            cout << state << endl;
        }
        
    }
    
    info.update();
    info.checkPosition();
    
    thread.unlock();
    
    prevState = state;
//    cout << "Prev State: " << prevState << " State: " << state << endl;


}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(50);
    
//    video.draw(0, 0);
    // rotate camera video 90 degrees for portrait mode
    // hard coded dimensions for 1080 video
    glPushMatrix();
        ofTranslate(1080,1920);
        ofRotate(90);
        vidGrabber.draw(-1080,850);
    glPopMatrix();
    
    ofSetColor(0, 175);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    for (int i = 0; i < particles.size(); i++) {
        particles[i].render();
    }
    
    info.render();
    
    std::stringstream co2level;
    std::stringstream dustlevel;
    
    co2level << "Current CO2 Level: " << co2 << endl;
    dustlevel << "Current Particulate Level: " << dust << endl;
    
    ofSetColor(255);
    ofDrawBitmapString(co2level.str(), 10, 20);
    ofDrawBitmapString(dustlevel.str(), 10, 35);
    
    if (state == 8) {
        
        moreInfo.setColor (255, 255, 255, a);
        moreInfo.drawCenter(ofGetWidth()/2, 100);
        
        string reset = "Touch the bottom of the screen to start over.";
        ofSetColor(255, a);
        callFont.drawString(reset, ofGetWidth()/2-callFont.stringWidth(reset)/2, ofGetHeight()-25);
        
        if (info.d > 100) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
        } else {
            textAlpha = -50;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-140);
        
        if (a < 255) {
            a+=10;
        } else {
            a = 255;
        }
        
    } else if (state == 7) {
        
        if (info.d >= 100) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
            
            int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
            text.setColor(255, 255, 255, mappedAlpha);
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-130);
        }
        
    } else if (state == 6) {
        std::stringstream convert;
        convert << co2Count + dustCount;
        
        if (info.d >= 100) {
            
            if (textAlpha < 255) {
                textAlpha += 10;
            }
        
            int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);
            text.setColor(255, 255, 255, mappedAlpha);
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - 120);
            
        
            scoreText.setColor(255, 255, 255, 255);
            scoreText.drawCenter(ofGetWidth()/2, ofGetHeight()/2+70);
        }
        
    } else if (state == 5) {
        std::stringstream convert;
        convert << 10-timer;
        
        if (timer >= 10) {
            state = 6;
            cout << "STATE 6" << endl;
        }
        
        string s = "Time left: " + convert.str();
        ofSetColor(255);
        myFont.drawString(s, ofGetWidth()/2 - myFont.stringWidth(s)/2, 100);
        
    } else if (state == 4 && info.d >= 100) {
        
        if (textAlpha < 255) {
            textAlpha += 10;
        }

        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-120);
        
        string call = "Pop to start!";
        ofSetColor(255);
        callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+150);
        
    } else if (state == 3) {
        
        if (textAlpha < 255) {
            textAlpha += 10;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-140);
        
        string call = "Pop to continue.";
        ofSetColor(255);
        callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+150);
    
    } else if (state == 2) {
        
        if (imageAlpha >= 0 && textAlpha >= -50) {
            textAlpha -= 10;
        } else if (imageAlpha >= 0 && textAlpha <= -50){
            imageAlpha -= 10;
        } else if (imageAlpha <= 0) {
            state = 3;
//            cout << "state: " << state << endl;
        }
        
        ofSetColor(100, imageAlpha);
        introImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, -60, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2+100);
        
//        cout << "textAlpha: " << textAlpha << endl;
        
    } else if (state == 1) {
        //draw image
        if (imageAlpha <= 255){
            imageAlpha += 10;
        } else if (imageAlpha >= 255) {
            if (textAlpha <= 255) {
            textAlpha += 10;
            }
            
        }
        
        ofSetColor(100, imageAlpha);
        introImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2+100);
        
    } else if (state == 0) {
        
        if(info.d >= 100 && info.contract == FALSE){
//            cout << "text alpha: " << textAlpha << endl;
            if (timer >= 5){
//                cout << "START FADING OUT" << endl;
                if (textAlpha >= -50) {
                    textAlpha -= 10;
                }
            } else if (timer >= 0 && timer < 5) {
                if (textAlpha <= 255) {
                    textAlpha += 10;
                }
            }
            
            int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
            text.setColor(255, 255, 255, mappedAlpha);
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-100);
            string call = "Pop to begin!";
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+150);
        }

    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    
    
    for (int i = 0; i < particles.size(); i++) {
        
        if (particles[i].on == true && info.d <= 0) {
            
            if (state == 8) {
                clickCount++;
            }
//            cout << i << " was clicked, temp value " << particles[i].temp << endl;
            displayedNum = i;
            displayedTemp = particles[i].temp;
            particles[i].pop = TRUE;
            
            if (particles[i].type == 0) {
                factType = 0;
            } else {
                factType = 1;
            }
        }
    }
    
    cout << factType << endl;
    
    if (info.on == TRUE && info.expand == FALSE) {
        if (state == 8) {
            info.contract = TRUE;
            textAlpha = -50;
        } else if (state == 7) {
            info.contract = TRUE;
            state = 8;
        } else if (state == 6) {
            info.contract = TRUE;
            textAlpha = 0;
            state = 7;
        } else if (state == 4) {
            info.contract = TRUE;
            state = 5;
        } else if (state == 3) {
            if (explainCount >= 3) {
                info.contract = TRUE;
                state = 4;
            }
        } else if (state == 0) {
            info.contract = TRUE;
        }
    }
    
    if (state == 8) {
//        textAlpha = -50;
        
        if (ofGetMouseY() >= ofGetHeight() - 100){
            state = 0;
            clickCount = 0;
            beginTimer = ofGetElapsedTimeMillis();
            textAlpha = -50;
            attractorText = ofRandom(introText.size()-1);
            text.setText(introText[attractorText]);
            text.wrapTextX(300);
        } else {
            
            if (info.d <= 0) {
                
                cout << clickCount << endl;
                
                if (clickCount % 5 == 0) {
                    int factSelect = ofRandom(factsTips[4].size()-1);
                    text.setText(factsTips[4][factSelect]);
                    text.wrapTextArea(300, 300);
                } else {
                    
                    if (factType == 0) {
                        int factSelect = ofRandom(factsTips[1].size()-1);
                        text.setText(factsTips[1][factSelect]);
                    } else {
                        int factSelect = ofRandom(factsTips[2].size()-1);
                        text.setText(factsTips[2][factSelect]);
                    }
                    
                    text.wrapTextArea(300, 300);
                }
            }
        }
        
    } else if (state == 3) {
        if (explainCount < 3) {
            explainCount++;
            textAlpha = 0;
            text.setText(explainScreen[explainCount]);
            text.wrapTextArea(300, 300);
            cout << explainCount << endl;
        }
    }
    
    if (state == 1) {
        state++;
    } else if (state == 0) {
        if (prevState == 0) {
        state++;
        }
    }
    
    cout << state << endl;

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::checkEdges(){
    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < particles.size(); j++) {
            
            if (i != j){
                
                if (ofDist(particles[i].x, particles[i].y, particles[j].x, particles[j].y) <= particles[i].d){
//                    cout << "COLLIDING" << endl;
                    if (particles[i].x > particles[j].x) {
                        particles[i].targetX = ofRandom(particles[i].x, ofGetWidth());
                    } else {
                        particles[i].targetX = ofRandom(particles[i].x, 0);
                    }
                    
                    if (particles[i].y > particles[j].y) {
                        particles[i].targetY = ofRandom(particles[i].y, ofGetHeight());
                    } else {
                        particles[i].targetY = ofRandom(particles[i].y, 0);
                    }
                    
                }
            }
            
        }

    }
}


void ofApp::exit() {
    
    thread.stop();
}
