#ifndef GENERATE_STATISTICS_H
#define GENERATE_STATISTICS_H 

#include <string>

#include "deck.h"
#include "hand.h"
#include "player.h"
#include "statistics.h"

template <typename T>
class Table
{
public:
    /*
     * @brief Default constructor.
     */
    Table();

    /*
     * @brief Constructor with stat name used for printing.
     */
    Table(std::string stats_name);

    T& get_my_crib(int index1, int index2) { return table_my_crib[index1][index2]; }
    T& get_opp_crib(int index1, int index2) { return table_opp_crib[index1][index2]; }

    void pretty_print();

    void save(std::string filename);
    void load(std::string filename);

private:
    std::string stats_name;
    T table_my_crib[13][13];
    T table_opp_crib[13][13];
};

class GenerateCribStatistics
{
public: 
    GenerateCribStatistics(Player* dealer, Player* pone);

    void generate_all_tables();

    void generate_freq_tables();

    void generate_max_min_tables();
    void generate_mean_tables();
    void generate_median_tables();

    void generate_variance_tables();

    void generate_std_dev_tables();


    void load_mean_tables(std::string filename);
    void save_mean_tables(std::string filename);

    void load_median_tables(std::string filename);
    void save_median_tables(std::string filename);

    void load_freq_tables(std::string filename);
    void save_freq_tables(std::string filename);

    void load_max_min_tables(std::string filename);
    void save_max_min_tables(std::string filename);

    void load_variance_tables(std::string filename);
    void save_variance_tables(std::string filename);

    void load_std_dev_tables(std::string filename);
    void save_std_dev_tables(std::string filename);

    void pretty_print_freq_tables();
    void pretty_print_max_tables();

private:
    void generate_unscored_freq_tables();
    void generate_scored_freq_tables();
    /* void generate_score_dist_tables(); */

private:
    Table<float> freq_table;
    int freq_num_games;

    Table<ScoreDistributionTable> score_dist_table;
    bool freq_tables_generated;

    Table<float> mean_table;
    bool mean_tables_generated;

    Table<int> median_table;

    Table<float> variance_table;
    bool variance_tables_generated;

    Table<float> std_dev_table;

    Table<int> max_table;
    Table<int> min_table;

    Player* dealer;
    Player* pone;
};

#endif // GENERATE_STATISTICS_H 
