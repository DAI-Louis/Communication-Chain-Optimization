#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <time.h>
#include <limits.h>
#include <arm_neon.h>

#include "transmitter.h"
#include "receiver.h"

int main(int argc, char *argv[]){
    //Défini pour éviter les warnings
    float min_SNR = 0.0, max_SNR = 12.0, step_val = 1.0;
    uint64_t f_max = 100, info_bits = 32, codeword_size = 128;
    uint8_t d_type = 0;

    int opt;
    //Norme
    int option_index = 0; 

    // Nos deux interrupteurs
    int use_all_zeros = 0; 
    int mod_all_ones = 0;  

    int use_quantizer = 0; // Passe à 1 si --qf est détecté
    int qf_val = 0;
    int qs_val = 8;        // Valeur par défaut imposée par la consigne

    struct option long_options[] = {
        {"src-all-zeros", no_argument, &use_all_zeros, 1},
        {"mod-all-ones",  no_argument, &mod_all_ones, 1}, // <-- NOUVELLE LIGNE
        {"qf",            required_argument, 0, 1000},
        {"qs",            required_argument, 0, 1001},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "m:M:s:e:K:N:D:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                // <-- NOUVEAU : Quand getopt_long lit "--src-all-zeros" ou "--mod-all-ones",
                // il met automatiquement la variable à 1 et renvoie 0. 
                // On a juste besoin de faire un break pour passer à la suite !
                break;
            case 'm':
                min_SNR = atof(optarg);
                break;
            case 'M':
                max_SNR = atof(optarg);
                break;
            case 's':
                step_val = atof(optarg);
                break;
            case 'e':
                f_max = atoi(optarg);
                break;
            case 'K':
                info_bits = atoi(optarg);
                break;
            case 'N':
                codeword_size = atoi(optarg);
                break;
            case 'D':
                if      (!strcmp(optarg, "rep-hard"))              d_type = 0;
                else if (!strcmp(optarg, "rep-soft"))              d_type = 1;
                else if (!strcmp(optarg, "rep-hard8"))             d_type = 2;
                else if (!strcmp(optarg, "rep-soft8"))             d_type = 3;
                else if (!strcmp(optarg, "rep-hard8-neon"))        d_type = 4;
                else if (!strcmp(optarg, "rep-soft8-neon"))        d_type = 5;
                else if (!strcmp(optarg, "rep-bit-packing-hard"))  d_type = 6;
                else if (!strcmp(optarg, "rep-bit-packing-soft"))  d_type = 7;
                else if (!strcmp(optarg, "rep-bit-packing-hard8")) d_type = 8;
                else if (!strcmp(optarg, "rep-bit-packing-soft8")) d_type = 9;
                else{
                    fprintf(stderr,"error : decoder type -> -D [\"rep-hard\"|\"rep-soft\"]\n");
                    exit(1);
                }
            case 1000: // --qf
                qf_val = atoi(optarg);
                if (qf_val < 0 || qf_val > 8) {
                    fprintf(stderr, "error : --qf range is [0;8]\n");
                    exit(1);
                }
                use_quantizer = 1; // On active le module !
                break;
                
            case 1001: // --qs
                qs_val = atoi(optarg);
                if (qs_val < 1 || qs_val > 8) {
                    fprintf(stderr, "error : --qs range is [1;8]\n");
                    exit(1);
                }
                break;
    
                break;
            case '?': // For unknown options
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }

    if(codeword_size%info_bits!=0){
        fprintf(stderr,"error : N has to be a multiple of K\n");
        exit(1);
    }
    
    float R = (float)info_bits/(float)codeword_size;
    size_t n_reps = codeword_size/info_bits;
    uint64_t n_bit_errors, n_frame_errrors, n_frame;

    uint8_t U_K[info_bits], 
            V_K[info_bits],
            C_N[codeword_size];
    int32_t X_N[codeword_size];
    float   Y_N[codeword_size],
            L_N[codeword_size];
    int8_t  L8_N[codeword_size];

    /*packing array*/
    uint8_t U_K_packed[info_bits/8], 
            V_K_packed[info_bits/8],
            C_N_packed[codeword_size/8];

    printf("Eb/N0,Es/N0,sigma,be,fe,fn,BER,FER,total time,avg time,Mbps\n");
    for(int SNR = min_SNR; SNR<=max_SNR; SNR+=step_val){
        n_bit_errors    = 0;
        n_frame_errrors = 0;
        n_frame         = 0;
        double tot_time = 0, avg_time = 0;
        double Sim_thr  = 0;

        float Es_N0 = SNR + 10.0f*log10f(R);
        float sigma = sqrt( 1.0f / ( 2.0f * (float)pow( 10, (Es_N0/10.0) ) ) );

        clock_t startblock;
        double timeblck;
        double ttb[7], minb[7], maxb[7]; /////////, avgb[7];
        double stb[7];


        if (mod_all_ones) {
            // U_K doit être rempli de zéros pour que le monitor soit content
            for (size_t i = 0; i < info_bits; i++) U_K[i] = 0;
            // X_N est rempli de 1
            modem_BPSK_modulate_all_ones(C_N, X_N, codeword_size);
        }
        
        for(int i = 0; i < 7; i++){
            minb[i] = 9999.0;
            maxb[i] = 0.0;
            ttb[i]  = 0.0;
            stb[i]  = 0.0;
        }
        do{

            clock_t debut = clock();
            n_frame++;
            
            // generate U_K
            if (!mod_all_ones) {
                
                if (d_type >= 6 && d_type <= 9){
                    startblock = clock();
                    if((use_all_zeros == 0)){
                        source_generate_packed(U_K_packed, info_bits);
                    }
                    else{
                        for (size_t i = 0; i < info_bits / 8; i++)
                            U_K_packed[i] = 0;
                    }
                }
                else {
                    startblock = clock();
                    (use_all_zeros == 0) ? source_generate(U_K, info_bits) : source_generate_all_zeros(U_K, info_bits);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                ttb [0] += timeblck;
                minb[0] = (timeblck < minb[0]) ? timeblck : minb[0];
                maxb[0] = (timeblck > maxb[0]) ? timeblck : maxb[0];
                stb [0] += info_bits;
            }

            // generate C_N
            if (!mod_all_ones) {
                if(d_type >= 6 && d_type <= 9){
                    startblock = clock();
                    codec_repetition_encode_packed(U_K_packed, C_N_packed, info_bits, n_reps);
                }
                else{
                    startblock = clock();
                    codec_repetition_encode(U_K, C_N, info_bits, n_reps);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                ttb [1] += timeblck;
                minb[1] = (timeblck < minb[1]) ? timeblck : minb[1];
                maxb[1] = (timeblck > maxb[1]) ? timeblck : maxb[1];
                stb [1] += codeword_size;
            }
            // generate X_N
            if (!mod_all_ones) {
                if (d_type == 4 || d_type == 5){
                    startblock = clock();
                    modem_BPSK_modulate_neon(C_N, X_N, codeword_size);
                }
                else if (d_type >= 6 && d_type <= 9){
                    startblock = clock();
                    modem_BPSK_modulate_packed(C_N_packed, X_N, codeword_size);
                }
                else{
                    startblock = clock();
                    modem_BPSK_modulate(C_N, X_N, codeword_size);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                ttb [2] += timeblck;
                minb[2] = (timeblck < minb[2]) ? timeblck : minb[2];
                maxb[2] = (timeblck > maxb[2]) ? timeblck : maxb[2];
                stb [2] += codeword_size;
            }
            
            startblock = clock();
            channel_AWGN_add_noise(X_N, Y_N, codeword_size, sigma);
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            ttb [3] += timeblck;
            minb[3] = (timeblck < minb[3]) ? timeblck : minb[3];
            maxb[3] = (timeblck > maxb[3]) ? timeblck : maxb[3];
            stb [3] += codeword_size;

            startblock = clock();
            if (d_type == 4 || d_type == 5) modem_BPSK_demodulate8_neon(Y_N, L_N, codeword_size, sigma);
            else                            modem_BPSK_demodulate(Y_N, L_N, codeword_size, sigma);
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            ttb [4] += timeblck;
            minb[4] = (timeblck < minb[4]) ? timeblck : minb[4];
            maxb[4] = (timeblck > maxb[4]) ? timeblck : maxb[4];
            stb [4] += codeword_size;

            if (use_quantizer) {
                if (d_type == 4 || d_type == 5) quantizer_transform8_neon(L_N, L8_N, codeword_size, qs_val, qf_val);
                else                            quantizer_transform8(L_N, L8_N, codeword_size, qs_val, qf_val);
            }

            if(d_type == 0 || d_type == 6){
                // hard decoder
                startblock = clock();
                codec_repetition_hard_decode(L_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if(d_type == 1 || d_type == 7){       
                // soft decoder
                startblock = clock();
                codec_repetition_soft_decode(L_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (d_type == 2 || d_type == 8) {
                startblock = clock();
                codec_repetition_hard_decode8(L8_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (d_type == 3 || d_type == 9) {
                startblock = clock();
                codec_repetition_soft_decode8(L8_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (d_type == 4){
                startblock = clock();
                codec_repetition_hard_decode8_neon(L8_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else {
                startblock = clock();
                codec_repetition_soft_decode8_neon(L8_N, V_K, info_bits, n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            ttb [5] += timeblck;
            minb[5] = (timeblck < minb[5]) ? timeblck : minb[5];
            maxb[5] = (timeblck > maxb[5]) ? timeblck : maxb[5];
            stb [5] += info_bits;

            startblock = clock();
            if (d_type >= 6 && d_type <= 9){
                bit_pack_array(V_K, V_K_packed, info_bits);
                monitor_check_errors_packed(U_K_packed, V_K_packed, info_bits, &n_bit_errors, &n_frame_errrors);
            }
            else if (d_type == 4 || d_type == 5) monitor_check_errors_neon(U_K, V_K, info_bits, &n_bit_errors, &n_frame_errrors);
            else                                 monitor_check_errors     (U_K, V_K, info_bits, &n_bit_errors, &n_frame_errrors);
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            ttb [6] += timeblck;
            minb[6] = (timeblck < minb[6]) ? timeblck : minb[6];
            maxb[6] = (timeblck > maxb[6]) ? timeblck : maxb[6];
            stb [6] += info_bits;
            
            clock_t fin = clock();

            double temps_secondes = (double)(fin - debut) / CLOCKS_PER_SEC;
            tot_time += temps_secondes;
            //char buf[255];
            
        }while(n_frame_errrors < f_max);
        float FER = (float)n_frame_errrors / n_frame;
        float BER = (float)n_bit_errors / (n_frame * info_bits);
        avg_time = tot_time/n_frame;   //Peut être opti
        Sim_thr  = ((n_frame * info_bits) / 1000000.0) / tot_time;
        printf("%d,%0.2f,%0.2f,%lu,%lu,%lu,%0.2f,%0.2f,%f,%f,%f\n", SNR, Es_N0, sigma, n_bit_errors, n_frame_errrors, n_frame, BER, FER, tot_time, avg_time, Sim_thr);
        // Teste si ENABLE_STATS n'est défini alors affichage
        #ifdef ENABLE_STATS
            //Calcul préalable des valeurs pour chaque bloc ---
            double avg[7], mbps[7], pct[7];
            for (int i = 0; i < 7; i++) {
                avg[i]  = ttb[i] / n_frame; 
                mbps[i] = (ttb[i] > 0) ? ((stb[i] / 1000000.0) / ttb[i]) : 0.0;
                pct[i]  = (ttb[i] / tot_time) * 100.0;
            }  

            fprintf(stderr, "\n==========================================================================\n");
            fprintf(stderr, "Bloc %d               Avg (s)    Min (s)    Max (s)    Avg Mbps   %% Temps\n", SNR);
            fprintf(stderr, "--------------------------------------------------------------------------\n");
            
            fprintf(stderr, "Temps Source       : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[0], minb[0] == 9999.0 ? 0 : minb[0], maxb[0], mbps[0], pct[0]);
            fprintf(stderr, "Temps Répétition   : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[1], minb[1] == 9999.0 ? 0 : minb[1], maxb[1], mbps[1], pct[1]);
            fprintf(stderr, "Temps Modulation   : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[2], minb[2] == 9999.0 ? 0 : minb[2], maxb[2], mbps[2], pct[2]);
            fprintf(stderr, "Temps AWGN         : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[3], minb[3], maxb[3], mbps[3], pct[3]);
            fprintf(stderr, "Temps Démodulation : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[4], minb[4], maxb[4], mbps[4], pct[4]);
            fprintf(stderr, "Temps Décodage     : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[5], minb[5], maxb[5], mbps[5], pct[5]);
            fprintf(stderr, "Temps Check        : %8.6f, %8.6f, %8.6f, %9.3f, %6.2f %%\n", avg[6], minb[6], maxb[6], mbps[6], pct[6]);
            fprintf(stderr, "==========================================================================\n\n");
        #endif
    }
    return 0;
}