/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
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

struct mnist_data_reader  {

    mnist_data_reader(const std::string& image_file, const std::string& labels_file)
        : image_stream(), label_stream(), image_count(0), image_height(), image_width() {

        //read file
        fs::path image_path(image_file);
        fs::path labels_path(labels_file);

        if(fs::exists(image_path) && fs::exists(labels_path)) {

            //Read in header and initialize member variables

            image_stream.open(image_path, fs::fstream::in | fs::fstream::binary);
            label_stream.open(labels_path, fs::fstream::in | fs::fstream::binary);
            if(!image_stream.good() || !label_stream.good()) {
                throw std::runtime_error("Image or label not opening");
            }

            auto magic_num_images = read_stream<int32_t>(image_stream);
            auto magic_num_labels = read_stream<int32_t>(label_stream);

            if(magic_num_images != 2051) {
                throw std::runtime_error("Image magic failure");
            }

            if(magic_num_labels != 2049) {
                throw std::runtime_error("Labels magic failure");
            }

            image_count = read_stream<int32_t>(image_stream);
            image_height = read_stream<int32_t>(image_stream);
            image_width = read_stream<int32_t>(image_stream);

            if(image_stream.bad()) {
                throw std::runtime_error("Failed reading header");
            }

        } else {
            throw std::runtime_error("Image or label file missing");
        }
    }


    size_t size() {
        return image_count;
    }

    virtual ~mnist_data_reader() {
        image_stream.close();
        label_stream.close();
    }

private:

    template<typename T>
    T read_stream(fs::fstream& stream) {
        T val;
        stream.read ((char*)&val, sizeof (T));
        return boost::endian::big_to_native(val);
    }

    fs::fstream image_stream;
    fs::fstream label_stream;
    uint32_t image_count;
    uint32_t image_height;
    uint32_t image_width;

};

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_DATASET_MNIST_DATA_READER_HPP */