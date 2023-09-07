#include "config.h"
#include "peaksort.h"
#include <stdlib.h>
#include <stdio.h>

int cmpfunc (const void * a, const void * b) {
    
   if( **(float**)a < **(float**)b ) {
       return 1;
   }
   else{
       return -1;
   }
}


float ** find_peak_pointers(float* spectrum, int * peak_size) {
    float ** peak_pointers = malloc(NH_SIZE * sizeof(int));
    int selection = 0;
    
    float curr, prev, next; // find a way to explicitely use registers instead of memory access
    prev = spectrum[1]; curr = spectrum[2]; next = spectrum[3];

    // hopefully compiler recognizes this as "use the damn registers" instead of memory accesses. 
    for (int i = 2; i < NH_SIZE - 1; i++) {
        // checks the previous and next values to see if it's a local maxima
        // Adds the index to the peak_indices
        
        if ((prev < curr) && (next < curr)) { 
            peak_pointers[selection] = &spectrum[i];
            selection++;
        }
        
        prev = curr; curr = next; next = spectrum[i + 2];
    }
    
    *peak_size = selection; // for returning capped size of array
    
    return peak_pointers;
}

float ** peaksort(float *spectrum, int * num_peaks) {
    float ** peak_pointer_arr;
    int peak_size = 0; 
    int num_pointers = 0;
    float threshold;
    
    peak_pointer_arr = find_peak_pointers(spectrum, &peak_size); // find pointers of peaks
    
    qsort(peak_pointer_arr, peak_size, sizeof(float*), cmpfunc); // quicksort
    
    threshold = *peak_pointer_arr[0] * EPS; // Thingy?
    
    for (int i = 0; (i < MAX_PEAKS) && (i < peak_size); i++) {
        if (*(peak_pointer_arr[i]) < threshold) {
            break;
        }
        num_pointers = i + 1;
    }
    
    *num_peaks = num_pointers;
    
    return peak_pointer_arr;
}

/*
 * Some ideas on how to optimize the code
 * Rather than doing quicksort, we can just find the max immediately
 * and then filter out all the values that don't reach threshold
 * then we quick sort.
 * 
 * */

/*
 * Issue: We have wasted space, but we don't know how many peaks there are
 * Issue2: Do we use indices or pointers? 
*/