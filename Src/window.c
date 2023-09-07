#include <stdlib.h>
#include "config.h"
#include "window.h"
#include "math.h"

void hanning_initialization(float w[])
{
    int half, i, idx;
    
    half = N_SIZE/2;
    for(i=0; i<half; i++) //CALC_HANNING   Calculates Hanning window samples.
        w[i] = 0.5 * (1 - cos(2*M_PI*(i+1) / (N_SIZE+1)));

    idx = half-1;
    for(i=half; i<N_SIZE; i++) {
        w[i] = w[idx];
        idx--;
    }
}

void window(float * arr, float *new_arr) {
    
    static int hanning_init = 0;
    static float hanning[N_SIZE]; 
		if (!hanning_init) {
            hanning_initialization(hanning); 
            hanning_init = 1; 
        }
    
    for(int i = 0; i < N_SIZE; i++) {
    	new_arr[i] = arr[i] * hanning[i];
    	writeFile("win.txt", new_arr[i], N_SIZE);

    }
}
