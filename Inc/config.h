#ifndef CONFIG_H
#define CONFIG_H

#define N_SIZE 4096
#define ND_SIZE N_SIZE*2
#define NH_SIZE N_SIZE/2
#define MAX_PEAKS 50
#define EPS 0.05

#define HOP_IN 121.0
#define HOP_OUT 242.0
#define HOP_IN_I 121
#define HOP_OUT_I 242
#define SAMPLING_RATE 12000.0 //44100

#define DT_IN HOP_IN/SAMPLING_RATE
#define DT_OUT HOP_OUT/SAMPLING_RATE
#define D_F N_SIZE/SAMPLING_RATE

#endif
