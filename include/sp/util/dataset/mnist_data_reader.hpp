/**
 * Copyright (C). All Rights Reserved.
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * Proprietary and confidential.
 *
 * Written by
 * - Aurora Hernandez <aurora@aurorahernandez.com>, 2018
 * - Jacob Escobedo <jacob@jmesco.com>, 2018
 */

#ifndef SP_UTIL_DATASET_MNIST_DATA_READER_HPP
#define SP_UTIL_DATASET_MNIST_DATA_READER_HPP

#include <limits>
#include <exception>
#include <boost/filesystem.hpp>
#include <boost/endian/conversion.hpp>

#include "sp/algo/nn/types.hpp"


SP_UTIL_NAMESPACE_BEGIN

using mnist_images_result = sp::algo::nn::sample_vector_type;
using mnist_labels_result = sp::algo::nn::class_vector_type;

struct mnist_data_reader  {

    using scaling_range = sp::algo::nn::valid_range;

    mnist_data_reader(      const std::string& images_file,
                            const std::string& labels_file,
                            scaling_range scale = {-1.0f, 1.0f},
                            size_t pad_h_ = 0,
                            size_t pad_w_ = 0,
                            size_t batch = std::numeric_limits<size_t>::max())
        : images_stream(), labels_stream(), count(0),
          img_height(), img_width(),
          current_images_index(0), current_labels_index(0),
          scaling(scale),
          pad_h(pad_h_), pad_w(pad_w_),
          batch_size(batch) {

        namespace fs = boost::filesystem;

        //read file
        fs::path images_path(images_file);
        fs::path labels_path(labels_file);

        if(fs::exists(images_path) && fs::exists(labels_path)) {

            //Read in header and initialize member variables

            images_stream.open(images_path, fs::fstream::in | fs::fstream::binary);
            labels_stream.open(labels_path, fs::fstream::in | fs::fstream::binary);

            if(!images_stream.good() || !labels_stream.good()) {
                throw std::runtime_error("Image or label not opening");
            }

            auto magic_num_images = read_stream<int32_t>(images_stream);
            auto magic_num_labels = read_stream<int32_t>(labels_stream);

            if(magic_num_images != 2051) {
                throw std::runtime_error("Image magic failure");
            }

            if(magic_num_labels != 2049) {
                throw std::runtime_error("Labels magic failure");
            }

            size_t labels_count = read_stream<int32_t>(labels_stream);

            count = read_stream<int32_t>(images_stream);
            img_height = read_stream<int32_t>(images_stream);
            img_width = read_stream<int32_t>(images_stream);

            if(count != labels_count) {
                throw std::runtime_error("Image and labels count do not match");
            }

            if(images_stream.bad()) {
                throw std::runtime_error("Failed reading header");
            }

            if(count < batch) {
                batch_size = count;
            }

            if(count % batch_size != 0) {
                throw std::runtime_error("Batch size should be a factor of sample size");
            }

        } else {
            throw std::runtime_error("Image or label file missing");
        }
    }

    size_t size() {
        return count;
    }

    virtual ~mnist_data_reader() {
        images_stream.close();
        labels_stream.close();
    }

    size_t frame_size() {
        return batch_size;
    }

    size_t image_width() {
        return img_width;
    }

    size_t image_height() {
        return img_height;
    }

    bool has_images() {
        return current_images_index < count;
    }

    bool has_labels() {
        return current_labels_index < count;
    }

    mnist_images_result read_images() {
        /* Initialize result to a vector of batch_size */
        mnist_images_result result(batch_size,
            mnist_images_result::value_type(
                1,
                img_height + 2 * pad_h,
                img_width + 2 * pad_w
            )
        );

        using namespace sp::algo::nn;

        /* raw input bufffer */
        std::vector<uint8_t> buff(img_height * img_width);

        auto &[ min, max] = scaling;

        for(size_t b = 0; b < batch_size; ++b) {
            if(pad_w || pad_h) {
                /* initialize whole image to scale minimum */
                result[b].setConstant(min);
            }
            /* read bytes into buff */
            images_stream.read(reinterpret_cast<char*>(&buff[0]), img_height * img_width);

            /* transform each ubyte into a float given the range [min,max] */
            for(size_t row = 0; row < img_height; ++row) {
                /* for every row, transform values */
                std::transform(
                    &buff[row * img_width],
                    &buff[(row+1) * img_width],
                    &result[b](0, row+pad_h, pad_w),
                    [&](const uint8_t& val) {
                        return (static_cast<float_t>(val) / 255.0f) * (max - min) + min;
                    }
                );
            }
        }
        current_images_index += batch_size;
        return result;
    }

    mnist_labels_result read_labels() {
        mnist_labels_result result(batch_size, 0);
        for(size_t b = 0; b < batch_size; ++b) {
            uint8_t label;
            labels_stream.read(reinterpret_cast<char*>(&label), 1);
            result[b] = static_cast<size_t>(label);
        }
        current_labels_index += batch_size;
        return result;
    }

private:

    template<typename T>
    T read_stream(boost::filesystem::fstream& stream) {
        T val;
        stream.read (reinterpret_cast<char*>(&val), sizeof (T));
        return boost::endian::big_to_native(val);
    }

    boost::filesystem::fstream images_stream;
    boost::filesystem::fstream labels_stream;
    size_t count;
    size_t img_height;
    size_t img_width;
    size_t current_images_index;
    size_t current_labels_index;
    scaling_range scaling;
    size_t pad_h;
    size_t pad_w;
    size_t batch_size;
};

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_DATASET_MNIST_DATA_READER_HPP */