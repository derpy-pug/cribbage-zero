#ifndef STATISTICS_H
#define STATISTICS_H

struct ScoreDistributionTable
{
    ScoreDistributionTable()
        : dist_table()
    {
    }
    float dist_table[29];

    float& operator[](int i)
    {
        return dist_table[i];
    }
};

#endif // STATISTICS_H
