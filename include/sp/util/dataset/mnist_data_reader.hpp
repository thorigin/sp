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
    
    const static size_t default_samples_read = 1;
    
    
    using vec_t = std::vector<uint8_t>;
    using mat_t = std::vector<vec_t>;
    using tensor_t = std::vector<mat_t>;
    
    using result_type = tensor_t;
    
    mnist_data_reader(const std::string& image_file, const std::string& labels_file, size_t batch_size = default_samples_read)
        : image_stream(), label_stream(), image_count(0), image_height(), image_width(), batch_size(batch_size), current_index(0) {
      
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

            if(image_count % batch_size != 0) {
                throw std::runtime_error("Batch size should be a factor of sample size");
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
    
    size_t frame_size() {
        return batch_size;
    }
    
    size_t frame_width() {
        return image_width;
    }
    
    size_t frame_height() {
        return image_height;
    }
    
    bool has_next_frame() {
        return current_index < image_count;
    }

    result_type read_frame() {
        //batch_sized
        result_type result;
        for(size_t si = 0; si < batch_size; ++si) {
            mat_t m;
            m.reserve(image_height);
            for (size_t hi = 0; hi < image_height; hi++) {
                vec_t v;
                v.reserve(image_width);
                for (size_t wi = 0; wi < image_width; wi++) {
                    v.emplace_back(read_stream<int32_t>(image_stream));
                }
                m.emplace_back(std::move(v));
            }
            result.emplace_back(std::move(m));
        }
        current_index += batch_size;
        return result;
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
    size_t batch_size;
    size_t current_index;

};

SP_UTIL_NAMESPACE_END


#endif /* SP_UTIL_DATASET_MNIST_DATA_READER_HPP */