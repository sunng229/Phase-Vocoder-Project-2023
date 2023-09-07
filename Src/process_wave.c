#include "process_wave.h"
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "window.h"
#include "process_spectrum.h"
#include "config.h"
#include "peaksort.h"
#include <arm_math.h>
#define ARM_MATH_CM7
#define __FPU PRESENT 1

float* process_wave(float * input, int * size_in, int * size_out) {
    
	float win[N_SIZE];
	float complex[ND_SIZE];
	float mag[NH_SIZE + 1];
	float phs[NH_SIZE + 1];
	int len_seg;
    
	float realPart, imagPart;
	
	float * offset_ptr;
	
    len_seg = floor((*size_in - N_SIZE) / HOP_IN) - 1;
    
	*size_out = ceil(HOP_OUT/HOP_IN * (*size_in) );
    float * output = calloc(sizeof(float), *size_out);
	
	//FFT_Setup
    arm_cfft_instance_f32 fft_handler;
	arm_cfft_init_f32(&fft_handler, N_SIZE);
    
	for (int s = 0; s < len_seg; s++) {
		if (s % 10 == 0) printf("%d\n", s);
        //printf("%d\n", s);
		window(&input[s * HOP_IN_I], win); // Windowing Code
        
        for (int i = 0; i < N_SIZE; i++) { // Turn into interleaved complex array
        	complex[i*2] = win[i];
			complex[(i*2) + 1] = 0;
			printf("%f\n",complex[i*2]);
			printf("%f\n",complex[(i*2) + 1]);
		}
        
        //fft
        arm_cfft_f32(&fft_handler, complex, 0, 1);

		// Get the magnitude
		for(int i = 0; i < NH_SIZE + 1; i++) mag[i] = sqrt(pow(complex[2*i],2) + pow(complex[2*i + 1],2));
		
		// Get the phase
		for(int i = 0; i < NH_SIZE + 1; i++) phs[i] = atan2(complex[2*i+1], complex[2*i]);
        
		process_spectrum(mag, phs); // Where the bulk of the processes happen

         for (int i = 0; i < NH_SIZE; i++) {
			realPart = cos(phs[i]) * mag[i];
			imagPart = sin(phs[i]) * mag[i];
			
			complex[2*i]	    = realPart;
			complex[2*i + 1]    = imagPart;
			
			complex[ND_SIZE - 2*i - 2] = realPart;
			complex[ND_SIZE - 2*i - 1] = -imagPart;
		}
        
        complex[N_SIZE] = mag[NH_SIZE] * cos(phs[NH_SIZE]);
		complex[N_SIZE + 1] = mag[NH_SIZE] * sin(phs[NH_SIZE]);
        
		//ifft
		arm_cfft_f32(&fft_handler, complex, 1, 1);
       
		//for (int i = 0; i < N_SIZE; i++) win[i] = complex[2*i] / N_SIZE; // every so other?
        
        offset_ptr = &output[s * HOP_OUT_I];
        
        for (int i = 0; i < N_SIZE; i++) offset_ptr[i] = offset_ptr[i] + win[i];
       
    }
    
    return output;
	
}
