#include "ofApp.h"

void ofApp::setup() {
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    // Setup audio
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 1;
    settings.numInputChannels = 0;
    settings.bufferSize = 512;
    soundStream.setup(settings);
    
    // Setup voice synthesizer
    voice.setup(44100, 512);
    voice.setFrequency(220.0f);
    voice.setTenseness(0.6f);
    voice.setVowelA(); // Start with 'A' sound
    
    // Initialize parameters
    frequency = 220.0f;
    tenseness = 0.6f;
    tongueX = 0.5f;
    tongueY = 0.5f;
    isPlaying = true;
    
    // Initialize visualization buffers
    waveform.resize(512);
    tractShape.resize(44);
    noseShape.resize(28);
    
    // Fill with initial values
    for (int i = 0; i < tractShape.size(); i++) {
        tractShape[i] = 1.0f; // Default diameter
    }
    for (int i = 0; i < noseShape.size(); i++) {
        noseShape[i] = 1.0f; // Default diameter
    }
}

void ofApp::update() {
    // Copy tract data for visualization - ALWAYS update this
    float* tractData = voice.getTractDiameters();
    float* noseData = voice.getNoseDiameters();
    
    if (tractData) {
        int tractLength = voice.getTractLength();
        // Make sure we don't exceed array bounds
        int copyLength = min(tractLength, (int)tractShape.size());
        for (int i = 0; i < copyLength; i++) {
            tractShape[i] = tractData[i];
        }
    }
    
    if (noseData) {
        int noseLength = voice.getNoseLength();
        // Make sure we don't exceed array bounds
        int copyLength = min(noseLength, (int)noseShape.size());
        for (int i = 0; i < copyLength; i++) {
            noseShape[i] = noseData[i];
        }
    }
}

void ofApp::draw() {
    ofBackground(20);
    
    // Draw instructions
    ofSetColor(255);
    ofDrawBitmapString("Pink Trombone Voice Synthesizer", 20, 20);
    ofDrawBitmapString("Mouse: Control tongue position", 20, 40);
    ofDrawBitmapString("Keys: A,E,I,O,U - vowels, Space - silence", 20, 60);
    ofDrawBitmapString("Up/Down arrows: pitch, Left/Right: tenseness", 20, 80);
    
    // Draw waveform
    ofSetColor(100, 200, 255);
    ofNoFill();
    ofBeginShape();
    for (int i = 0; i < waveform.size(); i++) {
        float x = ofMap(i, 0, waveform.size(), 50, ofGetWidth() - 50);
        float y = ofGetHeight() / 2 + waveform[i] * 100;
        ofVertex(x, y);
    }
    ofEndShape();
    
    // Draw vocal tract with better visualization
    ofSetColor(255, 100, 100);
    ofFill();
    ofBeginShape();
    // Top of tract
    for (int i = 0; i < tractShape.size(); i++) {
        float x = ofMap(i, 0, tractShape.size(), 50, ofGetWidth() - 50);
        float y = ofGetHeight() * 0.7f - tractShape[i] * 30;
        ofVertex(x, y);
    }
    // Bottom of tract (reversed)
    for (int i = tractShape.size() - 1; i >= 0; i--) {
        float x = ofMap(i, 0, tractShape.size(), 50, ofGetWidth() - 50);
        float y = ofGetHeight() * 0.7f + tractShape[i] * 30;
        ofVertex(x, y);
    }
    ofEndShape();
    
    // Draw center line of tract for reference
    ofSetColor(255, 50, 50);
    ofNoFill();
    ofDrawLine(50, ofGetHeight() * 0.7f, ofGetWidth() - 50, ofGetHeight() * 0.7f);
    
    // Draw nose cavity
    ofSetColor(100, 255, 100);
    ofFill();
    ofBeginShape();
    for (int i = 0; i < noseShape.size(); i++) {
        float x = ofMap(i, 0, noseShape.size(), ofGetWidth() * 0.6f, ofGetWidth() - 50);
        float y = ofGetHeight() * 0.3f - noseShape[i] * 20;
        ofVertex(x, y);
    }
    for (int i = noseShape.size() - 1; i >= 0; i--) {
        float x = ofMap(i, 0, noseShape.size(), ofGetWidth() * 0.6f, ofGetWidth() - 50);
        float y = ofGetHeight() * 0.3f + noseShape[i] * 20;
        ofVertex(x, y);
    }
    ofEndShape();
    
    // Draw tongue position indicator
    if (ofGetMousePressed()) {
        ofSetColor(255, 255, 0);
        ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 10);
    }
    
    // Draw parameters
    ofSetColor(255);
    ofDrawBitmapString("tongueDiameterDisplay: " + ofToString(tongueDiameterDisplay, 1), 20, ofGetHeight() - 100);
    ofDrawBitmapString("tongueIndexDisplay: " + ofToString(tongueIndexDisplay, 1), 20, ofGetHeight() - 80);
    ofDrawBitmapString("Frequency: " + ofToString(frequency, 1) + " Hz", 20, ofGetHeight() - 60);
    ofDrawBitmapString("Tenseness: " + ofToString(tenseness, 2), 20, ofGetHeight() - 40);
    ofDrawBitmapString("Playing: " + string(isPlaying ? "Yes" : "No"), 20, ofGetHeight() - 20);
    
    // Debug: Show tract shape values
    ofSetColor(200, 200, 200);
    string tractDebug = "Tract shape (first 10): ";
    for (int i = 0; i < min(10, (int)tractShape.size()); i++) {
        tractDebug += ofToString(tractShape[i], 1) + " ";
    }
    ofDrawBitmapString(tractDebug, 20, 120);
}

void ofApp::keyPressed(int key) {
    switch (key) {
        case 'a': case 'A':
            voice.setVowelA();
            isPlaying = true;
            break;
        case 'e': case 'E':
            voice.setVowelE();
            isPlaying = true;
            break;
        case 'i': case 'I':
            voice.setVowelI();
            isPlaying = true;
            break;
        case 'o': case 'O':
            voice.setVowelO();
            isPlaying = true;
            break;
        case 'u': case 'U':
            voice.setVowelU();
            isPlaying = true;
            break;
        case ' ':
            voice.setSilence();
            isPlaying = false;
            break;
        case OF_KEY_UP:
            frequency = ofClamp(frequency + 10, 80, 400);
            voice.setFrequency(frequency);
            isPlaying = true;
            break;
        case OF_KEY_DOWN:
            frequency = ofClamp(frequency - 10, 80, 400);
            voice.setFrequency(frequency);
            isPlaying = true;
            break;
        case OF_KEY_RIGHT:
            tenseness = ofClamp(tenseness + 0.1f, 0.0f, 1.0f);
            voice.setTenseness(tenseness);
            break;
        case OF_KEY_LEFT:
            tenseness = ofClamp(tenseness - 0.1f, 0.0f, 1.0f);
            voice.setTenseness(tenseness);
            break;
    }
}

void ofApp::audioOut(ofSoundBuffer& output) {
    if (isPlaying) {
        voice.synthesize(output.getBuffer().data(), output.getNumFrames());
        int numSamples = min((int)waveform.size(), (int)output.getNumFrames());
        for (int i = 0; i < numSamples; i++) {
            waveform[i] = output[i];
        }
    } else {
        output.set(0.0f);
        fill(waveform.begin(), waveform.end(), 0.0f);
    }
}

void ofApp::mouseMoved(int x, int y) {
    // Empty implementation is fine
}

void ofApp::mouseDragged(int x, int y, int button) {
    tongueX = (float)x / ofGetWidth();
    tongueY = (float)y / ofGetHeight();
    
    // Map to tongue parameters
    float tongueIndex = ofMap(tongueX, 0.0f, 1.0f, 10.0f, 30.0f);
    float tongueDiameter = ofMap(tongueY, 0.0f, 1.0f, 1.0f, 3.5f);
    
    tongueIndexDisplay = tongueIndex;
    tongueDiameterDisplay = tongueDiameter;
    
    cout << "Mouse dragged - Setting tongue: " << tongueIndex << ", " << tongueDiameter << endl;
    
    voice.setTonguePosition(tongueIndex, tongueDiameter);
    
    isPlaying = true; // Make sure sound is playing
}

void ofApp::mousePressed(int x, int y, int button) {
    tongueX = (float)x / ofGetWidth();
    tongueY = (float)y / ofGetHeight();
    
    // Map to tongue parameters
    float tongueIndex = ofMap(tongueX, 0.0f, 1.0f, 10.0f, 30.0f);
    float tongueDiameter = ofMap(tongueY, 0.0f, 1.0f, 1.0f, 3.5f);
    
    tongueIndexDisplay = tongueIndex;
    tongueDiameterDisplay = tongueDiameter;
    
    cout << "Mouse pressed - Setting tongue: " << tongueIndex << ", " << tongueDiameter << endl;
    
    voice.setTonguePosition(tongueIndex, tongueDiameter);
    
    isPlaying = true; // Make sure sound is playing
}
