#include "process_spectrum.h"
#include "peaksort.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void QUACK1(int quack) {
    printf("QUACK: %d\n", quack);
    quack++;
}

void initialize_frequencies (float * output) {
    // the delta f is
    float df = SAMPLING_RATE / N_SIZE;
    for (int i = 0; i < NH_SIZE + 1; i++) output[i] = i * df; 
}

float ** offset (float * initial, float * end, float ** pointers, int num_ptr) {
    float ** offset_pointers;
    long long arr_offset = end - initial;
    offset_pointers = malloc(sizeof(float*) * num_ptr);
    for (int i = 0; i < num_ptr; i++) offset_pointers[i] = pointers[i] + arr_offset;
    
    return offset_pointers;
}

void process_spectrum(float * mag, float * phs) {
    // assumes that outside of it, magnitude and phase have been
    // computed already
    
    static float frequencies[NH_SIZE + 1]; static int f_init = 0; if (!f_init) { initialize_frequencies(frequencies); f_init = 1; }
    static float ph_hld[NH_SIZE + 1] = {0};
    static float ph_adv[NH_SIZE + 1] = {0};
    
    int num_peaks;
    float * best_frequency;
    float ** peak_pointers, **phs_pointers, **hld_pointers, **adv_pointers, **frq_pointers;
    float t; // temporary variable that is used to hold values... a lot
 
    peak_pointers = peaksort(mag, &num_peaks); // mag pointers (NOT PHASE POINTERS)
    
    // offset to point to corresponding arr. Works since they're pointing to same arrays
    // Worst case is this train takes (50 + 20) * 4 cycles
    phs_pointers = offset(mag, phs, peak_pointers, num_peaks); 
    hld_pointers = offset(mag, ph_hld, peak_pointers, num_peaks);
    adv_pointers = offset(mag, ph_adv, peak_pointers, num_peaks);
    frq_pointers = offset(mag, frequencies, peak_pointers, num_peaks);
    
    best_frequency = malloc(sizeof(float)*num_peaks);
    // We want to reduce the number of divisions we do. Takes 18 fricken cycles as opposed to 2~3 for multiplication
    for (int pk = 0; pk < num_peaks; pk++) {
        
        t = DT_IN * (*frq_pointers[pk]) + (*hld_pointers[pk] - *phs_pointers[pk]) / (2*M_PI);
        t = round(t);
        t = (*phs_pointers[pk] - *hld_pointers[pk]) + 2 * M_PI * t; 
        t = t / (2 * M_PI * DT_IN);
        best_frequency[pk] = t;
    }
    
    for (int i = 0; i < NH_SIZE + 1; i++) {
        ph_hld[i] = phs[i]; // store current spectrum phase for next
    }
    
	for (int pk = 0; pk < num_peaks; pk++) {
        t = best_frequency[pk];
        (*adv_pointers[pk]) = fmod(((*adv_pointers[pk]) + 2 * M_PI * t * DT_OUT), (2 * M_PI));
        (*phs_pointers[pk]) = (*adv_pointers[pk]);
    }

	free(best_frequency);
    free(peak_pointers);
    free(phs_pointers);
    free(hld_pointers);
    free(adv_pointers);
    free(frq_pointers);
	
}
