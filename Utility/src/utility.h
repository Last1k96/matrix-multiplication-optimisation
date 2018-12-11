#pragma once
#include <vector>
#include <iostream>
#include <random>
#include <functional>
#include <chrono>
#include <string>

static auto mult_tr(const std::vector<std::vector<double>>& a,
                    const std::vector<std::vector<double>>& b)
{
	const auto size = static_cast<int>(a.size());

	auto c = std::vector<std::vector<double>>(size, std::vector<double>(size, 0.0));

#pragma omp parallel for
	for (auto i = 0; i < size; i++)
	{
		for (auto j = 0; j < size; j++)
		{
			auto sum = 0.0;
			for (auto k = 0; k < size; k++)
			{
				sum += a[i][k] * b[j][k];
			}
			c[i][j] = sum;
		}
	}

	return c;
}

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

static double compare(const std::vector<std::vector<double>>& a,
                      const std::vector<std::vector<double>>& b)
{
	const auto eps = 1e-11;

	for (auto i = 0u; i < a.size(); i++)
	{
		for (auto j = 0u; j < a[0].size(); j++)
		{
			const auto delta = std::abs(a[i][j] - b[i][j]);
			if (delta > eps)
			{
				return delta;
			}
		}
	}

	return 0.0;
}

static bool is_correct(const std::string& name,
                       const std::function<decltype(mult_tr)>& func,
                       const bool transposed)
{
	const auto a = init_matrix(256);
	const auto b = init_matrix(256);
	const auto bt = transpose(b);
	const auto c = mult_tr(a, bt);
	const auto d = func(a, transposed ? bt : b);

	const auto delta = compare(c, d);
	if (delta != 0.0)
	{
		std::cout << name << " works incorrectly: max delta = " << delta << '\n';
		return false;
	}
	return true;
}

static auto test(const std::string& name, const std::function<decltype(mult_tr)>& func, const int size,
                 const bool transposed)
{
	if (!is_correct(name, func, transposed))
	{
		return 0.0;
	}

	const auto matrix_a = init_matrix(size);
	const auto matrix_b = init_matrix(size);

	const auto start = std::chrono::high_resolution_clock::now();
	const auto matrix_c = func(matrix_a, matrix_b);
	const auto end = std::chrono::high_resolution_clock::now();

	const auto duration = std::chrono::duration<double>(end - start).count();
	std::cout << size << ", " << name << ": " << duration << " s.\n";
	return std::chrono::duration<double>(end - start).count();
}
