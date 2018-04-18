/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#include <iostream>
#include <iomanip>
#include <iterator>

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
    const size_t epoch_count = 30;

    training<batch_size, epoch_count, ada_gradient_optimizer<>, mean_square_error> trainer;

    trainer.optimizer.alpha = 0.01f * std::min<float_t>(4.0f, std::sqrt(batch_size));
    util::progress prog(batch_count);

    std::cout << "The learning rate is: " << trainer.optimizer.alpha  << "\n\n";

    bool reset_network = true;
    if(nn.load("leNet.model")) {
        std::cout << "Loading existing leNet.model...\n";
        reset_network = false;
    }
    nn.configure(batch_size, reset_network);

    auto[correct, total, cm] = nn.test(test_images, test_labels);
    (void)cm;
    std::cout << "Initial accuracy: " << correct << " / " << total << " (" << (static_cast<float>(correct) / total * 100)  << "%)\n\n";

    trainer.on_epoch = [&](const auto& e) {
        std::cout << "\nEpoch " << (e + 1) << " Finished. ";
        auto[correct, total, cm] = nn.test(test_images, test_labels);
        (void)cm;
        std::cout << "Accuracy: " << correct << " / " << total << " (" << (static_cast<float>(correct) / total * 100)  << "%)\n";

        std::cout << "Persisting model...";
        nn.save("leNet.model");
        std::cout << "Done!\n\n";
    };

    trainer.on_batch = [&](const auto& batch) {
        prog.update(batch + 1);
        std::cout << prog;
    };
    std::cout << "\nStarting learning... (total of " << batch_count << " batches)\n";

    {
        util::scoped_timer doing_tests("Learning time");
        trainer(nn, train_images, train_labels, false);
    }

    {
        util::scoped_timer doing_tests("Testing time");
        auto[correct, total, cm] = nn.test(test_images, test_labels);
        (void)cm;
        std::cout << "\n\nTotal correct: " << correct << " / " << total << " (" << (static_cast<float>(correct) / total * 100)  << "%)\n";
    }

}