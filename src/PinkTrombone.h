//==============================================================================
// src/PinkTrombone.h - Main synthesis engine
//==============================================================================

#pragma once

#include "ofMain.h"
#include "core/Glottis.h"
#include "core/Tract.h"
#include "core/WhiteNoise.h"
#include "core/Biquad.h"

class PinkTrombone {
public:
    PinkTrombone(float sampleRate);
    ~PinkTrombone();
    
    void synthesize(float* output, int bufferSize);
    
    // Parameter setters
    void setFrequency(float frequency);
    void setTenseness(float tenseness);
    void setTonguePosition(float index, float diameter);
    void setConstriction(float index, float diameter, float fricative);
    void setVibrato(float amount, float frequency);
    void setParameterSmoothingTime(float seconds);
    
    // Data access for visualization
    float* getTractDiameters();
    float* getNoseDiameters();
    int getTractLength();
    int getNoseLength();
    
    // Add this method to access the tract directly
    Tract* getTract() { return tract; }
    
private:
    void processBlock(float* output, int bufferSize);
    
    float sampleRate;
    float blockTime;
    
    Glottis* glottis;
    Tract* tract;
    WhiteNoise* whiteNoise;
    Biquad* aspirateFilter;
    Biquad* fricativeFilter;
    
    t_tractProps tractProps;
    
    // Parameter smoothing
    float smoothingTime;
    float targetFrequency, currentFrequency;
    float targetTenseness, currentTenseness;
    float targetTongueIndex, currentTongueIndex;
    float targetTongueDiameter, currentTongueDiameter;
    float targetConstrictionIndex, currentConstrictionIndex;
    float targetConstrictionDiameter, currentConstrictionDiameter;
    float targetFricative, currentFricative;
    
    void updateParameters();
    float smoothParameter(float current, float target, float deltaTime);
};
