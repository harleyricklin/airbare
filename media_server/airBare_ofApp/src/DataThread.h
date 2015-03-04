//
//  DataThread.h
//  airBare_test
//
//  Created by Roopa Vasudevan on 1/4/15.
//
//

#ifndef airBare_test_DataThread_h
#define airBare_test_DataThread_h

#include "ofMain.h"
#include "ofxJSON.h"

class DataThread : public ofThread {
    
protected:
    
    
public:
    
    DataThread() {
        
    }
    
    void start() {
        startThread();
    }
    
    void stop() {
        stopThread();
    }
    
    void threadedFunction() {
        while(isThreadRunning()) {
            
            collectData();
            
        }
    }
    
    void collectData() {
        
        std::string url = "https://www.manylabs.org/data/api/v1/datasets/549/rows/";
        
        if (!response.open(url)) {
            ofLogWarning("collectData()") << "Failed to parse JSON";
        }
        
        int lastCollected = response["results"].size()-1;
        
        co2Value = response["results"][lastCollected]["data"][4].asFloat();
        dustValue = response["results"][lastCollected]["data"][3].asFloat();
        
//
//        cout << "Data collected!" << endl;
//        cout << "CO2: " << co2Value << endl;
//        cout << "Dust: " << dustValue << endl;
    }

    
    ofxJSONElement response;
    float co2Value;
    float dustValue;
    
};

#endif
