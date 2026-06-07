#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <vector>
#include <string>

struct MaturitySlice
{
    double maturity;              // days to expiry
    std::vector<double> strikes;  // sorted ascending
    std::vector<double> ivs;      // implied vols, same order as strikes
};

std::vector<MaturitySlice> load_data(const std::string& filepath);

#endif
