#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(24);
    ofEnableSmoothing();
    //    ofHideCursor();
    
    //establish video stream
    video.setVerbose(true);
    video.setDeviceID(0);
    video.initGrabber(1920, 1080);
    
    introImage.loadImage("intro.jpg");
    
    for (int i = 0; i < 4; i++) {
        buttons.push_back(Button(i));
    }
    
    myFont.loadFont("Apercu Medium.otf", 35);
    callFont.loadFont("Apercu Medium.otf", 20);
    
    text.init("Apercu Medium.otf", 50);
    answer1.init("Apercu Medium.otf", 35);
    answer2.init("Apercu Medium.otf", 35);
    answer3.init("Apercu Medium.otf", 35);
    
    infoWindowText1.init("Apercu Medium.otf", 35);
    infoWindowText2.init("Apercu Medium.otf", 35);
    infoWindowText3.init("Apercu Medium.otf", 35);
    
    moreInfo.init("Apercu Medium.otf", 20);
    alignment = OF_TEXT_ALIGN_CENTER;
    
    loadText();
    
    thread.start();
    
    state = 0;
    prevState = 0;
    timer = 0;
    coCount = 0;
    dustCount = 0;
    textAlpha = 0;
    imageAlpha = 0;
    imageCount = 0;
    answerCount = 0;
    previous = 0;
    factSelect = 0;
    
    idleTimer = 0;
    beginIdle = 0;
    
    intro = false;
    infoWindow = false;
    quizTime = false;
    next = false;
    beginning = true;
    
    float area = (ofGetWidth()/3)*(ofGetWidth()/3);
    //    textWidth = sqrt(area/2);
    textWidth = 500;
    
    attractorText = ofRandom(6);
    text.setText(introText[attractorText]);
    text.wrapTextArea(300, 300);
    
    moreInfo.setText("Continue popping particles to learn more.");
    moreInfo.wrapTextX(450);
    
    
    // setup OSC sender
    sender.setup(HOST, PORT);
    
    calculateInhalerData();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    video.update();
    
    thread.lock();
    
    co = thread.coValue;
    dust = thread.dustValue;
    temperature = thread.tempValue;
    
    float totalco = ofMap(thread.coValue, 0, 2, 20, 100);
    float totaldust = ofMap(thread.dustValue, 0, 218146, 20, 500);
    float totalNum = totalco + totaldust;
    
    calculateInhalerData();
    
    //SEND OUT OSC DATA HERE
    ofxOscMessage m;
    m.setAddress("/temp");
    m.addIntArg((int) temperature);
    sender.sendMessage(m);
    
    float coparticles = 0;
    float dustparticles = 0;
    
    //aligning # of particles with data
    if (particles.size() > 0) {
        for (int i = 0; i < particles.size(); i++) {
            if (particles[i].type == 0) {
                coparticles++;
            } else {
                dustparticles++;
            }
        }
        
        if (coparticles < totalco) {
            int diff = totalco - coparticles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 0));
        } else if (coparticles > totalco){
            int diff = coparticles - totalco;
        }
        
        if (dustparticles < totaldust){
            int diff = totaldust - dustparticles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 1));
        }
        
    } else {
        
        for (int i = 0; i < totalco; i++) {
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
            //            cout << factSelect << endl;
            if (prevState == 7) {
                
                if (particles[i].d <= 0) {
                    particles[i].expand = TRUE;
                }
            }
            
            if (quizTime == false) {
                if (particles[i].pop == TRUE) {
                    if (particles[i].type == 1) {
                        info.fill = particles[i].dustColor;
                    } else {
                        info.fill = particles[i].coColor;
                    }
                    info.expand = TRUE;
                    
                } else if (info.d <= 0 && particles[i].d <= 0) {
                    particles[i].expand = TRUE;
                }
            }
            
            if (factSelect >= 5) {
                quizTime = true;
            }
            
        } else if (state == 6){
            if (prevState == 5) {
                
                if (coCount >= dustCount) {
                    info.fill = particles[i].coColor;
                } else {
                    info.fill = particles[i].dustColor;
                }
            }
            
            //gameplay
        } else if (state == 5) {
            
            if (particles[i].pop == true && particles[i].prevPop == FALSE) {
                if (particles[i].type == 0){
                    coCount++;
                    cout << coCount << " CO particles popped" << endl;
                } else {
                    dustCount++;
                    cout << dustCount << " Particulate particles popped" << endl;
                }
            }
            
            //attractor mode
        }   else if (state == 0) {
            info.fill = particles[i].coColor;
        }
        
        particles[i].prevPop = particles[i].pop;
        
    }
    
    //info bubbles
    if (state == 9) {
        
        if (info.contract == FALSE) {
            text.setText("Well done! Take another quiz to increase your score.");
            text.wrapTextArea(400, 400);
            info.expand = TRUE;
        }
        
        
        
        if (next == true && info.d <= 10) {
            state = 8;
            factSelect = 0;
            quizNumber++;
            next = false;
            quizTime = false;
        } else if (beginning == true && info.d <= 10){
            state = 0;
            beginning = false;
        }
        
        
    } if (state == 8) {
        if (prevState == 7) {
            cout << "PREV STATE 7" << endl;
            coCount = 0;
            dustCount = 0;
            //            beginTimer = ofGetElapsedTimeMillis();
        }
        
        if (quizTime == true) {
            if (info.contract == FALSE) {
                if (answerCount <= 2){
                    if (info.d <= 10 || answerCount != previous){
                        text.setText(quiz[quizNumber][answerCount*4]);
                        text.wrapTextArea(textWidth, textWidth/2);
                        answer1.setText(answers[quizNumber][answerCount*3].text);
                        answer1.wrapTextArea(150, 150);
                        answer2.setText(answers[quizNumber][(answerCount*3)+1].text);
                        answer2.wrapTextArea(150, 150);
                        answer3.setText(answers[quizNumber][(answerCount*3)+2].text);
                        answer3.wrapTextArea(150, 150);
                        
                        setAnswerCoordinates();
                        info.expand = TRUE;
                    }
                }
            }
            
            for (int i = 0; i < 3; i++) {
                
                if (answerCount <= 2) {
                answers[quizNumber][(answerCount*3) + i].checkAnswer();
//                cout << answers[quizNumber][i].correct << endl;
                if (answers[quizNumber][(answerCount*3) + i].pressed) {
                    cout << "Is this correct? " << answers[quizNumber][(answerCount*3) + i].correct << " " << answers[quizNumber][(answerCount*3) + i].text << endl;
                    if (answers[quizNumber][(answerCount*3) + i].correct == true) {
                        text.setText("Good job!");
                    } else {
                        text.setText("Sorry, the correct answer is:");
                        text.wrapTextArea(400, 200);
                    }
                    answers[quizNumber][(answerCount*3) + i].pressed = false;
                }
                }
            }
            
            previous = answerCount;
        }
        
    } else if (state == 7) {
        if (prevState == 6) {
            
            cout << "PREV STATE 6" << endl;
            
            text.setText("Pop 5 more particles and pay attention... there will be a POP QUIZ at the end.");
            text.wrapTextArea(400, 400);
            
            
        }
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
    } else if (state == 6){
        if (prevState == 5) {
            cout << "PREV STATE 5" << endl;
            std::stringstream convert;
            convert << "Well done! You popped " << coCount << " CO particles and " << dustCount << " particulate particles! Now try to increase your score with a POP QUIZ!" << endl;
            
            text.setText(convert.str());
            text.wrapTextArea(400, 400);
            
            //            score = "\n\nPop to continue.";
            //
            //            scoreText.setText(score);
            //            scoreText.wrapTextX(textWidth);
            
            info.expand = TRUE;
        }
        
        //gameplay
    } else if (state == 5) {
        if (prevState == 4) {
            cout << "PREV STATE 4" << endl;
            beginTimer = ofGetElapsedTimeMillis();
        }
        
        if (timer >= 10) {
            timer = 0;
        } else {
            timer = (ofGetElapsedTimeMillis() - beginTimer)/1000;
        }
    } else if (state == 4) {
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (prevState == 3 || prevState == 9) {
            cout << "PREV STATE 3" << endl;
            textAlpha = 0;
            text.setText("Pop as many particles as you can in 10 seconds by tapping the screen.");
            text.wrapTextArea(450, 350);
        }
    } else if (state == 3) {
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (prevState == 2) {
            cout << "PREV STATE 2" << endl;
            textAlpha = 0;
            text.setText(explainScreen[0]);
            text.wrapTextArea(textWidth, textWidth);
        }
        
        //intro screen
    } else if (state == 1) {
        if (prevState == 0) {
            cout << "PREV STATE 0" << endl;
            textAlpha = 0;
            
            text.setText(introscreen);
            text.wrapTextArea(500, 300);
            
            //attractor mode
        }
        
        buttons[0].update();
        buttons[1].update();
        
    }  else if (state == 0) {
        
        if (prevState != 0) {
            
        }
        
        timer = (ofGetElapsedTimeMillis() - beginTimer)/1000;
        
        if (info.contract == FALSE && info.d <=10) {
            info.expand = TRUE;
        }
        
        if (timer >= 5) {
            if (textAlpha <= -50) {
                timer = 0;
                attractorText = ofRandom(introText.size()-1);
                text.setText(introText[attractorText]);
                text.wrapTextX(textWidth);
                text.wrapTextForceLines(3);
                //                cout << attractorText << endl;
                beginTimer = ofGetElapsedTimeMillis();
            }
        } else if (timer >= 0) {
            //            if (info.contract == FALSE && info.d < 10) {
            //
            //                    info.expand = TRUE;
            //
            //            }
        }
        
        cout << "expand state " << info.expand << endl;
        cout << "contract state " << info.contract << endl;
        cout << "timer: " << timer << endl;
    }
    
    info.update();
    info.checkPosition();
    
    thread.unlock();
    
    
    //    cout << "Prev State: " << prevState << " State: " << state << endl;
    idleTimer = (ofGetElapsedTimeMillis() - beginIdle)/1000;
    
    if (idleTimer > 30 && state != 0) {
        if (info.d > 100 && info.expand == FALSE) {
            info.contract = TRUE;
            beginning = true;
            cout << "STATE RESET TO 0" << endl;
        }
        
        
    }
    
    if (state != 0) {
        if (beginning == true && info.d <= 10) {
            state = 0;
            beginning = false;
        }
    }
    
    if (state < 1 || state > 2) {
        buttons[2].update();
        buttons[3].update();
        if (infoWindow) {
            
        }
    }
    prevState = state;
    
}


//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(50);
    ofSetColor(255, 255, 255);
    
//        video.draw(0, 0);
    
    glPushMatrix();
    ofTranslate(1920,1080);
    ofRotate(90);
    video.draw(-1080,850);
    glPopMatrix();
    
    ofFill();
    ofSetColor(0, 100);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    for (int i = 0; i < particles.size(); i++) {
        particles[i].render();
    }
    
    info.render();
    
    std::stringstream colevel;
    std::stringstream dustlevel;
    
    colevel << "Current CO Level: " << co << endl;
    dustlevel << "Current Particulate Level: " << dust << endl;
    
    ofSetColor(255);
    ofDrawBitmapString(colevel.str(), 50, 50);
    ofDrawBitmapString(dustlevel.str(), 50, 65);
    if (state == 9) {
        if (info.d > 100) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
        } else {
            textAlpha = -50;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
        
    } else if (state == 8) {
        
        moreInfo.setColor (255, 255, 255, a);
        moreInfo.drawCenter(ofGetWidth()/2, 100);
        
        if (info.d > 100) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
        } else {
            textAlpha = -50;
        }
        totalHeight = text.getHeight() + answer1.getHeight(); + answer2.getHeight() + answer3.getHeight();
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-totalHeight/2);
        
        if (quizTime == true) {
            
            if (answerCount <= 2) {
            
            answers[quizNumber][answerCount*3].renderBackground(ofGetWidth()/2 - 215, ofGetHeight()/2 + 100);
            answers[quizNumber][answerCount*3+1].renderBackground(ofGetWidth()/2, ofGetHeight()/2 + 100);
            answers[quizNumber][answerCount*3+2].renderBackground(ofGetWidth()/2 + 215, ofGetHeight()/2 + 100);
            
            answer1.setColor(255, 255, 255, 255);
            answer2.setColor(255, 255, 255, 255);
            answer3.setColor(255, 255, 255, 255);
            
            answer1.drawCenter(answers[quizNumber][answerCount*3].x, answers[quizNumber][answerCount*3].y-answer1.getWidth()/2);
            answer2.drawCenter(answers[quizNumber][answerCount*3+1].x, answers[quizNumber][answerCount*3+1].y-answer1.getWidth()/2);
            answer3.drawCenter(answers[quizNumber][answerCount*3+2].x, answers[quizNumber][answerCount*3+2].y-answer1.getWidth()/2);
            
//            cout << answers[quizNumber][answerCount*3].text << endl;
            }
        } else {
            
            if (info.d >= 100) {
                string call = "Got it? Pop to continue.";
                callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 75));
            }
            
        }
        
        
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
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
            
            string call = "Pop to start!";
            
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 100));
        }
        
    } else if (state == 6) {
        std::stringstream convert;
        convert << coCount + dustCount;
        
        if (info.d >= 100) {
            
            if (textAlpha < 255) {
                textAlpha += 10;
            }
            
            int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);
            text.setColor(255, 255, 255, mappedAlpha);
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - text.getHeight()/2);
            
            string call = "Pop to continue.";
            
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 100));
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
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
        
        string call = "Pop to start!";
        ofSetColor(255);
        callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+(info.d-100));
        
    } else if (state == 3) {
        
        if (textAlpha < 255) {
            textAlpha += 10;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-(text.getHeight()/2+25));
        
        string call = "Pop to continue.";
        ofSetColor(255);
        callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+ (info.d-75));
        
    } else if (state == 2) {
        
        if (imageAlpha >= 0 && textAlpha >= -50) {
            textAlpha -= 10;
        } else if (imageAlpha >= 0 && textAlpha <= -50){
            imageAlpha -= 10;
        } else if (imageAlpha <= 0) {
            state = 3;
            intro = false;
            //            cout << "state: " << state << endl;
        }
        
        ofSetColor(100, imageAlpha);
        introImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, -60, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
        
        buttons[0].render();
        buttons[1].render();
        
        //        cout << "textAlpha: " << textAlpha << endl;
        
    } else if (state == 1) {
        
        if (intro == true) {
            //draw image
            if (imageAlpha <= 255){
                imageAlpha += 10;
            } else if (imageAlpha >= 255) {
                if (textAlpha <= 255) {
                    textAlpha += 10;
                }
            }
            
        } else if (intro == false) {
            
        }
        
        ofSetColor(100, imageAlpha);
        introImage.draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawRight(ofGetWidth()-100, ofGetHeight()/2+100);
        
        buttons[0].render();
        buttons[1].render();
        
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
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
            string call = "Pop to begin!";
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+(info.d - 100));
        }
        
    }
    
    if (state < 1 || state > 2) {
        buttons[2].render();
        buttons[3].render();
        if (infoWindow) {
            displayInfoWindow();
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
    
    beginIdle = ofGetElapsedTimeMillis();
    
    for (int i = 0; i < particles.size(); i++) {
        
        if (particles[i].on == true && info.d <= 0) {
            
            
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
        if (state == 9) {
            info.contract = TRUE;
            
            if (ofGetMouseX() <= ofGetWidth()/2) {
                next = true;
                beginning = false;
            } else {
                beginning = true;
                next = false;
            }
        }
        else if (state == 8) {
            
            if (quizTime == TRUE) {
                if (answerCount > 1) {
                    quizTime = false;
                    info.contract = TRUE;
                    state = 9;
                    answerCount = 0;
                    clickCount = 0;
                } else {
                    if (ofGetMouseY() >= ofGetHeight()/2 + 150) {
                        answerCount++;
                        cout << "answer count: " << answerCount << endl;
                    }
                }
                //                factSelect = 0;
            } else if (quizTime == false) {
                info.contract = TRUE;
                factSelect++;
                cout << "fact select: " << factSelect << endl;
            }
            
            //            textAlpha = -50;
            
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
            info.contract = TRUE;
            state = 4;
        } else if (state == 0 && infoWindow == false) {
            info.contract = TRUE;
        }
    }
    
    
    if (state == 8) {
        if (quizTime == TRUE) {
            for (int i = 0; i < 3; i++) {
                if (answers[quizNumber][(answerCount*3) + i].select) {
                    answers[quizNumber][(answerCount*3) + i].pressed = true;
                } else {
                    answers[quizNumber][(answerCount*3) + i].pressed = false;
                }
            }
        }
        
        
        if (info.d <= 0) {
            
            if (quizTime == false) {
                
                text.setText(facts[quizNumber][factSelect]);
                text.wrapTextArea(textWidth, textWidth);
            }
        }
    }
    
    if (state == 1) {
        if (buttons[1].on) {
            imageCount ++;
            cout << imageCount << endl;
        } else if (buttons[0].on) {
            state++;
        }
    } else if (state == 0 && infoWindow == false) {
        if (infoWindow == false) {
            if (info.on) {
                if (prevState == 0) {
                    state++;
                    intro = true;
                }
            }
        }
    }
    
    if (infoWindow == false) {
        if (buttons[2].on) {
            infoWindow = true;
            
            cout << "INFO WINDOW APPEARS" << endl;
        }
    } else {
        
        if(ofDist(ofGetMouseX(), ofGetMouseY(), info.x, info.y) > info.d) {
            infoWindow = false;
            cout << "INFO WINDOW GONE" << endl;
        }
    }
    
    
    if (buttons[3].pressed && state !=0) {
        if (info.d > 100 && info.expand == FALSE) {
            info.contract = TRUE;
            cout << "EXIT BUTTON PRESSED" << endl;
        }
        
        beginning = true;
        text.setText(introText[attractorText]);
        text.wrapTextArea(300, 300);
        textAlpha = -50;
    }
    
    
    cout << "state: " << state << endl;
    
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

void ofApp::loadText() {
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
    
    f.open(ofToDataPath("about.txt").c_str());
    
    while (f != NULL) {
        string s;
        getline(f, s);
        infoText.push_back(s);
        cout << s << endl;
    }
    
    f.close();
    
    infoWindowText1.setText(infoText[0]);
    infoWindowText1.wrapTextArea(700, 200);
    infoWindowText2.setText(infoText[1]);
    infoWindowText2.wrapTextArea(700, 150);
    infoWindowText3.setText(infoText[2]);
    infoWindowText3.wrapTextArea(700, 50);
    
    for (int i = 0; i < 3; i++) {
        ifstream g;
        std::stringstream index;
        index << i << ".txt";
        g.open(ofToDataPath(index.str()).c_str());
        
        while(g != NULL) {
            string s;
            getline(g, s);
            facts[i].push_back(s);
            
        }
        
        g.close();
    }
    
    for (int i = 0; i < 3; i++) {
        std::stringstream f;
        f << "quiz" << i << ".txt";
        
        loadQuiz(f.str(), i);
    }
}

void ofApp::loadQuiz(string filename, int index) {
    
    ifstream q;
    q.open(ofToDataPath(filename).c_str());
    
    while (q != NULL) {
        string str;
        getline(q, str);
        quiz[index].push_back(str);
    }
    
    for (int i = 1; i < 12; i++) {
        
        if (i % 4 != 0 ){
            std::string s = quiz[index][i];
            std::string delimiter = ";";
            std::string token = s.substr(0, s.find(delimiter));
            s.erase(0, s.find(delimiter) + delimiter.length());
            
            if (s == "correct"){
                answers[index].push_back(Answer(token, TRUE));
            } else {
                answers[index].push_back(Answer(token, FALSE));
            }
            
            //            cout << token << " " << s << endl;
        }
    }
    
    q.close();
    
    
}

void ofApp::displayInfoWindow() {
    ofFill();
    ofSetColor(0, 0, 0, 225);
    ofRect(100, ofGetHeight()/2 - (ofGetHeight()/3)/2, ofGetWidth()-200, ofGetHeight()/3);
    
    infoWindowText1.setColor(255, 255, 255, 255);
    infoWindowText1.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 75);
    
    infoWindowText2.setColor(255, 255, 255, 255);
    infoWindowText2.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 285);
    
    infoWindowText3.setColor(255, 255, 255, 255);
    infoWindowText3.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 460);
}

void ofApp::setAnswerCoordinates() {
    
    
    
}

void ofApp::exit() {
    
    thread.stop();
}
