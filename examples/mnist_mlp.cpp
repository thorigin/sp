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

int main(int argc, char** argv) {
    /**
     * MNIST NN example, based on LeCun 1998
     */

    using network_def = network<
        fully_connected_layer<
            volume_dims<1, 32, 32>,
            256
        >,
        tanh_layer<volume_dims<256>>,
        fully_connected_layer<
            volume_dims<64>,
            64
        >,
        tanh_layer<volume_dims<64>>,
        fully_connected_layer<
            volume_dims<64>,
            10
        >,
        tanh_layer<volume_dims<10>>
    >;

    network_def nn;

    class_vector_type  train_labels, test_labels;
    sample_vector_type train_images, test_images;

    util::load_mnist("resources/mnist",
        train_images, train_labels,
        test_images, test_labels,
        {-1.0f, 1.0f},
        2, 2
    );

    constexpr size_t batch_size = 15;

    const size_t batch_count = train_images.size()/batch_size;
    training<batch_size, 500, ada_gradient_optimizer<std::ratio<4, 100>>, mean_square_error> trainer;

    util::progress prog(batch_count);

    trainer.on_epoch = [&](const auto& e) {
        std::cout << "\nEpoch " << e << "\n";
    };

    trainer.on_batch = [&](const auto& batch) {
        prog.update(batch);
        std::cout << prog;
    };

    std::cout << "\nStarting learning...\n";
    {
        util::scoped_timer doing_tests("Learning time");
        trainer(nn, train_images, train_labels);
    }

    {
        util::scoped_timer doing_tests("Testing time");
        auto[correct, total, cm] = nn.test(test_images, test_labels);
        (void)cm;
        std::cout << "\n\nTotal correct: " << correct << " / " << total << " (" << (static_cast<float>(correct) / total)  << ")\n";
    }
}