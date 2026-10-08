#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <limits.h>
#include <arm_neon.h>
#include <pthread.h>
//Bibliothèque pour les variables atomiques
#include <stdatomic.h>

#include "transmitter.h"
#include "receiver.h"

#define NTHREADS 6

typedef struct{
    uint64_t f_max, info_bits, codeword_size;
    uint8_t d_type, id;

    double ttb[7];
    double minb[7];
    double maxb[7];
    double stb[7];
    double tot_time;
    size_t n_reps;
    unsigned seed;
} sim_args;

//Variable globale qui permet l'atomicité
atomic_uint_least64_t n_bit_errors, n_frame_errrors, n_frame;
float sigma;

int use_all_zeros = 0; 
int mod_all_ones = 0;  

int use_quantizer = 0; // Passe à 1 si --qf est détecté
int qf_val = 0;
int qs_val = 8;        // Valeur par défaut imposée par la consigne


void* simulation(void* ptr){
    sim_args *args = (sim_args*)ptr; // On ouvre la boîte à outils
    
    // Variables purement locales (chaque thread a les siennes)
    double timeblck;
    clock_t startblock;
    // clock_t debut_thread = clock();

    uint8_t U_K[args->info_bits], 
            V_K[args->info_bits],
            C_N[args->codeword_size];
    int32_t X_N[args->codeword_size];
    float   Y_N[args->codeword_size],
            L_N[args->codeword_size];
    int8_t  L8_N[args->codeword_size];

    /*packing array*/
    uint8_t U_K_packed[args->info_bits/8], 
            V_K_packed[args->info_bits/8],
            C_N_packed[args->codeword_size/8];

    uint64_t mes_frames_locales = 0;

    // Initialisation des stats de CE thread
    for(int i = 0; i < 7; i++){
        args->minb[i] = 9999.0;
        args->maxb[i] = 0.0;
        args->ttb[i]  = 0.0;
        args->stb[i]  = 0.0;
    }

    if (mod_all_ones) {
            // U_K doit être rempli de zéros pour que le monitor soit content
            for (size_t i = 0; i < args->info_bits; i++) U_K[i] = 0;
            // X_N est rempli de 1

            memset(C_N, 1, args->codeword_size);

            modem_BPSK_modulate_all_ones(C_N, X_N, args->codeword_size);
        }
     do{

            clock_t debut = clock();
            mes_frames_locales++;
            
            // generate U_K
            if (!mod_all_ones) {
                
                if (args->d_type >= 6 && args->d_type <= 9){
                    startblock = clock();
                    if((use_all_zeros == 0)){
                        source_generate_packed_thread(U_K_packed, args->info_bits, &args->seed);
                    }
                    else{
                        for (size_t i = 0; i < args->info_bits / 8; i++)
                            U_K_packed[i] = 0;
                    }
                }
                else {
                    startblock = clock();
                    (use_all_zeros == 0) ? source_generate_thread(U_K, args->info_bits, &args->seed) : source_generate_all_zeros(U_K, args->info_bits);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                args->ttb [0] += timeblck;
                args->minb[0] = (timeblck < args->minb[0]) ? timeblck : args->minb[0];
                args->maxb[0] = (timeblck > args->maxb[0]) ? timeblck : args->maxb[0];
                args->stb [0] += args->info_bits;
            }

            // generate C_N
            if (!mod_all_ones) {
                if(args->d_type >= 6 && args->d_type <= 9){
                    startblock = clock();
                    codec_repetition_encode_packed(U_K_packed, C_N_packed, args->info_bits, args->n_reps);
                }
                else{
                    startblock = clock();
                    codec_repetition_encode(U_K, C_N, args->info_bits, args->n_reps);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                args->ttb [1] += timeblck;
                args->minb[1] = (timeblck < args->minb[1]) ? timeblck : args->minb[1];
                args->maxb[1] = (timeblck > args->maxb[1]) ? timeblck : args->maxb[1];
                args->stb [1] += args->codeword_size;
            }
            // generate X_N
            if (!mod_all_ones) {
                if (args->d_type == 4 || args->d_type == 5){
                    startblock = clock();
                    modem_BPSK_modulate_neon(C_N, X_N, args->codeword_size);
                }
                else if (args->d_type >= 6 && args->d_type <= 9){
                    startblock = clock();
                    modem_BPSK_modulate_packed(C_N_packed, X_N, args->codeword_size);
                }
                else{
                    startblock = clock();
                    modem_BPSK_modulate(C_N, X_N, args->codeword_size);
                }
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
                args->ttb [2] += timeblck;
                args->minb[2] = (timeblck < args->minb[2]) ? timeblck : args->minb[2];
                args->maxb[2] = (timeblck > args->maxb[2]) ? timeblck : args->maxb[2];
                args->stb [2] += args->codeword_size;
            }
            
            startblock = clock();
            channel_AWGN_add_noise_thread(X_N, Y_N, args->codeword_size, sigma, &args->seed);
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            args->ttb [3] += timeblck;
            args->minb[3] = (timeblck < args->minb[3]) ? timeblck : args->minb[3];
            args->maxb[3] = (timeblck > args->maxb[3]) ? timeblck : args->maxb[3];
            args->stb [3] += args->codeword_size;

            startblock = clock();
            if (args->d_type == 4 || args->d_type == 5) modem_BPSK_demodulate8_neon(Y_N, L_N, args->codeword_size, sigma);
            else                            modem_BPSK_demodulate(Y_N, L_N, args->codeword_size, sigma);
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            args->ttb [4] += timeblck;
            args->minb[4] = (timeblck < args->minb[4]) ? timeblck : args->minb[4];
            args->maxb[4] = (timeblck > args->maxb[4]) ? timeblck : args->maxb[4];
            args->stb [4] += args->codeword_size;

            if (use_quantizer) {
                if (args->d_type == 4 || args->d_type == 5) quantizer_transform8_neon(L_N, L8_N, args->codeword_size, qs_val, qf_val);
                else                            quantizer_transform8(L_N, L8_N, args->codeword_size, qs_val, qf_val);
            }

            if(args->d_type == 0 || args->d_type == 6){
                // hard decoder
                startblock = clock();
                codec_repetition_hard_decode(L_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if(args->d_type == 1 || args->d_type == 7){       
                // soft decoder
                startblock = clock();
                codec_repetition_soft_decode(L_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (args->d_type == 2 || args->d_type == 8) {
                startblock = clock();
                codec_repetition_hard_decode8(L8_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (args->d_type == 3 || args->d_type == 9) {
                startblock = clock();
                codec_repetition_soft_decode8(L8_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else if (args->d_type == 4){
                startblock = clock();
                codec_repetition_hard_decode8_neon(L8_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            else {
                startblock = clock();
                codec_repetition_soft_decode8_neon(L8_N, V_K, args->info_bits, args->n_reps);
                timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            }
            args->ttb [5] += timeblck;
            args->minb[5] = (timeblck < args->minb[5]) ? timeblck : args->minb[5];
            args->maxb[5] = (timeblck > args->maxb[5]) ? timeblck : args->maxb[5];
            args->stb [5] += args->info_bits;

            startblock = clock();
            uint64_t erreurs_bits_locales = 0, erreurs_frames_locales = 0;

            if (args->d_type >= 6 && args->d_type <= 9){
                bit_pack_array(V_K, V_K_packed, args->info_bits);
                monitor_check_errors_packed(U_K_packed, V_K_packed, args->info_bits, &erreurs_bits_locales, &erreurs_frames_locales);
            }
            else if (args->d_type == 4 || args->d_type == 5) monitor_check_errors_neon(U_K, V_K, args->info_bits, &erreurs_bits_locales, &erreurs_frames_locales);
            else                                             monitor_check_errors     (U_K, V_K, args->info_bits, &erreurs_bits_locales, &erreurs_frames_locales);
            if (erreurs_frames_locales > 0) {
                 atomic_fetch_add(&n_bit_errors, erreurs_bits_locales);
                 atomic_fetch_add(&n_frame_errrors, 1);
            }
            timeblck = (double)(clock() - startblock) / CLOCKS_PER_SEC;
            args->ttb [6] += timeblck;
            args->minb[6] = (timeblck < args->minb[6]) ? timeblck : args->minb[6];
            args->maxb[6] = (timeblck > args->maxb[6]) ? timeblck : args->maxb[6];
            args->stb [6] += args->info_bits;

            clock_t fin = clock();

            double temps_secondes = (double)(fin - debut) / CLOCKS_PER_SEC;
            args->tot_time += temps_secondes;
            //char buf[255];
            
    }while(atomic_load(&n_frame_errrors) < args->f_max);

    atomic_fetch_add(&n_frame, mes_frames_locales);

    return NULL;
}

int main(int argc, char *argv[]){
    //Défini pour éviter les warnings
    float min_SNR = 0.0, max_SNR = 12.0, step_val = 1.0;
    uint64_t f_max = 100, info_bits = 32, codeword_size = 128;
    uint8_t d_type = 0;
    
    int opt;
    //Norme
    int option_index = 0; 

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
                // il met automatiquement la variable à 1 et renvoie 0. 
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
    
    sim_args args_array  [NTHREADS]; 
    // pthread_t mes_threads[NTHREADS];
    size_t n_reps = codeword_size/info_bits;

    for(int i = 0; i < NTHREADS; i++){
        args_array[i].f_max         = f_max;
        args_array[i].info_bits     = info_bits;
        args_array[i].codeword_size = codeword_size;
        args_array[i].d_type        = d_type;

        args_array[i].id            = i;
        args_array[i].n_reps        = n_reps;
        args_array[i].seed = time(NULL) + i;
    }


    float R = (float)info_bits/(float)codeword_size;
    
    

    printf("Eb/N0,Es/N0,sigma,be,fe,fn,BER,FER,total time,avg time,Mbps\n");
    for(int SNR = min_SNR; SNR<=max_SNR; SNR+=step_val){
        atomic_store(&n_bit_errors, 0);
        atomic_store(&n_frame_errrors, 0);
        atomic_store(&n_frame, 0);
        // double tot_time = 0, avg_time = 0;
        // double Sim_thr  = 0;

        float Es_N0 = SNR + 10.0f*log10f(R);
        sigma = sqrt( 1.0f / ( 2.0f * (float)pow( 10, (Es_N0/10.0) ) ) );
        // double ttb[7], minb[7], maxb[7]; /////////, avgb[7];
        // double stb[7];
        struct timespec debut_vrai_temps, fin_vrai_temps;
        clock_gettime(CLOCK_MONOTONIC, &debut_vrai_temps);

        pthread_t thread_tab[NTHREADS];
        for(int i = 0; i < NTHREADS; i++){
            pthread_create(&thread_tab[i], NULL, simulation, &args_array[i]);
        }

        for(int i = 0; i < NTHREADS; i++){
            pthread_join(thread_tab[i], NULL);
        }


        uint64_t total_n_frame         = atomic_load(&n_frame);
        uint64_t total_n_frame_errrors = atomic_load(&n_frame_errrors);
        uint64_t total_n_bit_errors    = atomic_load(&n_bit_errors);

        clock_gettime(CLOCK_MONOTONIC, &fin_vrai_temps);
        double vrai_temps_secondes = (fin_vrai_temps.tv_sec - debut_vrai_temps.tv_sec) + 
                                     (fin_vrai_temps.tv_nsec - debut_vrai_temps.tv_nsec) / 1e9;

        double tot_time = 0.0, avg_time = 0.0, Sim_thr = 0.0;
        double ttb[7] = {0.0}, stb[7] = {0.0}, maxb[7] = {0.0};
        double minb[7];
        for (int i = 0; i < 7; i++) minb[i] = 9999.0;

        for (int i = 0; i < NTHREADS; i++) {
            // Cumul du temps total CPU
            tot_time += args_array[i].tot_time;
            
            // Fusion des statistiques par bloc
            for (int b = 0; b < 7; b++) {
                ttb[b] += args_array[i].ttb[b];
                stb[b] += args_array[i].stb[b];
                if (args_array[i].minb[b] < minb[b]) minb[b] = args_array[i].minb[b];
                if (args_array[i].maxb[b] > maxb[b]) maxb[b] = args_array[i].maxb[b];
            }
        }

        float FER = (float)total_n_frame_errrors / total_n_frame;
        float BER = (float)total_n_bit_errors / (total_n_frame * info_bits);
        avg_time = tot_time/total_n_frame;   //Peut être opti
        double real_wall_time = tot_time / NTHREADS;
        Sim_thr  = ((total_n_frame * info_bits) / 1000000.0) / real_wall_time;
        double vrai_Sim_thr = ((total_n_frame * info_bits) / 1000000.0) / vrai_temps_secondes;
        printf("%d,%0.2f,%0.2f,%lu,%lu,%lu,%0.2f,%0.2f,%f,%f,%f\n", SNR, Es_N0, sigma, total_n_bit_errors, total_n_frame_errrors, total_n_frame, BER, FER, tot_time, avg_time, vrai_Sim_thr);
        // Teste si ENABLE_STATS n'est défini alors affichage
        #ifdef ENABLE_STATS
            //Calcul préalable des valeurs pour chaque bloc ---
            double avg[7], mbps[7], pct[7];
            for (int i = 0; i < 7; i++) {
                avg[i]  = ttb[i] / total_n_frame; 
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