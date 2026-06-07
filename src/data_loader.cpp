#include "data_loader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <map>

std::vector<MaturitySlice> load_data(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("load_data: cannot open file " + filepath);

    // map groups rows by maturity and keeps them sorted
    std::map<double, std::pair<std::vector<double>, std::vector<double>>> groups;

    std::string line;

    std::getline(file, line); 
    
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;

        std::stringstream ss(line);
        std::string token;

        double maturity, strike, iv;

        std::getline(ss, token, ',');
        maturity = std::stod(token);

        std::getline(ss, token, ',');
        strike = std::stod(token);

        std::getline(ss, token, ',');
        iv = std::stod(token);

        groups[maturity].first.push_back(strike);
        groups[maturity].second.push_back(iv);
    }

    file.close();

    if (groups.empty())
        throw std::runtime_error("load_data: no data found in file");

    std::vector<MaturitySlice> slices;

    for (auto& entry : groups)
    {
        double maturity = entry.first;
        auto& data = entry.second;

        MaturitySlice slice;
        slice.maturity = maturity;
        slice.strikes  = data.first;
        slice.ivs      = data.second;

        int n = slice.strikes.size();

        // sort strikes because the spline needs increasing x
        // index array keeps strikes and ivs aligned
        std::vector<int> idx(n);
        for (int i = 0; i < n; ++i) idx[i] = i;

        std::sort(idx.begin(), idx.end(), [&](int a, int b){
            return slice.strikes[a] < slice.strikes[b];
        });

        std::vector<double> sorted_strikes(n);
        std::vector<double> sorted_ivs(n);
        for (int i = 0; i < n; ++i)
        {
            sorted_strikes[i] = slice.strikes[idx[i]];
            sorted_ivs[i]     = slice.ivs[idx[i]];
        }

        slice.strikes = sorted_strikes;
        slice.ivs     = sorted_ivs;

        slices.push_back(slice);
    }

    return slices;
}
