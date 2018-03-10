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

#include <vector>
#include <utility>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/endian/conversion.hpp>
#include <array>
#include <exception>
#include <vector>

SP_UTIL_NAMESPACE_BEGIN

namespace fs = boost::filesystem;

using mnist_images_result = std::vector<std::vector<std::vector<uint8_t>>>;
using mnist_labels_result = std::vector<uint8_t>;

struct mnist_data_reader  {

    const static size_t default_samples_read = std::numeric_limits<size_t>::max();

    mnist_data_reader(const std::string& images_file, const std::string& labels_file, size_t batch = default_samples_read)
        : images_stream(), labels_stream(), count(0),
          img_height(), img_width(), batch_size(batch),
          current_images_index(0), current_labels_index(0) {

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
        //batch_sized
        mnist_images_result result;
        for(size_t si = 0; si < batch_size; ++si) {
            mnist_images_result::value_type m;
            m.reserve(img_height);
            for (size_t hi = 0; hi < img_height; hi++) {
                mnist_images_result::value_type::value_type v;
                v.reserve(img_width);
                for (size_t wi = 0; wi < img_width; wi++) {
                    v.emplace_back(read_stream<int32_t>(images_stream));
                }
                m.emplace_back(std::move(v));
            }
            result.emplace_back(std::move(m));
        }
        current_images_index += batch_size;
        return result;
    }

    mnist_labels_result read_labels() {
        //batch_sized
        mnist_labels_result result;
        result.reserve(batch_size);
        for(size_t si = 0; si < batch_size; ++si) {
            result.emplace_back(read_stream<uint8_t>(labels_stream));
        }
        current_labels_index += batch_size;
        return result;
    }

private:

    template<typename T>
    T read_stream(fs::fstream& stream) {
        T val;
        stream.read ((char*)&val, sizeof (T));
        return boost::endian::big_to_native(val);
    }

    fs::fstream images_stream;
    fs::fstream labels_stream;
    size_t count;
    size_t img_height;
    size_t img_width;
    size_t batch_size;
    size_t current_images_index;
    size_t current_labels_index;

};

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_DATASET_MNIST_DATA_READER_HPP */