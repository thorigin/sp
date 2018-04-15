/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_NN_MATRIX_HPP
#define SP_ALGO_NN_MATRIX_HPP

#include "config.hpp"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

/**
 * \file This file contains common typedefs for use with linear algebra 
 * operations for the use with neural network
 *
 * Currently, Eigen is the choice for a LA library implementation although
 * armadillo is also a candiate. However, due to Tensor support in Eigen it is
 * the currently used library.
 */
SP_ALGO_NN_NAMESPACE_BEGIN

/**
 * Dynamic matrix typedef
 */
using matrix = Eigen::Matrix<float_t, Eigen::Dynamic, Eigen::Dynamic>;

using sparse_matrix = Eigen::SparseMatrix<float_t>;

/**
 * Static matrix typedef
 */
template<size_t rows, size_t cols>
using matrix_fixed = Eigen::Matrix<float_t, rows, cols>;

template<size_t size>
using vector_fixed = Eigen::Matrix<float_t, size, 1>;

using vector = Eigen::Matrix<float_t, Eigen::Dynamic, 1>;

template<typename ... Parameters>
using tensor_base = Eigen::TensorBase<Parameters...>;

template<size_t Rank, typename T = float_t>
using tensor_n = Eigen::Tensor<T, Rank, Eigen::Aligned128 | Eigen::RowMajor>;

template<size_t Rank, typename T = float_t>
using tensor_n_ref = Eigen::TensorRef<tensor_n<Rank, T>>;

template<size_t Rank>
using tensor_map = Eigen::TensorMap<tensor_n<Rank>, Eigen::Aligned128 | Eigen::RowMajor>;

/**
 * Tensor, Rank 5
 */
using tensor_4 = tensor_n<4>;

/**
 * Tensor reference, Rank 5
 */
using tensor_5 = tensor_n<5>;

/**
 * Tensor, Rank 4
 */
using tensor_4 = tensor_n<4>;

/**
 * Tensor reference, Rank 4
 */
using tensor_4_ref = tensor_n_ref<4>;

/**
 * Tensor, Rank 3,
 *
 */
using tensor_3 = tensor_n<3>;

/**
 * Tensor reference, Rank 3
 */
using tensor_3_ref = tensor_n_ref<3>;

/**
 * Tensor, Rank 2
 */
using tensor_2 = tensor_n<2>;

/**
 * Tensor reference, Rank 2
 */
using tensor_2_ref = tensor_n<2>;

/**
 * Tensor, Rank 1
 */
using tensor_1 = tensor_n<1>;

/**
 * Tensor reference, Rank 1
 */
using tensor_1_ref = tensor_n<1>;

/**
 * Tensor, Rank 0
 */
using tensor_0 = tensor_n<0>;

/**
 * Tensor reference, Rank 0
 */
using tensor_0_ref = tensor_n_ref<0>;

/**
 * \brief Convert a tensor (rank 2) to a matrix
 */
template<typename ... Parameters>
auto as_mat(tensor_2& tensor) {
    return Eigen::Map<matrix>(
        tensor.data(),
        tensor.dimension(0),
        tensor.dimension(1)
    );
}

/**
 * Map a matrix as a tensor
 * \param mat
 * \return
 */
auto as_tensor(matrix& mat) {
    return tensor_map<2>(
        mat.data(),
        mat.rows(),
        mat.cols()
    );
}


SP_ALGO_NN_NAMESPACE_END

#endif	/* SP_ALGO_NN_MATRIX_HPP */


