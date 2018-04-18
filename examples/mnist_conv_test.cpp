/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include <iomanip>

#include "sp/algo/nn.hpp"
#include "sp/util/dataset.hpp"
#include "sp/util/timing.hpp"
#include "sp/util/progress.hpp"

using namespace sp;
using namespace sp::algo::nn;

/**
 * \brief Read in a BMP
 */
std::vector<float_t> read_bmp(const std::string &file, const valid_range& range) {
    static constexpr size_t HEADER_SIZE = 54;

    auto [min, max] = range;

    std::ifstream bmp(file, std::ios::binary);

    std::array<char, HEADER_SIZE> header;
    bmp.read(header.data(), header.size());

    auto fileSize = *reinterpret_cast<uint32_t *>(&header[2]);
    auto dataOffset = *reinterpret_cast<uint32_t *>(&header[10]);
    auto width = *reinterpret_cast<uint32_t *>(&header[18]);
    auto height = *reinterpret_cast<uint32_t *>(&header[22]);
    //auto depth = *reinterpret_cast<uint16_t *>(&header[28]);

    if(width != 32) {
        throw std::runtime_error("Invalid width + ");
    }
    if(height != 32) {
        throw std::runtime_error("Invalid width");
    }

    std::vector<char> img(fileSize - dataOffset);
    std::vector<float_t> res(height * width, min);

    bmp.read(img.data(), img.size());

    auto dataSize = ((width * 3 + 3) & (~3)) * height;
    img.resize(dataSize);
    bmp.read(img.data(), img.size());

    size_t idx = res.size()-1;
    for (size_t i = dataSize-4; i != static_cast<size_t>(-1); i -= 3, --idx) {
        //reading backward, swap for rgb
        auto temp = img[i];
        img[i] = img[i+2];
        img[i+2] = temp;
        res[idx] = (
            (   static_cast<float_t>(img[i] & 0xff) +
                static_cast<float_t>(img[i+1] & 0xff) +
                static_cast<float_t>(img[i+2] & 0xff)
            ) / (3.0f * 255.0f)
        ) * (max - min) + min;
    }

    return res;
}

int main(int argc, char** argv) {
    /**
     * MNIST NN example, based on LeCun 1998
     */
    using network_def = network<
        conv_layer<
            volume_dims<1, 32, 32>,
            kernel_symmetric_params<6, 5, 1, padding_type::valid>
        >,
        tanh_layer<volume_dims<6, 28, 28>>,
        mean_pooling_layer<volume_dims<6, 28, 28>, pooling_kernel_params<2, 2>>,
        tanh_layer<volume_dims<6, 14, 14>>,
        conv_layer<
            volume_dims<6, 14, 14>,
            kernel_symmetric_params<16, 5, 1, padding_type::valid>,
            true,
            table_connectivity<
                6, 16, //6x16 (6 inputs to 16 outputs) defined as:
                1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1,
                1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1,
                1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1,
                0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1,
                0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1,
                0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1
            >
        >,
        tanh_layer<volume_dims<16, 10, 10>>,
        mean_pooling_layer<
            volume_dims<16, 10, 10>,
            pooling_kernel_params<2, 2>
        >,
        tanh_layer<volume_dims<16, 5, 5>>,
        conv_layer<
            volume_dims<16, 5, 5>,
            kernel_symmetric_params<120, 5, 5>
        >,
        tanh_layer<volume_dims<120, 1, 1>>,
        fully_connected_layer<
            volume_dims<120>,
            10
        >,
        tanh_layer<volume_dims<10>>
    >;

    network_def nn;

    nn.configure(1, false);
    if(!nn.load("leNet.model")) {
        std::cerr << "No leNet model file exists- Please run training first.\n";
    } else {
        if(argc != 2) {
            std::cerr << "No input image specified!\n";
        } else {
            auto vec = read_bmp(argv[1], nn.out_range());
            tensor_4 in(1, 1, 32, 32);
            //convert from bmp vector to input vector
            for(size_t i = 0; i < 32; ++i) {
                for(size_t j = 0; j < 32; ++j) {
                    in(0, 0, i, j) = vec[i * 32 + j];
                    std::cout << std::setw(6) << std::fixed << std::setprecision(3) << in(0, 0, i, j);
                }
                std::cout << "\n";
            }

            auto res = nn.forward_max_index(in);
            std::cout << "---------------------------------------\n";
            std::cout << "File: " << argv[1] << "\n";
            std::cout << "The prediction is: " << res[0] << "\n";
            std::cout << "---------------------------------------\n";
        }
    }
}