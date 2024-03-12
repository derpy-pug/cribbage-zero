#include "statistics/generator/DiscardHeuristics.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace cribbage {

GenerateDiscardHeuristics::GenerateDiscardHeuristics(const AllDiscardStatistics& all_discard_stats)
    : all_discard_stats(all_discard_stats) {}

void zip_sort(std::vector<float>& a, std::vector<std::pair<Card,Card>>& b) {
    std::vector<size_t> indices(a.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort the indices based on the values in mainVector
    std::sort(indices.begin(), indices.end(),
              [&a](size_t i1, size_t i2) { return a[i1] < a[i2]; });

    // Create temporary vectors to hold the sorted elements
    std::vector<float> sortedMain(a.size());
    std::vector<std::pair<Card,Card>> sortedSecondary(a.size());

    // Apply the sorted order to both vectors
    for (size_t i = 0; i < indices.size(); ++i) {
        sortedMain[i] = a[indices[i]];
        sortedSecondary[i] = b[indices[i]];
    }

    a = std::move(sortedMain);
    b = std::move(sortedSecondary);
}

std::pair<float, float> get_max_min(const std::vector<float>& a) {
    auto [min, max] = std::minmax_element(a.begin(), a.end());
    return std::make_pair(*min, *max);
}

float mean_arc_calc(float x, float values_range, float arc) {
    float part1 = x * (1 - std::exp(-(arc * values_range / x)));
    float part2 = std::exp(-arc * values_range);
    return part1 + part2;
}

DiscardHeuristics GenerateDiscardHeuristics::generate_discard_heuristics(ScoreType score_type, Statistic stat) const {
    const std::vector<DiscardStatistics>& discard_stats = all_discard_stats.get_all_discard_stats();

    DiscardHeuristics discard_heuristics;
    auto& values = discard_heuristics.heuristics;
    auto& discards = discard_heuristics.discards;
    values.reserve(discard_stats.size());
    discards.reserve(discard_stats.size());

    for (auto it = discard_stats.cbegin(); it != discard_stats.cend(); ++it) {
        discards.push_back(std::make_pair((*it).get_discard1(), (*it).get_discard2()));
    }
    for (auto it = discard_stats.cbegin(); it != discard_stats.cend(); ++it) {
        float discard_stat_value = (*it).get_value(score_type, stat);
        values.push_back(discard_stat_value);
    }
    // If all the values are the same, return 1.0 for all. Next steps fail if all values are the same
    if (std::adjacent_find(values.begin(), values.end(), std::not_equal_to<float>()) == values.end()) {
        discard_heuristics.heuristics = std::vector<float>(values.size(), 1.0);
        return discard_heuristics;
    }

    // Get the mean and standard deviation of the values to calculate the z stat
    float mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    std::transform(values.begin(), values.end(), values.begin(),
                   [mean](float x) { return x - mean; });
    float standard_deviation = std::sqrt(
      std::inner_product(values.begin(), values.end(), values.begin(), 0.0) /
      values.size());
    std::transform(values.begin(), values.end(), values.begin(),
                   [mean](float x) { return x + mean; });

    // Calc the z stat of the all values 
    std::vector<float> z_stats;
    z_stats.reserve(values.size());
    std::transform(values.begin(), values.end(), std::back_inserter(z_stats),
                   [mean, standard_deviation](float x) {
                       return (x - mean) / standard_deviation;
                   });

    // Sort z_stats and values by z_stats
    zip_sort(z_stats, discards);

    // Calc the z percentage of all the values
    float z_range = z_stats.back() - z_stats.front();
    std::transform(z_stats.begin(), z_stats.end(), z_stats.begin(),
                   [z_range, z_stats](float x) {
                       return (x - z_stats.front()) / z_range;
                   });


    /*
     * Then depending on the values range, we can adjust the z_percent to be more or less important
     * When the range is small, the z_percent should get non-linearly shifted towards 1.0
     * When the range gets larger, the z_percent should get closer to itself
     *
     */

    std::pair<float, float> max_min = get_max_min(values);
    float values_range = max_min.second - max_min.first;

    std::transform(z_stats.begin(), z_stats.end(), z_stats.begin(),
                   [values_range](float x) {
                          return mean_arc_calc(x, values_range, 0.20);
                   });

    /* for (auto it = z_stats.begin(); it != z_stats.end(); ++it) { */
    /*     std::cout << *it << std::endl; */
    /* } */
    

    discard_heuristics.heuristics = z_stats;
    return discard_heuristics;
}

} // namespace cribbage
