#include "fft.h"

#include <fftw3.h>
#include <iostream>
#include <cmath>

DctCalculator::DctCalculator(size_t width, std::vector<double> *input,
                             std::vector<double> *output) {
    (void)width;
    (void)input;
    (void)output;
    if (width * width != input->size() || input->size() != output->size()) {
        throw std::invalid_argument("");
    }
    in_ = input->data();
    out_ = output->data();
    width_ = width;
    p_ = fftw_plan_r2r_2d(width_, width_, in_, out_, FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
}

void DctCalculator::Inverse() {
    for (size_t i = 0; i != width_; ++i) {
        (in_)[i] *= pow(2, 0.5);
    }
    for (size_t i = 0; i < width_ * width_; i += width_) {
        in_[i] *= pow(2, 0.5);
    }

    fftw_execute(p_);

    for (size_t i = 0; i < width_ * width_; ++i) {
        out_[i] /= 16;
    }
    fftw_destroy_plan(p_);
}
