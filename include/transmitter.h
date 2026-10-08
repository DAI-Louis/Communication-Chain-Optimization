#ifndef TRANSM_H
#define TRANSM_H

#include <stdlib.h>
#include <stdint.h>

// write into the buffer U_K
void source_generate(uint8_t *U_K, size_t K);

// write only zeros in U_K
void source_generate_all_zeros(uint8_t *U_K, size_t K);

// read from the buffer U_K and write into the buffer C_N
void codec_repetition_encode(const uint8_t *U_K, uint8_t *C_N, size_t K, size_t n_reps);

// read from C_N, write into X_N
void modem_BPSK_modulate(const uint8_t *C_N, int32_t *X_N, size_t N);

// write only ones in X_N
void modem_BPSK_modulate_all_ones(const uint8_t *C_N, int32_t *X_N, size_t N);

void modem_BPSK_modulate_neon(const uint8_t *C_N, int32_t *X_N, size_t N);


// write into the buffer U_K
void source_generate_packed(uint8_t *U_K_packed, size_t K);

// read from the buffer U_K and write into the buffer C_N
void codec_repetition_encode_packed(const uint8_t *U_K_packed, uint8_t *C_N_packed, size_t K, size_t n_reps);

// read from C_N, write into X_N
void modem_BPSK_modulate_packed(const uint8_t *C_N, int32_t *X_N, size_t N);

// write into the buffer U_K
void source_generate_thread(uint8_t *U_K, size_t K, unsigned int *seed);

// write into the buffer U_K
void source_generate_packed_thread(uint8_t *U_K_packed, size_t K, unsigned int *seed);

#endif