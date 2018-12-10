#pragma once
#include <vector>
#include <iostream>
#include <random>
#include <functional>
#include <chrono>
#include <string>

using func_type = std::vector<std::vector<double>>(const std::vector<std::vector<double>>& a,
                                                   const std::vector<std::vector<double>>& b);

static void print_matrix(std::vector<std::vector<double>> m)
{
	for (const auto& row : m)
	{
		for (const auto& val : row)
		{
			std::cout << val << ' ';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

static auto init_matrix(const int size)
{
	std::mt19937 rng{std::random_device()()};
	const std::uniform_real_distribution<double> rand{-10.0, 10.0};

	auto matrix = std::vector<std::vector<double>>(size, std::vector<double>(size, 0.0));
	for (auto& row : matrix)
	{
		for (auto& val : row)
		{
			val = rand(rng);
		}
	}

	return matrix;
}


static auto transpose(const std::vector<std::vector<double>>& vec)
{
	const auto rows = vec.size();
	if (rows == 0)
		return vec;
	const auto cols = vec[0].size();

	auto t_vec = std::vector<std::vector<double>>(cols, std::vector<double>(rows, 0.0));
	for (auto i = 0u; i < rows; i++)
	{
		for (auto j = 0u; j < cols; j++)
		{
			t_vec[j][i] = vec[i][j];
		}
	}

	return t_vec;
}

static auto test(const std::string& name, const std::function<func_type>& func, const int size)
{
	const auto matrix_a = init_matrix(size);
	const auto matrix_b = init_matrix(size);

	const auto start = std::chrono::high_resolution_clock::now();
	const auto matrix_c = func(matrix_a, matrix_b);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration<double>(end - start).count();
	std::cout << size << ", " << name << ": " << duration << " s.\n";
	return std::chrono::duration<double>(end - start).count();
}


static bool compare(const std::vector<std::vector<double>>& m,
                    const std::string& name, std::function<func_type>& func,
                    const std::vector<std::vector<double>>& a,
                    const std::vector<std::vector<double>>& b)
{
	const auto c = func(a, b);

	auto max_delta = 0.0;
	for (auto i = 0u; i < m.size(); i++)
	{
		for (auto j = 0u; j < m[0].size(); j++)
		{
			max_delta = std::max(max_delta, std::abs(m[i][j] - c[i][j]));
		}
	}

	const auto eps = 1e-11;
	if (abs(max_delta) > eps)
	{
		std::cout << name << " max delta = " << max_delta << '\n';
		return false;
	}
	return true;
}
