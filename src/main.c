#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "transmitter.h"
#include "receiver.h"


#define PI 3.14159265358979323846


int main(int argc, char** argv){
    srand(time(NULL));
    size_t K, N, n_reps;

    /*Partie arguments*/
    if (argc < 3){
        K = 4;
        n_reps = 3;
    }
    else{
        K = atoi(argv[1]);
        n_reps = atoi(argv[2]);
    }
    N = K*n_reps;

    /*Partie Test*/
    uint8_t U_K[K], C_N[N];
    int32_t X_N[N];

    //Test sur U_K
    source_generate(U_K, K);
    printf("U_K = [ ");
    for(int i = 0; i < K; i++) printf("%d ", U_K[i]);
    printf("]\n");

    //Test sur C_N
    // size_t n_reps = N/K;

    codec_repetition_encode(U_K, C_N, K, n_reps);
    printf("C_N = [ ");
    for(int i = 0; i < N; i++) printf("%d ", C_N[i]);
    // printf("]\ncoderate = %0.2f\n", (double)K/N);

    //Test sur X_N
    modem_BPSK_modulate(C_N, X_N, N);
    printf("X_N = [ ");
    for(int i = 0; i < N; i++) printf("%d ", X_N[i]);
    printf("]\n");

    //Test sur AWGN 
    float Y_N[N];
    float sigma = 0.9;
    channel_AWGN_add_noise(X_N, Y_N, N, sigma);
    printf("Y_N = [ ");
    for(int i = 0; i < N; i++) printf("%0.2f ", Y_N[i]);
    printf("]\n");
    
    //Test sur L_N
    float L_N[N];
    modem_BPSK_demodulate(Y_N, L_N, N, sigma);
    printf("L_N = [ ");
    for(int i = 0; i < N; i++) printf("%0.2f ", L_N[i]);
    printf("]\n\n");

    //Test sur VK
    uint8_t V_Kh[K], V_Ks[K];
    uint64_t be=0, fe=0;
    
    // hard decoder
    codec_repetition_hard_decode(L_N, V_Kh, K, n_reps);
    printf("V_Kh = [ ");
    for(int i = 0; i < K; i++) printf("%d ", V_Kh[i]);
    printf("]\n");
    monitor_check_errors(U_K, V_Kh, K, &be, &fe);
    printf("bit errors : %llu\nframe errors : %llu\n\n", be, fe);

    // soft decoder
    codec_repetition_soft_decode(L_N, V_Ks, K, n_reps);
    printf("V_Ks = [ ");
    for(int i = 0; i < K; i++) printf("%d ", V_Ks[i]);
    printf("]\n");
    monitor_check_errors(U_K, V_Ks, K, &be, &fe);
    printf("bit errors : %llu\nframe errors : %llu\n\n", be, fe);

    

    return 0;
}