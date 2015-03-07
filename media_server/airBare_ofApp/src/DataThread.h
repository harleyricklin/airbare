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
        
        std::string baseurl = "https://www.manylabs.org/data/api/v1/datasets/563/rows/";
        std::stringstream newurl;
        std::string url;
        
        if (!baseresponse.open(baseurl)) {
            ofLogWarning("collectData()") << "Failed to parse JSON";
        }
        
        
        int lastPage = baseresponse["count"].asInt()/100;
        lastPage += 1;
//        cout << lastPage << endl;
        
        newurl << baseurl << "?page=" << lastPage << "&format=json";
//        newurl << baseurl;
        url = newurl.str();
        
        if (!response.open(url)) {
            ofLogWarning("collectData()") << "Failed to parse JSON";
        }
        
        int lastCollected = response["results"].size()-1;
        
        coValue = response["results"][lastCollected]["data"][6].asFloat();
        dustValue = response["results"][lastCollected]["data"][5].asFloat();
        tempValue = response["results"][lastCollected]["data"][1].asFloat();
//
//        cout << "Data collected!" << endl;
//        cout << "co: " << coValue << endl;
//        cout << "Dust: " << dustValue << endl;
    }

    
    ofxJSONElement baseresponse;
    ofxJSONElement response;
    float coValue;
    float dustValue;
    float tempValue;
    int lastPage;
    
};

#endif
