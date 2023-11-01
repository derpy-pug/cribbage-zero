#ifndef STATISTICS_H
#define STATISTICS_H

struct ScoreFreqTable
{
    int freq_table[28];
    int num_games;
};

struct ScoreDistributionTable
{
    float dist_table[28];
};

#endif // STATISTICS_H
