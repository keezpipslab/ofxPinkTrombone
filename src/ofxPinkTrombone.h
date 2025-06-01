//==============================================================================
// src/ofxPinkTrombone.h
//==============================================================================

#pragma once

#include "ofMain.h"
#include "ofSoundStream.h"

// Forward declarations
class PinkTrombone;

class ofxPinkTrombone {
public:
    ofxPinkTrombone();
    ~ofxPinkTrombone();
    
    // Setup and configuration
    void setup(int sampleRate = 44100, int bufferSize = 512);
    void close();
    
    // Main synthesis method
    void synthesize(float* output, int bufferSize);
    
    // Voice parameters
    void setFrequency(float frequency);           // Fundamental frequency (Hz)
    void setTenseness(float tenseness);           // Vocal cord tension (0-1)
    void setTonguePosition(float index, float diameter); // Tongue shape
    void setConstriction(float index, float diameter, float fricative = 0.0f);
    void setVibrato(float amount, float frequency);
    
    // Getters for UI/visualization
    float* getTractDiameters();
    float* getNoseDiameters();
    int getTractLength();
    int getNoseLength();
    
    // Presets for common sounds
    void setVowelA();
    void setVowelE();
    void setVowelI();
    void setVowelO();
    void setVowelU();
    void setSilence();
    
    // Real-time parameter smoothing
    void setParameterSmoothingTime(float seconds);
    
private:
    PinkTrombone* pinkTrombone;
    int sampleRate;
    int bufferSize;
    bool isSetup;
};
