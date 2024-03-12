#include "statistics/stats/StatisticsTable.h"

#define TABLE_DIR "tables/"

namespace cribbage {

constexpr static int default_num_sim_discards = 2 << 19;

const static float default_stat_vs_stat_my_crib[13][13] = {
  {0.013979, 0.0284529, 0.0315924, 0.0317945, 0.00486279, 0.00565147,
   0.00692844, 0.00679111, 0.00712204, 0.00541401, 0.00761318, 0.00885773,
   0.0117083},
  {0, 0.0169525, 0.0442209, 0.0211363, 0.00473213, 0.00669384, 0.00923443,
   0.00796509, 0.00788975, 0.00536251, 0.00742912, 0.00865555, 0.012248},
  {0, 0, 0.0145931, 0.0221853, 0.00681114, 0.00627518, 0.00733852, 0.00943089,
   0.00707245, 0.00491714, 0.0067091, 0.00858021, 0.0122013},
  {0, 0, 0, 0.0106754, 0.00629139, 0.00302315, 0.00654888, 0.00491333,
   0.00529861, 0.00484371, 0.0057478, 0.0074749, 0.0108757},
  {0, 0, 0, 0, 0.00870228, 0.0059433, 0.00690365, 0.00428581, 0.00424576,
   0.0120583, 0.0203514, 0.0223598, 0.0224638},
  {0, 0, 0, 0, 0, 0.00878906, 0.0160666, 0.0151358, 0.0169592, 0.00271988,
   0.0038271, 0.00378036, 0.0053997},
  {0, 0, 0, 0, 0, 0, 0.0100336, 0.0265827, 0.0111609, 0.00276184, 0.0042038,
   0.00437927, 0.00682926},
  {0, 0, 0, 0, 0, 0, 0, 0.00892735, 0.0179653, 0.00785637, 0.00357723,
   0.00368023, 0.00394154},
  {0, 0, 0, 0, 0, 0, 0, 0, 0.00800228, 0.0113173, 0.00733089, 0.00389767,
   0.00456524},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00864506, 0.0234957, 0.0120077, 0.00799084},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0116854, 0.0313263, 0.023016},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0136251, 0.0307255},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0153828}};
const static float default_stat_vs_stat_opp_crib[13][13] = {
  {0.0029335, 0.0184946, 0.0121651, 0.00640774, 0.00148582, 0.0124569,
   0.0138016, 0.0201616, 0.0201998, 0.0178318, 0.0135164, 0.0243034, 0.0270805},
  {0, 0.00273418, 0.00302505, 0.0103493, 0.0018301, 0.0107327, 0.0158634,
   0.018096, 0.0178413, 0.0127363, 0.00943565, 0.0167513, 0.0189104},
  {0, 0, 0.00187778, 0.00573826, 0.000852585, 0.0128613, 0.010726, 0.0139341,
   0.0134163, 0.0115509, 0.00909233, 0.0151405, 0.0171585},
  {0, 0, 0, 0.00158119, 0.000750542, 0.0037365, 0.0120773, 0.0107899, 0.0128088,
   0.0118656, 0.00966454, 0.0162992, 0.0183659},
  {0, 0, 0, 0, 0.000113487, 0.000567436, 0.000887871, 0.00167084, 0.00201225,
   0.000875473, 0.000717163, 0.000752449, 0.0007658},
  {0, 0, 0, 0, 0, 0.00111771, 0.00438976, 0.00933838, 0.00513363, 0.0140743,
   0.00801277, 0.0159092, 0.02176},
  {0, 0, 0, 0, 0, 0, 0.00159454, 0.00238228, 0.0140562, 0.01859, 0.0102701,
   0.0192928, 0.0259924},
  {0, 0, 0, 0, 0, 0, 0, 0.00282097, 0.00977421, 0.00655842, 0.0137911,
   0.0193491, 0.0249863},
  {0, 0, 0, 0, 0, 0, 0, 0, 0.00287819, 0.00488853, 0.00852299, 0.0253763,
   0.0242138},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00213337, 0.00696659, 0.0209045, 0.0477057},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00154686, 0.00809574, 0.0122766},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00313854, 0.0310097},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00335312}};


StatisticsTable::StatisticsTable()
    : prob_table(),
      score_dist_table("Score Distribution Table"),
      mean_table("Mean Table"),
      median_table("Median Table"),
      variance_table("Variance Table"),
      std_dev_table("Standard Deviation Table"),
      max_table("Max Table"),
      min_table("Min Table") {}

void StatisticsTable::load_default_tables() {
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            prob_table.table.get_dealer_crib(i, j) =
              default_stat_vs_stat_my_crib[i][j];
            prob_table.table.get_opp_crib(i, j) =
              default_stat_vs_stat_opp_crib[i][j];
        }
    }
    prob_table.num_sim_discards = default_num_sim_discards;
}

int StatisticsTable::load_tables(std::optional<std::string> table) {
    // TODO: If no dirname, then load default tables
    if (!table) {
        load_default_tables();
        return 1;
    }
    std::string dirname_string = TABLE_DIR + table.value();

    int num_loaded = 0;
    num_loaded += prob_table.table.load(dirname_string + "freq_table.txt");
    num_loaded += mean_table.load(dirname_string + "mean_table.txt");
    num_loaded += median_table.load(dirname_string + "median_table.txt");
    num_loaded += variance_table.load(dirname_string + "variance_table.txt");
    num_loaded += std_dev_table.load(dirname_string + "std_dev_table.txt");
    num_loaded += max_table.load(dirname_string + "max_table.txt");
    num_loaded += min_table.load(dirname_string + "min_table.txt");
    return num_loaded;
}

void StatisticsTable::save_tables(std::string table) {
    prob_table.table.save(table + "freq_table.txt");
    // TODO: save score_dist_table
    mean_table.save(table + "mean_table.txt");
    median_table.save(table + "median_table.txt");
    variance_table.save(table + "variance_table.txt");
    std_dev_table.save(table + "std_dev_table.txt");
    max_table.save(table + "max_table.txt");
    min_table.save(table + "min_table.txt");
}

ScoreDistributionTable StatisticsTable::get_score_dist(Card card1, Card card2,
                                                      bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return score_dist_table.get_dealer_crib(index1, index2);
    } else {
        return score_dist_table.get_opp_crib(index1, index2);
    }
}

float StatisticsTable::get_mean(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return mean_table.get_dealer_crib(index1, index2);
    } else {
        return mean_table.get_opp_crib(index1, index2);
    }
}

int StatisticsTable::get_median(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return median_table.get_dealer_crib(index1, index2);
    } else {
        return median_table.get_opp_crib(index1, index2);
    }
}

float StatisticsTable::get_variance(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return variance_table.get_dealer_crib(index1, index2);
    } else {
        return variance_table.get_opp_crib(index1, index2);
    }
}

float StatisticsTable::get_std_dev(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return std_dev_table.get_dealer_crib(index1, index2);
    } else {
        return std_dev_table.get_opp_crib(index1, index2);
    }
}

int StatisticsTable::get_max(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return max_table.get_dealer_crib(index1, index2);
    } else {
        return max_table.get_opp_crib(index1, index2);
    }
}

int StatisticsTable::get_min(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return min_table.get_dealer_crib(index1, index2);
    } else {
        return min_table.get_opp_crib(index1, index2);
    }
}


}  // namespace cribbage
