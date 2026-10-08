#!/bin/bash

echo "=========================================================="
echo "🚀 CAMPAGNE BIT-PACKING INTÉGRALE (Modes 6, 7, 8, 9)"
echo "=========================================================="

# 1. Paramètres de quantification optimisés (pour les modes 8-bits)
HARD_QS=3
HARD_QF=1

# 2. Création des dossiers
mkdir -p resultats_csv_packed resultats_stats_packed

echo -e "\n---> VAGUE 1/2 : Trame Courte (K = 32)"
# ==============================================================================

# --- Mode 6 : Hard Classique (Float) ---
./simulator -m 0 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard" > resultats_csv_packed/sim1_hard.csv 2> resultats_stats_packed/sim1_hard_stats.txt 

# --- Mode 7 : Soft Classique (Float) ---
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-soft" > resultats_csv_packed/sim2_soft.csv 2> resultats_stats_packed/sim2_soft_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-bit-packing-soft" > resultats_csv_packed/sim3_soft.csv 2> resultats_stats_packed/sim3_soft_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-bit-packing-soft" > resultats_csv_packed/sim4_soft.csv 2> resultats_stats_packed/sim4_soft_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-bit-packing-soft" > resultats_csv_packed/sim5_soft.csv 2> resultats_stats_packed/sim5_soft_stats.txt 

# --- Mode 8 : Hard 8-bits (Quantifié) ---
./simulator -m 0 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_packed/sim1_hard8.csv 2> resultats_stats_packed/sim1_hard8_stats.txt 

# --- Mode 9 : Soft 8-bits (Quantifié) ---
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-soft8" --qs 6 --qf 3 > resultats_csv_packed/sim2_soft8.csv 2> resultats_stats_packed/sim2_soft8_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-bit-packing-soft8" --qs 5 --qf 2 > resultats_csv_packed/sim3_soft8.csv 2> resultats_stats_packed/sim3_soft8_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-bit-packing-soft8" --qs 6 --qf 1 > resultats_csv_packed/sim4_soft8.csv 2> resultats_stats_packed/sim4_soft8_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-bit-packing-soft8" --qs 1 --qf 0 > resultats_csv_packed/sim5_soft8.csv 2> resultats_stats_packed/sim5_soft8_stats.txt 

echo "Vague 1 terminée."


echo -e "\n---> VAGUE 2/2 : Trame Longue (K = 256)"
# ==============================================================================

# --- Mode 6 : Hard Classique (Float) ---
./simulator -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-hard" > resultats_csv_packed/sim1_hard_k256.csv 2> resultats_stats_packed/sim1_hard_k256_stats.txt 

# --- Mode 7 : Soft Classique (Float) ---
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-soft" > resultats_csv_packed/sim2_soft_k256.csv 2> resultats_stats_packed/sim2_soft_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-bit-packing-soft" > resultats_csv_packed/sim3_soft_k256.csv 2> resultats_stats_packed/sim3_soft_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-bit-packing-soft" > resultats_csv_packed/sim4_soft_k256.csv 2> resultats_stats_packed/sim4_soft_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-bit-packing-soft" > resultats_csv_packed/sim5_soft_k256.csv 2> resultats_stats_packed/sim5_soft_k256_stats.txt 

# --- Mode 8 : Hard 8-bits (Quantifié) ---
./simulator -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_packed/sim1_hard8_k256.csv 2> resultats_stats_packed/sim1_hard8_k256_stats.txt 

# --- Mode 9 : Soft 8-bits (Quantifié) ---
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-soft8" --qs 6 --qf 3 > resultats_csv_packed/sim2_soft8_k256.csv 2> resultats_stats_packed/sim2_soft8_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-bit-packing-soft8" --qs 5 --qf 2 > resultats_csv_packed/sim3_soft8_k256.csv 2> resultats_stats_packed/sim3_soft8_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-bit-packing-soft8" --qs 6 --qf 1 > resultats_csv_packed/sim4_soft8_k256.csv 2> resultats_stats_packed/sim4_soft8_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-bit-packing-soft8" --qs 1 --qf 0 > resultats_csv_packed/sim5_soft8_k256.csv 2> resultats_stats_packed/sim5_soft8_k256_stats.txt 

echo "Vague 2 terminée."
echo "✅ Tous les fichiers sont prêts ! Vous pouvez analyser les gains en Mbps."