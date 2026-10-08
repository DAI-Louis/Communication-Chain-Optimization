#!/bin/bash

echo "=========================================================="
echo "🚀 CAMPAGNE MULTI-THREADING EXHAUSTIVE (Noms sim1 à sim5)"
echo "=========================================================="

# 1. Paramètres de quantification optimisés
HARD_QS=3
HARD_QF=1

# 2. Création de l'arborescence complète
mkdir -p resultats_csv_thread/K32/{Standard,Neon,Packed}
mkdir -p resultats_csv_thread/K256/{Standard,Neon,Packed}

mkdir -p resultats_stats_thread/K32/{Standard,Neon,Packed}
mkdir -p resultats_stats_thread/K256/{Standard,Neon,Packed}

echo -e "\n---> VAGUE 1/2 : Trame Courte (K = 32)"
# ==============================================================================

# --- FAMILLE 1 : STANDARD FLOAT (Modes 0 & 1) ---
echo ">> Lancement Standard Float (0/1)..."
#./simulator_thread -m 0 -M 14 -s 1 -e 100 -K 32 -N 128 -D "rep-hard" > resultats_csv_thread/K32/Standard/sim1_hard.csv 2> resultats_stats_thread/K32/Standard/sim1_hard_stats.txt
#./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-soft" > resultats_csv_thread/K32/Standard/sim2_soft.csv 2> resultats_stats_thread/K32/Standard/sim2_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-soft" > resultats_csv_thread/K32/Standard/sim3_soft.csv 2> resultats_stats_thread/K32/Standard/sim3_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-soft" > resultats_csv_thread/K32/Standard/sim4_soft.csv 2> resultats_stats_thread/K32/Standard/sim4_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-soft" > resultats_csv_thread/K32/Standard/sim5_soft.csv 2> resultats_stats_thread/K32/Standard/sim5_soft_stats.txt

# --- FAMILLE 2 : STANDARD 8-BITS (Modes 2 & 3) ---
# echo ">> Lancement Standard 8-bits (2/3)..."
# ./simulator_thread -m 0 -M 14 -s 1 -e 100 -K 32 -N 128 -D "rep-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K32/Standard/sim1_hard8.csv 2> resultats_stats_thread/K32/Standard/sim1_hard8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-soft8" --qs 6 --qf 3 > resultats_csv_thread/K32/Standard/sim2_soft8.csv 2> resultats_stats_thread/K32/Standard/sim2_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-soft8" --qs 5 --qf 2 > resultats_csv_thread/K32/Standard/sim3_soft8.csv 2> resultats_stats_thread/K32/Standard/sim3_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-soft8" --qs 6 --qf 1 > resultats_csv_thread/K32/Standard/sim4_soft8.csv 2> resultats_stats_thread/K32/Standard/sim4_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-soft8" --qs 1 --qf 0 > resultats_csv_thread/K32/Standard/sim5_soft8.csv 2> resultats_stats_thread/K32/Standard/sim5_soft8_stats.txt

# --- FAMILLE 3 : NEON 8-BITS (Modes 4 & 5) ---
echo ">> Lancement NEON 8-bits (4/5)..."
# ./simulator_thread -m 0 -M 14 -s 1 -e 100 -K 32 -N 128 -D "rep-hard8-neon" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K32/Neon/sim1_hard8.csv 2> resultats_stats_thread/K32/Neon/sim1_hard8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-soft8-neon" --qs 6 --qf 3 > resultats_csv_thread/K32/Neon/sim2_soft8.csv 2> resultats_stats_thread/K32/Neon/sim2_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-soft8-neon" --qs 5 --qf 2 > resultats_csv_thread/K32/Neon/sim3_soft8.csv 2> resultats_stats_thread/K32/Neon/sim3_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-soft8-neon" --qs 6 --qf 1 > resultats_csv_thread/K32/Neon/sim4_soft8.csv 2> resultats_stats_thread/K32/Neon/sim4_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-soft8-neon" --qs 1 --qf 0 > resultats_csv_thread/K32/Neon/sim5_soft8.csv 2> resultats_stats_thread/K32/Neon/sim5_soft8_stats.txt

# --- FAMILLE 4 : PACKED FLOAT (Modes 6 & 7) ---
echo ">> Lancement Packed Float (6/7)..."
# ./simulator_thread -m 0 -M 14 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard" > resultats_csv_thread/K32/Packed/sim1_hard.csv 2> resultats_stats_thread/K32/Packed/sim1_hard_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-soft" > resultats_csv_thread/K32/Packed/sim2_soft.csv 2> resultats_stats_thread/K32/Packed/sim2_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-bit-packing-soft" > resultats_csv_thread/K32/Packed/sim3_soft.csv 2> resultats_stats_thread/K32/Packed/sim3_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-bit-packing-soft" > resultats_csv_thread/K32/Packed/sim4_soft.csv 2> resultats_stats_thread/K32/Packed/sim4_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-bit-packing-soft" > resultats_csv_thread/K32/Packed/sim5_soft.csv 2> resultats_stats_thread/K32/Packed/sim5_soft_stats.txt

# --- FAMILLE 5 : PACKED 8-BITS (Modes 8 & 9) ---
echo ">> Lancement Packed 8-bits (8/9)..."
# ./simulator_thread -m 0 -M 14 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K32/Packed/sim1_hard8.csv 2> resultats_stats_thread/K32/Packed/sim1_hard8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-soft8" --qs 6 --qf 3 > resultats_csv_thread/K32/Packed/sim2_soft8.csv 2> resultats_stats_thread/K32/Packed/sim2_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 96  -D "rep-bit-packing-soft8" --qs 5 --qf 2 > resultats_csv_thread/K32/Packed/sim3_soft8.csv 2> resultats_stats_thread/K32/Packed/sim3_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 64  -D "rep-bit-packing-soft8" --qs 6 --qf 1 > resultats_csv_thread/K32/Packed/sim4_soft8.csv 2> resultats_stats_thread/K32/Packed/sim4_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 32 -N 32  -D "rep-bit-packing-soft8" --qs 1 --qf 0 > resultats_csv_thread/K32/Packed/sim5_soft8.csv 2> resultats_stats_thread/K32/Packed/sim5_soft8_stats.txt


echo -e "\n---> VAGUE 2/2 : Trame Longue (K = 256)"
# ==============================================================================

# --- FAMILLE 1 : STANDARD FLOAT (Modes 0 & 1) ---
echo ">> Lancement Standard Float (0/1)..."
# ./simulator_thread -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-hard" > resultats_csv_thread/K256/Standard/sim1_hard.csv 2> resultats_stats_thread/K256/Standard/sim1_hard_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-soft" > resultats_csv_thread/K256/Standard/sim2_soft.csv 2> resultats_stats_thread/K256/Standard/sim2_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-soft" > resultats_csv_thread/K256/Standard/sim3_soft.csv 2> resultats_stats_thread/K256/Standard/sim3_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-soft" > resultats_csv_thread/K256/Standard/sim4_soft.csv 2> resultats_stats_thread/K256/Standard/sim4_soft_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-soft" > resultats_csv_thread/K256/Standard/sim5_soft.csv 2> resultats_stats_thread/K256/Standard/sim5_soft_stats.txt

# --- FAMILLE 2 : STANDARD 8-BITS (Modes 2 & 3) ---
echo ">> Lancement Standard 8-bits (2/3)..."
# ./simulator_thread -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K256/Standard/sim1_hard8.csv 2> resultats_stats_thread/K256/Standard/sim1_hard8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-soft8" --qs 6 --qf 3 > resultats_csv_thread/K256/Standard/sim2_soft8.csv 2> resultats_stats_thread/K256/Standard/sim2_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-soft8" --qs 5 --qf 2 > resultats_csv_thread/K256/Standard/sim3_soft8.csv 2> resultats_stats_thread/K256/Standard/sim3_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-soft8" --qs 6 --qf 1 > resultats_csv_thread/K256/Standard/sim4_soft8.csv 2> resultats_stats_thread/K256/Standard/sim4_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-soft8" --qs 1 --qf 0 > resultats_csv_thread/K256/Standard/sim5_soft8.csv 2> resultats_stats_thread/K256/Standard/sim5_soft8_stats.txt

# --- FAMILLE 3 : NEON 8-BITS (Modes 4 & 5) ---
echo ">> Lancement NEON 8-bits (4/5)..."
# ./simulator_thread -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-hard8-neon" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K256/Neon/sim1_hard8.csv 2> resultats_stats_thread/K256/Neon/sim1_hard8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-soft8-neon" --qs 6 --qf 3 > resultats_csv_thread/K256/Neon/sim2_soft8.csv 2> resultats_stats_thread/K256/Neon/sim2_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-soft8-neon" --qs 5 --qf 2 > resultats_csv_thread/K256/Neon/sim3_soft8.csv 2> resultats_stats_thread/K256/Neon/sim3_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-soft8-neon" --qs 6 --qf 1 > resultats_csv_thread/K256/Neon/sim4_soft8.csv 2> resultats_stats_thread/K256/Neon/sim4_soft8_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-soft8-neon" --qs 1 --qf 0 > resultats_csv_thread/K256/Neon/sim5_soft8.csv 2> resultats_stats_thread/K256/Neon/sim5_soft8_stats.txt

# --- FAMILLE 4 : PACKED FLOAT (Modes 6 & 7) ---
echo ">> Lancement Packed Float (6/7)..."
# ./simulator_thread -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-hard" > resultats_csv_thread/K256/Packed/sim1_hard.csv 2> resultats_stats_thread/K256/Packed/sim1_hard_stats.txt
# ./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-soft" > resultats_csv_thread/K256/Packed/sim2_soft.csv 2> resultats_stats_thread/K256/Packed/sim2_soft_stats.txt
./simulator_thread -m 0 -M 11 -s 1 -e 100 -K 256 -N 768  -D "rep-bit-packing-soft" > resultats_csv_thread/K256/Packed/sim3_soft.csv 2> resultats_stats_thread/K256/Packed/sim3_soft_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-bit-packing-soft" > resultats_csv_thread/K256/Packed/sim4_soft.csv 2> resultats_stats_thread/K256/Packed/sim4_soft_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-bit-packing-soft" > resultats_csv_thread/K256/Packed/sim5_soft.csv 2> resultats_stats_thread/K256/Packed/sim5_soft_stats.txt

# --- FAMILLE 5 : PACKED 8-BITS (Modes 8 & 9) ---
echo ">> Lancement Packed 8-bits (8/9)..."
./simulator_thread -m 0 -M 15 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-hard8" --qs $HARD_QS --qf $HARD_QF > resultats_csv_thread/K256/Packed/sim1_hard8.csv 2> resultats_stats_thread/K256/Packed/sim1_hard8_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 1024 -D "rep-bit-packing-soft8" --qs 6 --qf 3 > resultats_csv_thread/K256/Packed/sim2_soft8.csv 2> resultats_stats_thread/K256/Packed/sim2_soft8_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-bit-packing-soft8" --qs 5 --qf 2 > resultats_csv_thread/K256/Packed/sim3_soft8.csv 2> resultats_stats_thread/K256/Packed/sim3_soft8_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 512  -D "rep-bit-packing-soft8" --qs 6 --qf 1 > resultats_csv_thread/K256/Packed/sim4_soft8.csv 2> resultats_stats_thread/K256/Packed/sim4_soft8_stats.txt
./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 256  -D "rep-bit-packing-soft8" --qs 1 --qf 0 > resultats_csv_thread/K256/Packed/sim5_soft8.csv 2> resultats_stats_thread/K256/Packed/sim5_soft8_stats.txt

echo ">> Bonus"

./simulator_thread -m 0 -M 12 -s 1 -e 100 -K 256 -N 768  -D "rep-soft" > resultats_csv_thread/K256/Standard/sim3_soft.csv 2> resultats_stats_thread/K256/Standard/sim3_soft_stats.txt
./simulator_thread -m 15 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-hard" >> resultats_csv_thread/K32/Standard/sim1_hard.csv 2>> resultats_stats_thread/K32/Standard/sim1_hard_stats.txt
./simulator_thread -m 15 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-hard8" --qs $HARD_QS --qf $HARD_QF >> resultats_csv_thread/K32/Standard/sim1_hard8.csv 2>> resultats_stats_thread/K32/Standard/sim1_hard8_stats.txt
./simulator_thread -m 15 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-hard8-neon" --qs $HARD_QS --qf $HARD_QF >> resultats_csv_thread/K32/Neon/sim1_hard8.csv 2>> resultats_stats_thread/K32/Neon/sim1_hard8_stats.txt
./simulator_thread -m 15 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard" >> resultats_csv_thread/K32/Packed/sim1_hard.csv 2>> resultats_stats_thread/K32/Packed/sim1_hard_stats.txt
./simulator_thread -m 15 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-bit-packing-hard8" --qs $HARD_QS --qf $HARD_QF >> resultats_csv_thread/K32/Packed/sim1_hard8.csv 2>> resultats_stats_thread/K32/Packed/sim1_hard8_stats.txt



echo "✅ Toutes les simulations sont terminées !"