#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include "data_loader.h"
#include "cubic_spline.h"

int main()
{
    const std::string DATA_FILE = "data/options_data.csv";
    const double K_STAR = 207.0;
    const double T_STAR = 130.0;

    std::vector<MaturitySlice> slices;
    try {
        slices = load_data(DATA_FILE);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // step 1: interpolate in strike direction for each maturity
    std::vector<double> maturities;
    std::vector<double> v;

    for (const auto& s : slices)
    {
        CubicSpline sp;
        sp.build(s.strikes, s.ivs);
        maturities.push_back(s.maturity);
        v.push_back(sp.eval(K_STAR));
    }

    std::cout << "Step 1 results:" << std::endl;
    for (int j = 0; j < (int)maturities.size(); ++j)
        std::cout << "  T=" << maturities[j] << " | v="
                  << std::fixed << std::setprecision(6) << v[j]
                  << " (" << v[j]*100.0 << "%)" << std::endl;

    // step 2: interpolate in maturity direction using step 1 results
    CubicSpline sp_T;
    sp_T.build(maturities, v);
    double result = sp_T.eval(T_STAR);

    std::cout << "\nsigma_IV(T=" << T_STAR << ", K=" << K_STAR << ") = "
              << std::fixed << std::setprecision(6) << result
              << " (" << result*100.0 << "%)" << std::endl;

    std::ofstream out("output/result.txt");
    out << "T* = " << T_STAR << ", K* = " << K_STAR << "\n\n";
    out << "Step 1:\n";
    for (int j = 0; j < (int)maturities.size(); ++j)
        out << "  T=" << maturities[j] << " v=" << std::fixed
            << std::setprecision(6) << v[j] << "\n";
    out << "\nsigma_IV = " << result << "\n";
    out << "sigma_IV = " << result*100.0 << "%\n";
    out.close();

    return 0;
}
