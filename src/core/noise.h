//
//  noise.hpp
//  PinkTrombone - VST3
//
//  Created by Samuel Tarakajian on 8/29/19.
//

#ifndef noise_h
#define noise_h

#include <stdio.h>
#include "config.h"

sample_t simplex1(sample_t xin);
sample_t simplex2(sample_t xin, sample_t yin);

#endif /* noise_h */
