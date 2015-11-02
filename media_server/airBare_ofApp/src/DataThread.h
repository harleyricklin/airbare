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
            if (startingTime == 0 || ofGetElapsedTimeMillis() - startingTime >= 90000) {
                cout << "FETCHING DATA" << endl;
                collectData();
                startingTime = ofGetElapsedTimeMillis();
            }
            
        }
    }
    
    void collectData() {
        
        std::string baseurl = "https://api.forecast.io/forecast/" + tempKey + "/" + lat + "," + lon;
        
        std::string epaURL = "http://www.airnowapi.org/aq/observation/latLong/current/?format=application/json&latitude=" + lat + "&longitude=" + lon + "&distance=25&API_KEY=" + apiKey;
        
        if (!response.open(baseurl)) {
            ofLogWarning("collectData()") << "Failed to parse JSON";
        }
        
        tempValue = response["currently"]["temperature"].asFloat();
        
        cout << "temperature: " << tempValue << endl;
        
        if (!epaResponse.open(epaURL)) {
            ofLogWarning("collectData()") << "Failed to parse JSON";
        }
        
        dustValue = epaResponse[1]["AQI"].asFloat();
        coValue = epaResponse[0]["AQI"].asFloat();
        cout << "DUST VALUE FROM EPA: " << dustValue << endl;

        // sensor temperature is in Celsius, convert to Fahrenheit
        tempValue = tempValue * 9 / 5 + 32;
//
//        cout << "Data collected!" << endl;
//        cout << "co: " << coValue << endl;
//        cout << "Dust: " << dustValue << endl;
    }

    ofxJSONElement response;
    ofxJSONElement epaResponse;
    float coValue;
    float dustValue;
    float tempValue;
    int lastPage;
    
    string apiKey;
    string tempKey;
    
    string lat = "38.328732";
    string lon = "-85.764771";
    
    int startingTime = 0;
    
};

#endif
