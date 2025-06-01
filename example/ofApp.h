#pragma once

#include "ofMain.h"
#include "ofxPinkTrombone.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    
    void audioOut(ofSoundBuffer& output);
    
    ofxPinkTrombone voice;
    ofSoundStream soundStream;
    
    // Visualization
    vector<float> waveform;
    vector<float> tractShape;
    vector<float> noseShape;
    
    // Parameters
    float frequency;
    float tenseness;
    float tongueX, tongueY;
    bool isPlaying;
    
    float tongueIndexDisplay;
    float tongueDiameterDisplay;
};
