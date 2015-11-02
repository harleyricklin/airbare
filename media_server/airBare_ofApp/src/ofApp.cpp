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
    
    for (int i = 0; i < 2; i++) {
        introImages.push_back(ofImage());
        std::stringstream filename;
        filename << "intro" << i << ".jpg";
        
        introImages[i].loadImage(filename.str());
        
//        cout << filename.str() << " loaded" << endl;
    }
    
    
    introOverlay.loadImage("infoOverlay.png");
    
    for (int i = 0; i < 4; i++) {
        buttons.push_back(Button(i));
    }
    
    myFont.loadFont("Apercu Medium.otf", 35);
    callFont.loadFont("Apercu Medium.otf", 20);
    scoreText.loadFont("Apercu Medium.otf", 35);
    headerText.loadFont("Apercu Light.otf", 13);
    inhalerText.loadFont("Apercu Light.otf", 10);
    
    text.init("Apercu Medium.otf", 50);
    answer1.init("Apercu Medium.otf", 35);
    answer2.init("Apercu Medium.otf", 35);
    answer3.init("Apercu Medium.otf", 35);
    
    infoWindowText1.init("Apercu Medium.otf", 35);
    infoWindowText2.init("Apercu Medium.otf", 35);
    infoWindowText3.init("Apercu Medium.otf", 35);
    
    loadText();
    
    for (int i = 0; i < 3; i++) {
        captions[i].init("Apercu Medium.otf", 35);
        captions[i].setText(introScreen[i]);
        captions[i].wrapTextArea(500, 200);
    }

    alignment = OF_TEXT_ALIGN_CENTER;
    
    thread.start();
    
    state = 0;
    prevState = 0;
    timer = 0;
    coCount = 0;
    dustCount = 0;
    textAlpha = 0;
    introp2Alpha = 0;
    imageAlpha = 0;
    imageCount = 0;
    answerCount = 0;
    previous = 0;
    factSelect = 0;
    correctTracker = 0;
    
    idleTimer = 0;
    beginIdle = 0;
    highScore = 0;
    totalScore = 0;
    
    intro = false;
    infoWindow = false;
    quizTime = false;
    next = false;
    beginning = false;
    
    float area = (ofGetWidth()/3)*(ofGetWidth()/3);
    textWidth = 500;
    
    attractorText = ofRandom(6);
    text.setText(introText[attractorText]);
    text.wrapTextArea(300, 300);
    
    
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
    
    int totalco = int(ofMap(thread.coValue, 0, 500, 10, 100));
    int totaldust = int(ofMap(thread.dustValue, 0, 500, 10, 100));
    int totalNum = totalco + totaldust;
    
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
        
//        cout << "CO particles on screen: " << coparticles << endl;
//        cout << "PM particles on screen: " << dustparticles << endl;
//        
//        cout << "CO particles needed: " << totalco << endl;
//        cout << "PM particles needed: " << totaldust << endl;
        
        if (coparticles < totalco) {
            int diff = totalco - coparticles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 0));
        } else if (coparticles > totalco){
//            cout << "CO particles present: " << coparticles << endl;
            int diff = coparticles - totalco;
            int counter = 0;
            for (int i = particles.size() - 1; i >= 10; i--) {
                if (particles[i].type == 0 && counter < diff) {
                    particles.erase(particles.begin() + i);
                    counter++;
                    cout << "erased a particle" << endl;
                }
            }
        }
        
        if (dustparticles < totaldust){
            int diff = totaldust - dustparticles;
            particles.push_back(particle(ofRandomWidth(), ofRandomHeight(), ofRandomWidth(), ofRandomHeight(), 1));
        } else if (dustparticles > totaldust){
//            cout << "PM particles present: " << dustparticles << endl;
            int diff = dustparticles - totaldust;
            int counter = 0;
            
            for (int i = particles.size() - 1; i >= 10; i--) {
                if (particles[i].type == 1 && counter < diff) {
                    particles.erase(particles.begin() + i);
                    counter++;
                    cout << "erased a particle" << endl;
                }
            }
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
        
        currentRed = particles[i].dustRed;
        particles[i].update();
        checkEdges();
        particles[i].checkPosition(mouseX, mouseY);
        
        if (state == 8) {
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
                } else {
                    dustCount++;
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
            if (quizNumber >= 2) {
                if (totalScore > highScore){
                    highScore = totalScore;
                    text.setText("You have the new HIGH SCORE. You're a Premium Particle Popper!");
                    text.wrapTextArea(450, 450);
                    info.expand = TRUE;
//                    cout << "High Score: " << highScore << endl;
                    
                } else if (totalScore < highScore) {
                    text.setText("Thanks for playing!");
                    text.wrapTextArea(400, 400);
                    info.expand = TRUE;
//                    cout << "High Score: " << highScore << endl;
                }
                
            } else {
                if (correctTracker > 0) {
                    text.setText("Well done! Take another quiz to increase your score.");
                } else {
                    text.setText("Try again! Take another quiz to increase your score.");
                }
                text.wrapTextArea(400, 400);
                info.expand = TRUE;
            }
            
        }
        
        if (next == true && info.d <= 10) {
            state = 8;
            factSelect = 0;
            quizNumber++;
            next = false;
            quizTime = false;
            correctTracker = 0;
        } else if (beginning == true && info.d <= 10){
            state = 0;
            beginning = false;
        }
        
        
    } if (state == 8) {
        if (prevState == 7) {
//            cout << "PREV STATE 7" << endl;
            coCount = 0;
            dustCount = 0;
        }
        
        if (quizTime == true) {
            ofColor answerFill = info.fill;
            
            answerFill.setBrightness(answerFill.getBrightness()-30);
            
            if (info.contract == FALSE) {
                if (answerCount <= 2){
                    if (info.d <= 10 || answerCount != previous){
                        text.setText(quiz[quizNumber][answerCount*4]);
                        text.wrapTextArea(400, 200);
                        answer1.setText(answers[quizNumber][answerCount*3].text);
                        answer1.wrapTextArea(110, 140);
                        answer2.setText(answers[quizNumber][(answerCount*3)+1].text);
                        answer2.wrapTextArea(100, 140);
                        answer3.setText(answers[quizNumber][(answerCount*3)+2].text);
                        answer3.wrapTextArea(90, 140);
                        
                        info.expand = TRUE;
                    }
                }
            }
            
            for (int i = 0; i < 3; i++) {
                answers[quizNumber][(answerCount*3) + i].fill = answerFill;
                
                if (answerCount <= 2) {
                answers[quizNumber][(answerCount*3) + i].checkAnswer();
                if (answers[quizNumber][(answerCount*3) + i].pressed) {
//                    cout << "Is this correct? " << answers[quizNumber][(answerCount*3) + i].correct << " " << answers[quizNumber][(answerCount*3) + i].text << endl;
                    for (int j = 0; j < 3; j++) {
                        answers[quizNumber][(answerCount*3) + j].reveal = true;
                    }
                    if (answers[quizNumber][(answerCount*3) + i].correct == true) {
                        text.setText("Nice job! You picked the right answer.");
                        text.wrapTextArea(400, 200);
                        totalScore += 5;
                        correctTracker++;
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
            
//            cout << "PREV STATE 6" << endl;
            
            text.setText("Pop 5 more particles and pay attention... there will be a POP QUIZ at the end.");
            text.wrapTextArea(400, 400);
            
            
        }
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
    } else if (state == 6){
        if (prevState == 5) {
//            cout << "PREV STATE 5" << endl;
            std::stringstream convert;
            convert << "Well done! You popped " << coCount << " CO particles and " << dustCount << " particulate particles! Now try to increase your score with a POP QUIZ!" << endl;
            
            text.setText(convert.str());
            text.wrapTextArea(400, 400);
            
            totalScore = coCount + dustCount;
            info.expand = TRUE;
        }
        
        //gameplay
    } else if (state == 5) {
        if (prevState == 4) {
//            cout << "PREV STATE 4" << endl;
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
//            cout << "PREV STATE 3" << endl;
            textAlpha = 0;
            text.setText("Pop as many particles as you can in 10 seconds by tapping the screen.");
            text.wrapTextArea(450, 350);
        }
    } else if (state == 3) {
        
        if (info.contract == FALSE) {
            info.expand = TRUE;
        }
        
        if (prevState == 2) {
//            cout << "PREV STATE 2" << endl;
            textAlpha = 0;
            text.setText(explainScreen[0]);
            text.wrapTextArea(textWidth, textWidth);
        }
        
        //intro screen
    } else if (state == 1) {
        if (prevState == 0) {
//            cout << "PREV STATE 0" << endl;
            textAlpha = 0;
            introp2Alpha = 0;
            
            text.setText(introScreen[0]);
            introP2.setText(introScreen[1]);
            text.wrapTextArea(500, 150);
            introP2.wrapTextArea(515, 135);
        }
        
        buttons[0].update();
        buttons[1].update();
        
    //attractor mode
    }  else if (state == 0) {
        
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
                beginTimer = ofGetElapsedTimeMillis();
            }
        }

    }
    
    info.update();
    info.checkPosition();
    
    thread.unlock();
    
    idleTimer = (ofGetElapsedTimeMillis() - beginIdle)/1000;
    
    if (idleTimer > 30 && state != 0) {
        if (info.d > 300 && info.expand == FALSE) {
            info.contract = TRUE;
        }
        
        beginning = true;
        
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                answers[j][i].reveal = false;
            }
        }
        answerCount = 0;
        quizNumber = 0;
        //            cout << "STATE RESET TO 0" << endl;

    }
    
    if (state != 0) {
        if (beginning == true && info.d <= 10) {
            textAlpha = -50;
            text.setText(introText[attractorText]);
            text.wrapTextArea(300, 300);
            beginning = false;
            state = 0;
            quizNumber = 0;
            quizTime = false;
            factSelect = 0;
            
//            cout << "Quiz number: " << quizNumber << " Quiz on?: " << quizTime << endl;
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
    
    glPushMatrix();
    ofTranslate(1920,1080);
    ofRotate(90);
    video.draw(-1080,840);
    glPopMatrix();
    
    ofFill();
    ofSetColor(0, 100);
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    for (int i = 0; i < particles.size(); i++) {
        particles[i].render();
    }
    
    header();
    info.render();
    
    if (state == 9) {
        if (info.d > 300) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
            string call = "Pop to move on!";
            
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 75));
        } else {
            textAlpha = -50;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
        
    } else if (state == 8) {
        
        std::stringstream sc;
        sc << totalScore << endl;
        string s = "Score: " + sc.str();
        ofSetColor(255, 255, 255);
        scoreText.drawString(s, ofGetWidth()/2 - scoreText.stringWidth(s)/2, 250);
        
        if (info.d > 300) {
            if (textAlpha < 255) {
                textAlpha += 10;
            }
        } else {
            textAlpha = -50;
        }
        totalHeight = text.getHeight() + answer1.getHeight(); + answer2.getHeight() + answer3.getHeight();
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        
        text.setColor(255, 255, 255, mappedAlpha);
        
        
        if (quizTime == true) {
            
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()-75);
            
            if (answerCount <= 2) {
                
                for (int i = 0; i < 3; i++) {
                    answers[quizNumber][(answerCount*3) + i].a = mappedAlpha;
                }
//                if (textAlpha >= 255) {
                    answers[quizNumber][answerCount*3].renderBackground(ofGetWidth()/2 - 215, ofGetHeight()/2 + 100);
                    answers[quizNumber][answerCount*3+1].renderBackground(ofGetWidth()/2, ofGetHeight()/2 + 100);
                    answers[quizNumber][answerCount*3+2].renderBackground(ofGetWidth()/2 + 215, ofGetHeight()/2 + 100);
//                }
                
                    answer1.setColor(255, 255, 255, mappedAlpha);
                    answer2.setColor(255, 255, 255, mappedAlpha);
                    answer3.setColor(255, 255, 255, mappedAlpha);
                    
                    answer1.drawCenter(answers[quizNumber][answerCount*3].x, answers[quizNumber][answerCount*3].y-answer1.getHeight()/2);
                    answer2.drawCenter(answers[quizNumber][answerCount*3+1].x, answers[quizNumber][answerCount*3+1].y-answer2.getHeight()/2);
                    answer3.drawCenter(answers[quizNumber][answerCount*3+2].x, answers[quizNumber][answerCount*3+2].y-answer3.getHeight()/2);
            }
            
            for (int i = 0; i < 3; i++) {
                if (answers[quizNumber][answerCount*3 + i].reveal == true) {
                    string call = "Got it? Pop to continue.";
                    callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 75));
                }
            }
        } else {
            
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
            if (info.d >= 300) {
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
        
        if (info.d >= 300) {
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
        
        if (info.d >= 300) {
            
            if (textAlpha < 255) {
                textAlpha += 10;
            }
            
            int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);
            text.setColor(255, 255, 255, mappedAlpha);
            text.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - text.getHeight()/2);
            
            if (info.d > 300) {
                string call = "Pop to continue.";
                
                ofSetColor(255);
                callFont.drawString(call, ofGetWidth()/2-callFont.stringWidth(call)/2, ofGetHeight()/2 + (info.d - 100));
            }
        }
        
    } else if (state == 5) {
        std::stringstream convert;
        convert << 10-timer;
        
        if (timer >= 10) {
            state = 6;
//            cout << "STATE 6" << endl;
        }
        
        string s = "Time left: " + convert.str();
        ofSetColor(255);
        myFont.drawString(s, ofGetWidth()/2 - myFont.stringWidth(s)/2, 250);
        
    } else if (state == 4 && info.d >= 300) {
        
        if (textAlpha < 255) {
            textAlpha += 10;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-text.getHeight()/2);
        
        if (info.d > 300) {
            string call = "Pop to start!";
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+(info.d-100));
        }
        
    } else if (state == 3) {
        
        if (textAlpha < 255) {
            textAlpha += 10;
        }
        
        int mappedAlpha = ofMap(textAlpha, -50, 260, 0, 255);
        text.setColor(255, 255, 255, mappedAlpha);
        text.drawCenter(ofGetWidth()/2, ofGetHeight()/2-(text.getHeight()/2+25));
        
        if (info.d > 300) {
            string call = "Pop to continue.";
            ofSetColor(255);
            callFont.drawString(call, ofGetWidth()/2 - callFont.stringWidth(call)/2, ofGetHeight()/2+ (info.d-75));
        }
        
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
        introImages[imageCount].draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, -60, 260, 0, 255);

        captions[imageCount].setColor(255, 255, 255, mappedAlpha);
        captions[imageCount].drawRight(ofGetWidth()-100, ofGetHeight()/2 + 300 + text.getHeight() + 20);
        
        ofSetColor(255, 255, 255, imageAlpha-50);
        headerText.drawString("Photo by Al Blanc, November 28 1943.", 10, 30);
        
        buttons[0].a = imageAlpha;
        buttons[1].a = imageAlpha;
        
        buttons[0].render();
        buttons[1].render();
        
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
        introImages[imageCount].draw(0, 0, ofGetWidth(), ofGetHeight());
        
        int mappedAlpha = ofMap(textAlpha, 0, 260, 0, 255);

        captions[imageCount].setColor(255, 255, 255, mappedAlpha);
        captions[imageCount].drawRight(ofGetWidth()-100, ofGetHeight()/2 + 300 + text.getHeight() + 20);
        
        ofSetColor(255, 255, 255, imageAlpha-50);
        headerText.drawString("Photo by Al Blanc, November 28 1943.", 10, 30);
        
        buttons[0].a = imageAlpha;
        buttons[1].a = imageAlpha;
        
        buttons[0].render();
        buttons[1].render();
        
    } else if (state == 0) {
        
        if(info.d >= 300 && info.contract == FALSE){
            if (timer >= 5){
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
        buttons[2].a = 200;
        buttons[3].a = 200;
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
//    cout << factType << endl;
    
    if (info.on == TRUE && info.expand == FALSE) {
        if (state == 9) {
            if (ofDist(ofGetMouseX(), ofGetMouseY(), info.x, info.y) <= info.d) {
                info.contract = TRUE;
                
                if (quizNumber >= 2) {
                    beginning = true;
                    next = false;
                    quizNumber = 0;
                    quizTime = false;
                    answerCount = 0;
                } else {
                    
                    next = true;
                    beginning = false;
                }
            }
        }
        else if (state == 8) {
            
            if (quizTime == TRUE) {
                if (answerCount >= 2 && ofGetMouseY() > ofGetHeight()/2 + 200 && ofGetMouseY() <= ofGetHeight()/2 + info.d) {
                    quizTime = false;
                    info.contract = TRUE;
                    state = 9;
                    answerCount = 0;
                    clickCount = 0;
                    for (int i = 0; i < 9; i++) {
                        for (int j = 0; j < 3; j++) {
                            answers[j][i].reveal = false;
                        }
                    }
                } else {
                    
                    if (ofGetMouseY() > ofGetHeight()/2 + 200 && ofGetMouseY() <= ofGetHeight()/2 + info.d) {
                        
                        for (int i = 0; i < 3; i++) {
                            for (int j = 0; j < 3; j++) {
                                answers[j][(answerCount*3) + i].reveal = false;
                            }
                        }
                        answerCount++;
                    }
                }
//                cout << "answer count: " << answerCount << endl;
                
            } else if (quizTime == false) {
                info.contract = TRUE;
                factSelect++;
//                cout << "fact select: " << factSelect << endl;
            }
            
            
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
                
                if (answers[quizNumber][(answerCount*3) + i].a >= 255) {
                    if (answers[quizNumber][(answerCount*3) + i].select) {
                        answers[quizNumber][(answerCount*3) + i].pressed = true;
                    } else {
                        answers[quizNumber][(answerCount*3) + i].pressed = false;
                    }
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
            textAlpha = 0;
            if (imageCount == 1) {
                imageCount = 0;
            } else {
                imageCount ++;
            }
            
//            cout <<"image count " << imageCount << endl;
        } else if (buttons[0].on) {
            state++;
        }
    } else if (state == 0) {
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
            
//            cout << "INFO WINDOW APPEARS" << endl;
        }
    } else {
        
        if(ofDist(ofGetMouseX(), ofGetMouseY(), info.x, info.y) > info.d) {
            infoWindow = false;
//            cout << "INFO WINDOW GONE" << endl;
        }
    }
    
    
    if (buttons[3].on && state !=0) {
        if (info.d > 300 && info.expand == FALSE) {
            info.contract = TRUE;
        }
        
//        cout << "EXIT BUTTON PRESSED" << endl;
        beginning = true;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 3; j++) {
                answers[j][i].reveal = false;
            }
        }
        answerCount = 0;
        quizNumber = 0;
        text.setText(introText[attractorText]);
        text.wrapTextArea(300, 300);
        textAlpha = -50;
    }
    
    
//    cout << "state: " << state << endl;
    
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
    }
    
    fin.close();
    
    ifstream f;
    f.open(ofToDataPath("introscreen.txt").c_str());
    
    while(f != NULL) {
        string s;
        getline(f, s);
        introScreen.push_back(s);
        introscreen = introscreen + s + "\n\n";
        
        captions.push_back(ofxTextBlock());
    }
    
    f.close();
    
    f.open(ofToDataPath("explaintext.txt").c_str());
    
    while(f != NULL) {
        string s;
        getline(f, s);
        explainScreen.push_back(s);
    }
    
    f.close();
    
    f.open(ofToDataPath("about.txt").c_str());
    
    while (f != NULL) {
        string s;
        getline(f, s);
        infoText.push_back(s);
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
    
    ifstream h;
    h.open(ofToDataPath("key.txt").c_str());
    
    for (int i = 0; i < 2; i++) {
        string str;
        getline(h, str);
        if (i == 0) {
            thread.apiKey = str;
        } else {
            thread.tempKey = str;
        }
    }
    
    cout << "api Key: " << thread.apiKey << " temp Key: " << thread.tempKey << endl;
    h.close();
    
    
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
        }
    }
    
    q.close();
    
    
}

void ofApp::header() {
    ofFill();
    ofSetColor(0, 0, 0, 150);
    ofRect(0, 0, ofGetWidth(), 140);
    
    int currentHour;
    
    std::stringstream colevel;
    std::stringstream dustlevel;
    std::stringstream dateTime;
    std::stringstream location;
    std::stringstream allTimeHigh;
    std::stringstream inhalerUsage;
    
    string twitter = "Follow @airbareky on Twitter for more!";

    colevel << "Current CO level: " << co << endl;
    dustlevel << "Current PM2.5 level: " << dust << endl;
    inhalerUsage << "Expected number of inhaler usage events: " << int(usage) << endl;
    allTimeHigh << "High score: " << highScore << endl;
    location << "Downtown Louisville, 40202" << endl;
    dateTime << "Date: " << ofGetMonth() << "/" << ofGetDay() << "/" << ofGetYear() << ", ";
    
    if (ofGetHours() > 12) {
        currentHour = ofGetHours() - 12;
        if (ofGetMinutes() < 10) {
            dateTime << currentHour << ":0" << ofGetMinutes() << " PM" << endl;
        } else {
           dateTime << currentHour << ":" << ofGetMinutes() << " PM" << endl;
        }
    } else {
        currentHour = ofGetHours();
        if (ofGetMinutes() < 10) {
            dateTime << currentHour << ":0" << ofGetMinutes() << " AM" << endl;
        } else {
            dateTime << currentHour << ":" << ofGetMinutes() << " AM" << endl;
        }
    }
    
    ofFill();
    ofSetColor(0, 180, 158);
    ofCircle(28, 23, 8);
    ofSetColor(currentRed, 11, 221);
    ofCircle(28, 48, 8);
    
    ofSetColor(255);
    headerText.drawString(colevel.str(), 50, 30);
    headerText.drawString(dustlevel.str(), 50, 55);
    headerText.drawString(inhalerUsage.str(), 20, 80);

    headerText.drawString(dateTime.str(), ofGetWidth()-20-headerText.stringWidth(dateTime.str()), 30);
    headerText.drawString(location.str(), ofGetWidth()-20-headerText.stringWidth(location.str()), 55);
    headerText.drawString(allTimeHigh.str(), ofGetWidth()-20-headerText.stringWidth(allTimeHigh.str()), 80);
    headerText.drawString(twitter, ofGetWidth()-20-headerText.stringWidth(twitter), 105);
    
    for (int i = 0; i < 15; i++) {
        ofSetColor(100 + int(10.33*i), 11, 221);
        ofCircle(50 + (22.5*i), 105, 6);
    }
    
    ofSetColor(255);
    inhalerText.drawString("0", 20, 109);
    inhalerText.drawString(ofToString(MAXINHALER) + "+", 385, 109);
}

void ofApp::displayInfoWindow() {
    ofFill();
    ofSetColor(0, 0, 0, 200);
    ofRect(100, ofGetHeight()/2 - (ofGetHeight()/3)/2, ofGetWidth()-200, ofGetHeight()/3);
    
    ofSetColor(255, 175);
    introOverlay.draw(100, ofGetHeight()/2 - (ofGetHeight()/3)/2);
    
    infoWindowText1.setColor(255, 255, 255, 255);
    infoWindowText1.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 75);
    
    infoWindowText2.setColor(255, 255, 255, 255);
    infoWindowText2.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 285);
    
    infoWindowText3.setColor(255, 255, 255, 255);
    infoWindowText3.drawCenter(ofGetWidth()/2, ofGetHeight()/2 - (ofGetHeight()/3)/2 + 460);
}


void ofApp::exit() {
    
    thread.stop();
}
