#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <complex>

#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#include "CL/opencl.hpp"

#include "clblast.h"
#include "cblas.h"

void InitVector(std::vector<float> &v) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    for (int i = 0; i < v.size(); i++) {
        v[i] = distribution(gen);
    }
}

std::vector<float> Compare(const int M, const int N, const float *C, const float *C_ref) {
    float diff, real_max_diff = 0.0f, imag_max_diff = 0.0f;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < 2; k++) {
                float c = C[i * N * 2 + j * 2 + k], c_ref = C_ref[i * N * 2 + j * 2 + k];
                diff = std::abs(c - c_ref);
                if (k == 0) {
                    real_max_diff = diff > real_max_diff ? diff : real_max_diff;
                } else {
                    imag_max_diff = diff > imag_max_diff ? diff : imag_max_diff;
                }
            }
        }
    }
    return std::vector<float>{real_max_diff, imag_max_diff};
}

int main() {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    auto device = devices.front();

    // Create an OpenCL context
    cl::Context context({device});

    // Create a command queue
    cl::CommandQueue queue(context, device);

    int range_start = 100;
    int range_end = 1000;

    int scale = (range_start + range_end) / 2;
    while(scale > range_start && scale < range_end) {
        std::vector<float> A(scale * scale * 2),
                           B(scale * scale * 2),
                           C_Openblas(scale * scale * 2, 0),
                           C_Clblast(scale * scale * 2, 0);

        InitVector(A);
        InitVector(B);

        float alpha[2] = {1.f, 1.f},
              beta[2] = {0.f, 0.f};
        cblas_cgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, scale, scale, scale, alpha, A.data(), scale, B.data(), scale, beta, C_Openblas.data(), scale);

        // OpenCL computation
        cl::Buffer A_Buffer(context, CL_MEM_READ_ONLY, sizeof(float) * A.size()),
                   B_Buffer(context, CL_MEM_READ_ONLY, sizeof(float) * B.size()),
                   C_Clblast_Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * C_Clblast.size());
        queue.enqueueWriteBuffer(A_Buffer, CL_TRUE, 0, A.size() * sizeof(float), A.data());
        queue.enqueueWriteBuffer(B_Buffer, CL_TRUE, 0, B.size() * sizeof(float), B.data());
        queue.enqueueWriteBuffer(C_Clblast_Buffer, CL_TRUE, 0, C_Clblast.size() * sizeof(float), C_Clblast.data());

        auto raw_queue = queue();
        cl::Event event{nullptr};
        auto raw_event = event();
        std::complex<float> cl_alpha{1.f, 1.f},
                            cl_beta{0.f, 0.f};
        auto status = clblast::Gemm(clblast::Layout::kRowMajor, clblast::Transpose::kNo, clblast::Transpose::kNo,
                                    scale, scale, scale,
                                    cl_alpha,
                                    A_Buffer(), 0, scale,
                                    B_Buffer(), 0, scale,
                                    cl_beta,
                                    C_Clblast_Buffer(), 0, scale,
                                    &raw_queue, &raw_event);
        if (status == clblast::StatusCode::kSuccess) {
            clWaitForEvents(1, &raw_event);
            clFinish(raw_queue);
        }

        queue.enqueueReadBuffer(C_Clblast_Buffer, CL_TRUE, 0, C_Clblast.size() * sizeof(float), C_Clblast.data(), nullptr, &event);
        cl::WaitForEvents({event});

        // Compare
        auto max_diff = Compare(scale, scale, C_Clblast.data(), C_Openblas.data());
        int next_scale;
        if (max_diff[0] >= 1e-3 || max_diff[1] >= 1e-3) {
            printf("scale: %d, real_max_diff: %f, imag_max_diff: %f\n", scale, max_diff[0], max_diff[1]);
            range_end = scale;
        } else {
            printf("sacle: %d, OK\n", scale);
            range_start = scale;
        }
        next_scale = (range_start + range_end) / 2;

        if (next_scale == scale) {
            break;
        } else {
            scale = next_scale;
        }
    }

    return 0;
}