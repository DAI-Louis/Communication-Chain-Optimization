#include "receiver.h"
#include <stdio.h>
#include <math.h> 
#include <arm_neon.h>

#define PI 3.14159265358979323846

/*Partie Bruit*/

//Box Muller White Gaussian Noise
float box_muller(float sigma){
    float u1 = 0.0f;
    float u2 = 0.0f;

    while (u1 == 0.0f) {
        u1 = (float)rand() / (float)RAND_MAX;
    }

    u2 = (float)rand() / (float)RAND_MAX;

    float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * (float)PI * u2);

    return z0 * sigma;
}

// add white Gaussian noise
void channel_AWGN_add_noise(const int32_t *X_N, float *Y_N, size_t N, float sigma){
    for(int i = 0; i < N; i++){
        Y_N[i] = X_N[i] + box_muller(sigma);
    }
}

/*Partie Décodage*/

// demodulator, just copies Y_N in L_N for now
void modem_BPSK_demodulate(const float *Y_N, float *L_N, size_t N, float sigma){

    float facteur = 2.0f / (sigma * sigma);

    for(int i = 0; i < N; i++){
        L_N[i] = facteur*Y_N[i];
    }
}

// transform numbers from floating-point representation to fixed-point representation
// `s` is the number of bits used in the quantizer block
// `f` is the number of bits of the fractional part (`s` >= `f`)
void quantizer_transform8(const float *L_N, int8_t *L8_N, size_t N, size_t s, size_t f) {

    int32_t max_val = (1 << (s - 1)) - 1;  
    int32_t min_val = -(1 << (s - 1));     

    float scale = (float)(1 << f);

    for (size_t i = 0; i < N; i++) {
        float scaled_val = roundf(L_N[i] * scale);

        int32_t int_val = (int32_t)scaled_val;

        if (int_val > max_val) {
            int_val = max_val;
        } else if (int_val < min_val) {
            int_val = min_val;
        }

        L8_N[i] = (int8_t)int_val;
    }
}

// hard decoder: first hard decides each LLR and then makes a majority vote
void codec_repetition_hard_decode(const float *L_N, uint8_t *V_K, size_t K, size_t n_reps){
    uint16_t L_hard[K*n_reps];
    for(int i = 0; i < K*n_reps; i++){
        if(L_N[i] < 0) L_hard[i] = 1;
        else           L_hard[i] = 0;
    }

    int16_t vote[K];
    memset(vote, 0, K * sizeof(uint16_t));
    for(int i = 0; i < K*n_reps; i++){
        vote[i%K] = vote[i%K] + (1 - 2*L_hard[i]);
    }

    for(int i = 0; i < K; i++){
        if(vote[i] < 0) V_K[i] = 1;
        else            V_K[i] = 0;
    }
}

// hard decoder: first hard decides each LLR and then makes a majority vote
void codec_repetition_hard_decode8(const int8_t *L8_N, uint8_t *V_K, size_t K, size_t n_reps){
    uint16_t L_hard[K*n_reps];
    for(int i = 0; i < K*n_reps; i++){
        if(L8_N[i] < 0) L_hard[i] = 1;
        else            L_hard[i] = 0;
    }

    int16_t vote[K];
    memset(vote, 0, K * sizeof(uint16_t));
    for(int i = 0; i < K*n_reps; i++){
        vote[i%K] = vote[i%K] + (1 - 2*L_hard[i]);
    }

    for(int i = 0; i < K; i++){
        if(vote[i] < 0) V_K[i] = 1;
        else            V_K[i] = 0;
    }
}

// hard decoder: first hard decides each LLR and then makes a majority vote
void codec_repetition_hard_decode8_neon(const int8_t *L8_N, uint8_t *V_K, size_t K, size_t n_reps){
    int8x16_t L_hard;
    int8x16_t vote;
    //Utilisé pour l'addition
    int8x16_t mask;
    int8x16_t one_16 = vdupq_n_s8(1);
    
    for(size_t i = 0; i < K; i += 16){
        int8x16_t res = vdupq_n_s8(0);
        for(size_t j = 0; j < n_reps; j++){

            int ind = j * K + i;
            //Chargement des valeurs de L8_N dans un registre NEON
            L_hard = vld1q_s8(&L8_N[ind]);
            //Comparaison inférieur à 0
            L_hard = (int8x16_t)vcltzq_s8(L_hard);
            //On fait x + x (ce qui multiplie le masque par 2)
            // Les 0 restent 0. Les -1 (0xFF) deviennent -2 (0xFE)
            mask = vaddq_s8(L_hard, L_hard);
            // 0 + 1 = 1    
            // -2 + 1 = -1
            vote = vqaddq_s8(one_16, mask);

            res = vqaddq_s8(res, vote);
        }
        res = (int8x16_t)vcltzq_s8(res);
        res = vandq_s8(res, one_16);

        //Cast pour éviter le Warning
        vst1q_s8((int8_t*)&V_K[i], res);
    }
}

// soft decoder: computes the mean of each LLR to hard decide the bits
void codec_repetition_soft_decode(const float *L_N, uint8_t *V_K, size_t K, size_t n_reps){
    float L_avg[K];
    memset(L_avg, 0, K * sizeof(float));
    for(int i = 0; i < K*n_reps; i++){
        L_avg[i%K] += L_N[i];
    }

    for(int i = 0; i < K; i++){
        if(L_avg[i] < 0) V_K[i] = 1;
        else             V_K[i] = 0;
    }
}

// soft decoder: computes the mean of each LLR to hard decide the bits
void codec_repetition_soft_decode8(const int8_t *L8_N, uint8_t *V_K, size_t K, size_t n_reps){
    int32_t L_avg[K];
    memset(L_avg, 0, K * sizeof(int32_t));
    for(size_t i = 0; i < K*n_reps; i++){
        L_avg[i%K] += L8_N[i];
    }

    for(size_t i = 0; i < K; i++){
        if(L_avg[i] < 0) V_K[i] = 1;
        else             V_K[i] = 0;
    }
}

// soft decoder: computes the mean of each LLR to hard decide the bits
void codec_repetition_soft_decode8_neon(const int8_t *L8_N, uint8_t *V_K, size_t K, size_t n_reps){
    int8x16_t vote;
    int8x16_t one_16 = vdupq_n_s8(1);
    
    for(size_t i = 0; i < K; i += 16){
        int8x16_t res = vdupq_n_s8(0);
        for(size_t j = 0; j < n_reps; j++){
            int ind = j * K + i;
            //Chargement des valeurs de L8_N dans un registre NEON
            vote = vld1q_s8(&L8_N[ind]);
            res = vqaddq_s8(res, vote);
        }
        res = (int8x16_t)vcltzq_s8(res);
        res = vandq_s8(res, one_16);

        //Cast pour éviter le Warning
        vst1q_s8((int8_t*)&V_K[i], res);
    }
}

// update `n_bit_errors` and `n_frame_errors` variables depending on `U_K` and `V_K`
void monitor_check_errors(const uint8_t *U_K, const uint8_t *V_K, size_t K, uint64_t *n_bit_errors, uint64_t *n_frame_errors){
    int frame_err = 0;
    for(int i = 0; i < K; i++){
        if(U_K[i] != V_K[i]){
            *n_bit_errors+=1;
            if(!frame_err){
                frame_err = 1;
                *n_frame_errors+=1;
            }
        }
    }
}


/*========================== Partie Mini-Projet ==========================*/

/*----------------- NEON -----------------*/

// demodulator, just copies Y_N in L_N for now
void modem_BPSK_demodulate8_neon(const float *Y_N, float *L_N, size_t N, float sigma){

    float32x4_t facteur = vdupq_n_f32(2.0f / (sigma * sigma));
    for(int i = 0; i < N; i+=4){
        float32x4_t res = vld1q_f32(&Y_N[i]);
        res = vmulq_f32(facteur, res);
        vst1q_f32(&L_N[i], res);
    }
}

// update `n_bit_errors` and `n_frame_errors` variables depending on `U_K` and `V_K`
void monitor_check_errors_neon(const uint8_t *U_K, const uint8_t *V_K, size_t K, uint64_t *n_bit_errors, uint64_t *n_frame_errors){
    int frame_err = 0;
    for(int i = 0; i < K; i+=16){
        //Chargement 
        uint8x16_t u_vec = vld1q_u8(&U_K[i]);
        uint8x16_t v_vec = vld1q_u8(&V_K[i]);

        uint8x16_t cmp_eq = vceqq_u8(u_vec, v_vec);
        // Inverser (NOT) : Maintenant on a 0xFF là où il y a une ERREUR
        uint8x16_t diff = vmvnq_u8(cmp_eq);
        
        // On transforme les 0xFF (255) en 1 pour le comptage
        uint8x16_t one_16 = vdupq_n_u8(1);
        diff = vandq_u8(diff, one_16);

        uint16_t err_cette_boucle = vaddlvq_u8(diff);

        if (err_cette_boucle > 0) {
            *n_bit_errors += (uint64_t)err_cette_boucle;
            if(n_bit_errors && !frame_err){
                frame_err = 1;
                *n_frame_errors+=1;
            }
        }
    }
}

void quantizer_transform8_neon(const float *L_N, int8_t *L8_N, size_t N, size_t s, size_t f) {
    
    // 1. Préparation des limites et de l'échelle (en C standard)
    int32_t max_val_scalar = (1 << (s - 1)) - 1;
    int32_t min_val_scalar = -(1 << (s - 1));
    float scale_scalar = (float)(1 << f);

    // 2. Chargement de ces valeurs dans des vecteurs NEON qui se répètent
    float32x4_t v_scale = vdupq_n_f32(scale_scalar);
    int32x4_t v_max = vdupq_n_s32(max_val_scalar);
    int32x4_t v_min = vdupq_n_s32(min_val_scalar);

    // 3. Boucle vectorisée (on avance de 4 en 4 car on utilise float32x4_t)
    for (size_t i = 0; i < N; i += 4) {
        // A. Chargement de 4 LLRs flottants
        float32x4_t v_in = vld1q_f32(&L_N[i]);

        // B. Multiplication par l'échelle (scale)
        float32x4_t v_scaled = vmulq_f32(v_in, v_scale);

        // C. Arrondi à l'entier le plus proche et conversion en int32
        // vcvtaq_s32_f32 = Convert Float to Int with Round to Nearest (Away)
        int32x4_t v_int = vcvtaq_s32_f32(v_scaled);

        // D. La Saturation SIMD (Ultra rapide, remplace les "if/else")
        v_int = vminq_s32(v_int, v_max); // Coupe tout ce qui dépasse le max
        v_int = vmaxq_s32(v_int, v_min); // Coupe tout ce qui descend sous le min

        // E. Extraction des 4 entiers saturés pour les stocker dans le tableau 8-bits
        int32_t temp[4];
        vst1q_s32(temp, v_int);
        
        L8_N[i]   = (int8_t)temp[0];
        L8_N[i+1] = (int8_t)temp[1];
        L8_N[i+2] = (int8_t)temp[2];
        L8_N[i+3] = (int8_t)temp[3];
    }
}

/*----------------- End NEON        -----------------*/


/*----------------- Bit-Packing     -----------------*/

// Prend V_K (déballé) et génère V_K_packed
void bit_pack_array(const uint8_t *unpacked_array, uint8_t *packed_array, size_t K) {
    
    // On boucle sur le nombre final d'octets (K / 8)
    for(size_t i = 0; i < K / 8; i++) {
        uint8_t byte = 0; // On prépare un octet vide (00000000)
        
        // On va lire les 8 cases déballées correspondantes
        for(int j = 0; j < 8; j++) {
            
            // On récupère le bit (0 ou 1)
            uint8_t bit_val = unpacked_array[i * 8 + j];
            
            // On le décale à sa bonne place et on l'insère dans l'octet
            byte |= (bit_val << j);
        }
        
        // On range l'octet plein dans le nouveau tableau
        packed_array[i] = byte;
    }
}

// update `n_bit_errors` and `n_frame_errors` variables depending on `U_K` and `V_K`
void monitor_check_errors_packed(const uint8_t *U_K_packed, const uint8_t *V_K_packed, size_t K, uint64_t *n_bit_errors, uint64_t *n_frame_errors){
    int frame_err = 0;
    
    // On boucle sur les octets (K / 8)
    for(size_t i = 0; i < K / 8; i++){
        
        // Le XOR : met un '1' exactement là où les bits sont différents
        uint8_t diff = U_K_packed[i] ^ V_K_packed[i];
        
        // popcount compte le nombre de bits à '1' dans l'octet
        // optimisation, éviter les boucles
        int err_count = __builtin_popcount(diff);
        
        if (err_count > 0) {
            *n_bit_errors += err_count;
            frame_err = 1;
        }
    }
    if (frame_err) *n_frame_errors += 1;
}

/*----------------- End Bit-Packing -----------------*/

/*-----------------     Thread     ----------------- */

//
void channel_AWGN_add_noise_thread(int32_t *X_N, float *Y_N, int N, float sigma, unsigned int *seed){
    for(int i = 0; i < N; i++){
        Y_N[i] = X_N[i] + box_muller_thread(sigma, seed);
    }
}

//Box Muller White Gaussian Noise
float box_muller_thread(float sigma, unsigned int *seed){
    float u1 = 0.0f;
    float u2 = 0.0f;

    while (u1 == 0.0f) {
        u1 = (float)rand_r(seed) / (float)RAND_MAX;
    }

    u2 = (float)rand_r(seed) / (float)RAND_MAX;

    float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * (float)PI * u2);

    return z0 * sigma;
}

/*-----------------   End Thread  ----------------- */