/**
 * Copyright (C) Omar Thor <omarthoro@gmail.com> - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Omar Thor <omarthoro@gmail.com>, 2017
 */

#ifndef SP_ALGO_GEN_MULTIPLICATION_UNIFORM_MUTATION_RATE_HPP
#define	SP_ALGO_GEN_MULTIPLICATION_UNIFORM_MUTATION_RATE_HPP


SP_ALGO_GEN_NAMESPACE_BEGIN

/**
 * \brief Uniform mutation rate
 *
 * Uniform probability of
 *
 * Note: Currently the only mutation rate implementation
 */
template<std::intmax_t N, std::intmax_t D>
struct uniform_mutation_rate : mutation_rate<uniform_mutation_rate<N, D>> {

    static constexpr float mutation_rate = static_cast<float>(N) / static_cast<float>(D);
    static_assert(mutation_rate >= 0.0f && mutation_rate <= 1.0f, "Mutation rate must be in the range [0, 1]");

    template<typename Model>
    float operator()(Model* m, dna* d) {
        return mutation_rate;
    }

};

SP_ALGO_GEN_NAMESPACE_END


#endif	/* SP_ALGO_GEN_MULTIPLICATION_UNIFORM_MUTATION_RATE_HPP */

