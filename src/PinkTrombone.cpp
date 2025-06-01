//==============================================================================
// src/PinkTrombone.cpp
//==============================================================================

#include "PinkTrombone.h"

PinkTrombone::PinkTrombone(float sampleRate)
    : sampleRate(sampleRate)
    , blockTime(1.0f / sampleRate)
    , glottis(nullptr)
    , tract(nullptr)
    , whiteNoise(nullptr)
    , aspirateFilter(nullptr)
    , fricativeFilter(nullptr)
    , smoothingTime(0.1f)
    , targetFrequency(140.0f), currentFrequency(140.0f)
    , targetTenseness(0.6f), currentTenseness(0.6f)
    , targetTongueIndex(16.9f), currentTongueIndex(16.9f)
    , targetTongueDiameter(4.43f), currentTongueDiameter(4.43f)
    , targetConstrictionIndex(-1.0f), currentConstrictionIndex(-1.0f)
    , targetConstrictionDiameter(1.0f), currentConstrictionDiameter(1.0f)
    , targetFricative(0.0f), currentFricative(0.0f) {
    
    // Initialize tract properties
    initializeTractProps(&tractProps, 44);
    
    // Create synthesis components
    glottis = new Glottis(sampleRate);
    tract = new Tract(sampleRate, blockTime, &tractProps);
    whiteNoise = new WhiteNoise(1024);
    aspirateFilter = new Biquad(sampleRate);
    fricativeFilter = new Biquad(sampleRate);
    
    // Setup filters
    aspirateFilter->setFrequency(500.0f);
    aspirateFilter->setQ(0.5f);
    fricativeFilter->setFrequency(1000.0f);
    fricativeFilter->setQ(0.5f);
    
    // Set initial vowel (A)
    tract->setRestDiameter(targetTongueIndex, targetTongueDiameter);
        
    cout << "Tract length: " << tractProps.n << endl;
    cout << "Tongue bounds: " << tract->tongueIndexLowerBound() << " to " << tract->tongueIndexUpperBound() << endl;
    cout << "Blade start: " << tractProps.bladeStart << ", Tip start: " << tractProps.tipStart << endl;

}

PinkTrombone::~PinkTrombone() {
    delete glottis;
    delete tract;
    delete whiteNoise;
    delete aspirateFilter;
    delete fricativeFilter;
    
    if (tractProps.tractDiameter) free(tractProps.tractDiameter);
    if (tractProps.noseDiameter) free(tractProps.noseDiameter);
}

void PinkTrombone::synthesize(float* output, int bufferSize) {
    for (int i = 0; i < bufferSize; i++) {
        updateParameters();
        
        // Generate noise sources
        float noiseSource = whiteNoise->runStep();
        float turbulenceNoise = fricativeFilter->runStep(noiseSource);
        
        // Generate glottal source
        float lambda = (float)i / (float)bufferSize;
        float glottalOutput = glottis->runStep(lambda, noiseSource);
        float glottalNoiseModulator = glottis->getNoiseModulator();
        
        // Process through vocal tract
        tract->runStep(glottalOutput, turbulenceNoise, lambda, glottalNoiseModulator);
        
        // Mix outputs
        output[i] = tract->lipOutput + 0.8f * tract->noseOutput;
        
        // Soft limiting
        output[i] = ofClamp(output[i], -1.0f, 1.0f);
    }
    
    // Finish processing blocks
    glottis->finishBlock();
    tract->finishBlock();
}

void PinkTrombone::updateParameters() {
    float deltaTime = blockTime;
    
    // Smooth all parameters
    currentFrequency = smoothParameter(currentFrequency, targetFrequency, deltaTime);
    currentTenseness = smoothParameter(currentTenseness, targetTenseness, deltaTime);
    currentTongueIndex = smoothParameter(currentTongueIndex, targetTongueIndex, deltaTime);
    currentTongueDiameter = smoothParameter(currentTongueDiameter, targetTongueDiameter, deltaTime);
    currentConstrictionIndex = smoothParameter(currentConstrictionIndex, targetConstrictionIndex, deltaTime);
    currentConstrictionDiameter = smoothParameter(currentConstrictionDiameter, targetConstrictionDiameter, deltaTime);
    currentFricative = smoothParameter(currentFricative, targetFricative, deltaTime);
    
    // DEBUG: Print current values
    static int debugCounter = 0;
    if (debugCounter++ % 4410 == 0) {  // Print every 0.1 seconds at 44.1kHz
        cout << "Tongue: " << currentTongueIndex << ", " << currentTongueDiameter << endl;
    }
    
    // Apply to synthesis components
    glottis->setTargetFrequency(currentFrequency);
    glottis->setTargetTenseness(currentTenseness);
    tract->setRestDiameter(currentTongueIndex, currentTongueDiameter);
    tract->setConstriction(currentConstrictionIndex, currentConstrictionDiameter, currentFricative);
}

float PinkTrombone::smoothParameter(float current, float target, float deltaTime) {
    if (smoothingTime <= 0.0f) return target;
    
    float factor = 1.0f - expf(-deltaTime / smoothingTime);
    return current + factor * (target - current);
}

void PinkTrombone::setFrequency(float frequency) {
    targetFrequency = ofClamp(frequency, 50.0f, 800.0f);
}

void PinkTrombone::setTenseness(float tenseness) {
    targetTenseness = ofClamp(tenseness, 0.0f, 1.0f);
}

void PinkTrombone::setTonguePosition(float index, float diameter) {
    targetTongueIndex = currentTongueIndex = index;     // Set both target AND current
    targetTongueDiameter = currentTongueDiameter = diameter;  // Skip smoothing
    
    // Immediately update the tract shape for visual feedback
    if (tract) {
        tract->setRestDiameter(currentTongueIndex, currentTongueDiameter);
    }
}

void PinkTrombone::setConstriction(float index, float diameter, float fricative) {
    targetConstrictionIndex = index;
    targetConstrictionDiameter = diameter;
    targetFricative = ofClamp(fricative, 0.0f, 1.0f);
}

void PinkTrombone::setVibrato(float amount, float frequency) {
    if (glottis) {
        glottis->vibratoAmount = ofClamp(amount, 0.0f, 0.1f);
        glottis->vibratoFrequency = ofClamp(frequency, 1.0f, 15.0f);
    }
}

void PinkTrombone::setParameterSmoothingTime(float seconds) {
    smoothingTime = ofClamp(seconds, 0.0f, 2.0f);
}

float* PinkTrombone::getTractDiameters() {
    return tractProps.tractDiameter;
}

float* PinkTrombone::getNoseDiameters() {
    return tractProps.noseDiameter;
}

int PinkTrombone::getTractLength() {
    return tractProps.n;
}

int PinkTrombone::getNoseLength() {
    return tractProps.noseLength;
}
