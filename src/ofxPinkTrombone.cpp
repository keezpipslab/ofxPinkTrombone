#include "ofxPinkTrombone.h"
#include "PinkTrombone.h"

ofxPinkTrombone::ofxPinkTrombone()
    : pinkTrombone(nullptr)
    , sampleRate(44100)
    , bufferSize(512)
    , isSetup(false) {
}

ofxPinkTrombone::~ofxPinkTrombone() {
    close();
}

void ofxPinkTrombone::setup(int sampleRate, int bufferSize) {
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
    
    if (pinkTrombone) {
        delete pinkTrombone;
    }
    
    pinkTrombone = new PinkTrombone(sampleRate);
    isSetup = true;
}

void ofxPinkTrombone::close() {
    if (pinkTrombone) {
        delete pinkTrombone;
        pinkTrombone = nullptr;
    }
    isSetup = false;
}

void ofxPinkTrombone::synthesize(float* output, int bufferSize) {
    if (!isSetup || !pinkTrombone) {
        // Fill with silence if not setup
        memset(output, 0, bufferSize * sizeof(float));
        return;
    }
    
    pinkTrombone->synthesize(output, bufferSize);
}

void ofxPinkTrombone::setFrequency(float frequency) {
    if (pinkTrombone) {
        pinkTrombone->setFrequency(frequency);
    }
}

void ofxPinkTrombone::setTenseness(float tenseness) {
    if (pinkTrombone) {
        pinkTrombone->setTenseness(ofClamp(tenseness, 0.0f, 1.0f));
    }
}
/*
void ofxPinkTrombone::setTonguePosition(float index, float diameter) {
    if (pinkTrombone) {
        pinkTrombone->setTonguePosition(index, diameter);
    }
}
*/
void ofxPinkTrombone::setTonguePosition(float index, float diameter) {
    if (pinkTrombone && pinkTrombone->getTract()) {
        Tract* tract = pinkTrombone->getTract();
        float clampedIndex = ofClamp(index, (float)tract->tongueIndexLowerBound(),
                                           (float)tract->tongueIndexUpperBound());
        float clampedDiameter = ofClamp(diameter, 1.0f, 3.5f);
        
        cout << "Setting tongue: " << clampedIndex << ", " << clampedDiameter << endl;
        pinkTrombone->setTonguePosition(clampedIndex, clampedDiameter);
    }
}

void ofxPinkTrombone::setConstriction(float index, float diameter, float fricative) {
    if (pinkTrombone) {
        pinkTrombone->setConstriction(index, diameter, fricative);
    }
}

void ofxPinkTrombone::setVibrato(float amount, float frequency) {
    if (pinkTrombone) {
        pinkTrombone->setVibrato(amount, frequency);
    }
}

float* ofxPinkTrombone::getTractDiameters() {
    return pinkTrombone ? pinkTrombone->getTractDiameters() : nullptr;
}

float* ofxPinkTrombone::getNoseDiameters() {
    return pinkTrombone ? pinkTrombone->getNoseDiameters() : nullptr;
}

int ofxPinkTrombone::getTractLength() {
    return pinkTrombone ? pinkTrombone->getTractLength() : 0;
}

int ofxPinkTrombone::getNoseLength() {
    return pinkTrombone ? pinkTrombone->getNoseLength() : 0;
}

// Vowel presets
void ofxPinkTrombone::setVowelA() {
    setTonguePosition(12.9f, 2.43f);
    setConstriction(-1, 1.0f, 0.0f);
}

void ofxPinkTrombone::setVowelE() {
    setTonguePosition(21.0f, 2.36f);
    setConstriction(-1, 1.0f, 0.0f);
}

void ofxPinkTrombone::setVowelI() {
    setTonguePosition(25.0f, 2.05f);
    setConstriction(-1, 1.0f, 0.0f);
}

void ofxPinkTrombone::setVowelO() {
    setTonguePosition(14.0f, 2.7f);
    setConstriction(-1, 1.0f, 0.0f);
}

void ofxPinkTrombone::setVowelU() {
    setTonguePosition(16.0f, 2.8f);
    setConstriction(-1, 1.0f, 0.0f);
}

void ofxPinkTrombone::setSilence() {
    setTenseness(0.0f);
    setConstriction(15.0f, 0.0f, 0.0f);
}

void ofxPinkTrombone::setParameterSmoothingTime(float seconds) {
    if (pinkTrombone) {
        pinkTrombone->setParameterSmoothingTime(seconds);
    }
}
