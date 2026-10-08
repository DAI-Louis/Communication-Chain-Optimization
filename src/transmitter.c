#include "transmitter.h"
#include <string.h>
#include <arm_neon.h>

/*Partie Encodage*/

// write into the buffer U_K
void source_generate(uint8_t *U_K, size_t K){
    for(int i = 0; i < K; i++)
        //Valeur aléatoire entre 0 et 1
        U_K[i] = rand() % 2;
}

// write only zeros in U_K
void source_generate_all_zeros(uint8_t *U_K, size_t K){
    memset(U_K, 0, K * sizeof(uint8_t));
}

// read from the buffer U_K and write into the buffer C_N
void codec_repetition_encode(const uint8_t *U_K, uint8_t *C_N, size_t K, size_t n_reps){
    for(int i = 0; i < n_reps*K; i++)
        C_N[i] = U_K[i%K];
}

// read from C_N, write into X_N
void modem_BPSK_modulate(const uint8_t *C_N, int32_t *X_N, size_t N){
    for(int i = 0; i < N; i++)
        //1 - 0 => 0
        //1 - 2 => -1
        X_N[i] = 1 - 2 * C_N[i];
}

// write only ones in X_N
void modem_BPSK_modulate_all_ones(const uint8_t *C_N, int32_t *X_N, size_t N){

    //éviter des warnings
    (void)C_N;

    for(int i = 0; i < N; i++){
        X_N[i] = 1;
    }
}

/*========================== Partie Mini-Projet ==========================*/


/*----------------- NEON -----------------*/

// read from C_N, write into X_N version Neon
// Assume K is a multiple of 16
void modem_BPSK_modulate_neon(const uint8_t *C_N, int32_t *X_N, size_t N){
    int32x4_t one_32 = vdupq_n_s32(1);
    int32x4_t zero_32 = vdupq_n_s32(0);

    for(int i = 0; i < N; i+=4){
        // int32_t temp[4] = { C_N[i], C_N[i+1], C_N[i+2], C_N[i+3] };
        // int32x4_t res = vld1q_s32(temp);

        // On injecte les bits directement dans le processeur, case par case
        int32x4_t res = zero_32;
        res = vsetq_lane_s32(C_N[i],   res, 0);
        res = vsetq_lane_s32(C_N[i+1], res, 1);
        res = vsetq_lane_s32(C_N[i+2], res, 2);
        res = vsetq_lane_s32(C_N[i+3], res, 3);
        //1 - 0 => 0
        //1 - 2 => -1
        res = vqaddq_s32(res, res);
        res = vqsubq_s32(one_32, res);
        vst1q_s32(&X_N[i], res);
    }
}

/*----------------- End NEON        -----------------*/



/*----------------- Bit-Packing     -----------------*/

// write into the buffer U_K
void source_generate_packed(uint8_t *U_K_packed, size_t K){
    for(int i = 0; i < K / 8; i++)
        //Valeur aléatoire entre 0 et 1
        U_K_packed[i] = rand() % 0xFF;
}

// read from the buffer U_K and write into the buffer C_N
void codec_repetition_encode_packed(const uint8_t *U_K_packed, uint8_t *C_N_packed, size_t K, size_t n_reps){
    size_t K_bytes = K / 8;

    for(int i = 0; i < n_reps*( K / 8 ); i++)
        C_N_packed[i] = U_K_packed[i % K_bytes];
}

// read from C_N, write into X_N
void modem_BPSK_modulate_packed(const uint8_t *C_N, int32_t *X_N, size_t N){
    for(int i = 0; i < N / 8; i++){
        //byte_data with the 8 bit of information
        uint8_t byte_d = C_N[i];
        for(int j = 0; j < 8; j++){
            uint8_t val = (byte_d >> j) & 1;
            int ind = (i * 8) + j;
            //1 - 0 => 0
            //1 - 2 => -1
            X_N[ind] = 1 - 2 * val;
        }
    }
}

/*----------------- End Bit-Packing -----------------*/

/*-----------------     Thread     ----------------- */

// write into the buffer U_K
void source_generate_thread(uint8_t *U_K, size_t K, unsigned int *seed){
    for(int i = 0; i < K; i++)
        //Valeur aléatoire entre 0 et 1
        U_K[i] = rand_r(seed) % 2;
}

// write into the buffer U_K
void source_generate_packed_thread(uint8_t *U_K_packed, size_t K, unsigned int *seed){
    for(int i = 0; i < K / 8; i++)
        //Valeur aléatoire entre 0 et 1
        U_K_packed[i] = rand_r(seed) % 0xFF;
}

/*-----------------   End Thread  ----------------- */