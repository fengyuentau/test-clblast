#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

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

float Compare(const int M, const int N, const float *C, const float *C_ref) {
    float diff, max_diff = 0.0f;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            float c = C[i * N + j], c_ref = C_ref[i * N + j];
            diff = std::abs(c - c_ref);
            max_diff = diff > max_diff ? diff : max_diff;
        }
    }
    return max_diff;
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

    int range_start = 1000;
    int range_end = 3200;

    int scale = (range_start + range_end) / 2;
    while(scale > range_start && scale < range_end) {
        std::vector<float> A(scale * scale),
                           B(scale * scale),
                           C_Openblas(scale * scale, 0),
                           C_Clblast(scale * scale, 0);

        InitVector(A);
        InitVector(B);

        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, scale, scale, scale, 1.f, A.data(), scale, B.data(), scale, 0.f, C_Openblas.data(), scale);

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
        auto status = clblast::Gemm(clblast::Layout::kRowMajor, clblast::Transpose::kNo, clblast::Transpose::kNo,
                                    scale, scale, scale,
                                    1.f,
                                    A_Buffer(), 0, scale,
                                    B_Buffer(), 0, scale,
                                    0.f,
                                    C_Clblast_Buffer(), 0, scale,
                                    &raw_queue, &raw_event);
        if (status == clblast::StatusCode::kSuccess) {
            clWaitForEvents(1, &raw_event);
            clFinish(raw_queue);
        }

        queue.enqueueReadBuffer(C_Clblast_Buffer, CL_TRUE, 0, C_Clblast.size() * sizeof(float), C_Clblast.data(), nullptr, &event);
        cl::WaitForEvents({event});

        // Compare
        float max_diff = max_diff = Compare(scale, scale, C_Clblast.data(), C_Openblas.data());
        int next_scale;
        if (max_diff >= 1e-3) {
            printf("scale: %d, max_diff: %f\n", scale, max_diff);
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