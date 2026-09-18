//
// Created by tevin on 11/09/2026.
//

#include "multipoint.hpp"

#include <random>

multipoint::points multipoint::generate_points(const uint32_t amount, const float min, const float max)
{
	std::vector<float> points;
	points.reserve(amount);

	static std::default_random_engine engine;
	std::uniform_real_distribution<> dis(min, max);

	for (uint32_t i = 0; i < amount; i++)
		points.emplace_back(dis(engine));

	return points;
}
