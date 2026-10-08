#!/bin/bash
echo "🔧 Compilation du simulateur avec les statistiques activées..."

mkdir -p resultats_csv_normaux
mkdir -p resultats_stats_normaux

echo "🚀 Lancement de la campagne de simulations (NORMAL)..."

./simulator -m 0 -M 15 -s 1 -e 100 -K 32 -N 128 -D "rep-hard" > resultats_csv_normaux/sim1.csv 2> resultats_stats_normaux/sim1_stats.txt
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 128 -D "rep-soft" > resultats_csv_normaux/sim2.csv 2> resultats_stats_normaux/sim2_stats.txt
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 96 -D "rep-soft" > resultats_csv_normaux/sim3.csv 2> resultats_stats_normaux/sim3_stats.txt
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 64 -D "rep-soft" > resultats_csv_normaux/sim4.csv 2> resultats_stats_normaux/sim4_stats.txt
./simulator -m 0 -M 12 -s 1 -e 100 -K 32 -N 32 -D "rep-soft" > resultats_csv_normaux/sim5.csv 2> resultats_stats_normaux/sim5_stats.txt

echo "✅ Normal terminé !"