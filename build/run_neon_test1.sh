#!/bin/bash

echo "=========================================================="
echo "🚀 LANCEMENT DE LA CAMPAGNE SIMD NEON (K=32 et K=256)"
echo "=========================================================="

# 1. Paramètres de quantification optimisés
HARD_QS=3
HARD_QF=1

# 2. Création des dossiers spécifiques au NEON
mkdir -p resultats_csv_neon resultats_stats_neon

echo -e "\n---> VAGUE 1/2 : Simulations NEON 8-bits (K = 32)"
# ------------------------------------------------------------------------------
./simulator -m 0 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-hard8-neon" --qs $HARD_QS --qf $HARD_QF > resultats_csv_neon/sim1.csv 2> resultats_stats_neon/sim1_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-soft8-neon" --qs 6 --qf 3 > resultats_csv_neon/sim2.csv 2> resultats_stats_neon/sim2_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-soft8-neon" --qs 5 --qf 2 > resultats_csv_neon/sim3.csv 2> resultats_stats_neon/sim3_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-soft8-neon" --qs 6 --qf 1 > resultats_csv_neon/sim4.csv 2> resultats_stats_neon/sim4_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-soft8-neon" --qs 1 --qf 0 > resultats_csv_neon/sim5.csv 2> resultats_stats_neon/sim5_stats.txt 
echo "Vague 1 terminée."


echo -e "\n---> VAGUE 2/2 : Simulations NEON 8-bits (K = 256)"
# ------------------------------------------------------------------------------
./simulator -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-hard8-neon" --qs $HARD_QS --qf $HARD_QF > resultats_csv_neon/sim1_k256.csv 2> resultats_stats_neon/sim1_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-soft8-neon" --qs 6 --qf 3 > resultats_csv_neon/sim2_k256.csv 2> resultats_stats_neon/sim2_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-soft8-neon" --qs 5 --qf 2 > resultats_csv_neon/sim3_k256.csv 2> resultats_stats_neon/sim3_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-soft8-neon" --qs 6 --qf 1 > resultats_csv_neon/sim4_k256.csv 2> resultats_stats_neon/sim4_k256_stats.txt 
./simulator -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-soft8-neon" --qs 1 --qf 0 > resultats_csv_neon/sim5_k256.csv 2> resultats_stats_neon/sim5_k256_stats.txt 

echo "Vague 2 terminée."

echo "Les fichiers sont prêts dans les dossiers 'resultats_csv_neon' et 'resultats_stats_neon'."