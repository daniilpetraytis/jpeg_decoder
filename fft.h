#pragma once

#include <cstddef>
#include <vector>
#include <fftw3.h>

class DctCalculator {
private:
    double *in_;
    double *out_;
    fftw_plan p_;
    size_t width_;

public:
    // input and output are width by width matrices, first row, then
    // the second row.
    DctCalculator(size_t width, std::vector<double> *input, std::vector<double> *output);

    void Inverse();
};
