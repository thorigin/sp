# Project Sparring

Experimental Convolutional Neural Network (CNN) and Genetic Algorithms (GA) framwework.

## Getting Started

This framework is a work in progress and largely experimental.

### Prerequisites

Libraries:

* BOOST 1.62+ (libboost-all-dev)
* OpenMP 3.x+ (optional)
* GCC 7.1+ (required)
* Make 4.1+ (required)
* Doxygen 1.8.13+ (optional)

Installation in Debian based distros:

apt-get update && apt-get upgrade && apt-get install build-essential libboost-all-dev

### Installing

The library is header only. No installation is necessary aside from the dependencies mentioned above.

## Running tests

To run the complete test suite execute the following commands:

```
make
./run_tests.sh
```

## Example Usage

### Definition of MNIST

The following defines the network definition for MNIST (LeCun 1998)

```C++
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
```

- Code snippet from [examples/mnist_conv_train.cpp](https://github.com/thorigin/sp/blob/master/examples/mnist_conv_train.cpp)


The output of the MNIST traninig sample (./build/examples/mnist_conv_train.bin) when run:

```
$ ./build/examples/mnist_conv_train.bin
The learning rate is: 0.0387298

Initial accuracy: 940 / 10000 (9.4%)


Starting learning... (total of 4000 batches)
[==================================================] 100%
Epoch 1 Finished. Accuracy: 9382 / 10000 (93.82%)
Persisting model...Done!

[==================================================] 100%
Epoch 2 Finished. Accuracy: 9553 / 10000 (95.53%)
Persisting model...Done!

[==================================================] 100%
Epoch 3 Finished. Accuracy: 9608 / 10000 (96.08%)
Persisting model...Done!

```

### Max Ones Genetic Algorithm

The following is a sample of Max-Ones GA algorithm:

```C++
struct binary_evaluator {
    float operator()(const dna& d) const {
        size_t count = 0;
        size_t total = 0;
        for(size_t ch_idx = 0, ch_len = d.size(); ch_idx < ch_len; ++ch_idx) {
            const chromosome ch = d[ch_idx];
            for(size_t bit_idx = 0, bit_len = ch.size(); bit_idx < bit_len; ++bit_idx) {
                count += ch[bit_idx];
            }
            total += ch.size();
        }
        return static_cast<float>(count) / static_cast<float>(total);
    }
};

simple_meiosis_model<binary_evaluator> m;

auto p = m.new_island()->new_pop();

//Seed the population with 1024 DNAs, containing 1 chromosome of size 1024
m.seed(units::kilo(1), {units::kilo(1)}, p);

m.evolve(p,
    [](auto& ctx) { return ctx.highest_eval() >= 0.999f; }
);
```

Note that a DNA represents an individual where it can have one or more chromosomes.

- Code snippet from [test_genetic.cpp](https://github.com/thorigin/sp/blob/master/test/test_genetic.cpp)

## Versioning

Code being is still undergoing major refactoring and as such the API is not stable.

## Author

Omar Thor <omar@thorigin.com>

# License

Copyright (c) 2017 Omar Thor <omar@thorigin.com>

All rights reserved. No warranty, explicit or implicit, provided.

Note that I am working on selecting the appropriate OSS license and am welcome to suggestions.

## Versioning

For future use, the use of [SemVer](http://semver.org/) will be used for versioning. Once a stable API is available, look for [tags on this repository](https://github.com/thorigin/sp/tags).
