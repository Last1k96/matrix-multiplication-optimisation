#pragma once
#include <vector>

class tile
{
	std::vector<std::vector<double>>* data_ = nullptr;
	int row_ = 0;
	int col_ = 0;

public:
	tile(std::vector<std::vector<double>>* m, const int row, const int col, const int size)
		: data_(m), row_(row * size), col_(col * size)
	{
	}
	tile(const tile& t, const int row, const int col, const int size)
		: data_(t.data_), row_(t.row_ + row * size), col_(t.col_ + col * size)
	{
	}
	tile() = default;
	tile(const tile&) = default;
	tile(tile&&) = default;
	tile& operator=(const tile&) = default;
	tile& operator=(tile&&) = default;
	~tile() = default;

	double& operator()(const int row, const int col) const
	{
		return (*data_)[row_ + row][col_ + col];
	}
};

class tile_view
{
	const std::vector<std::vector<double>>* data_ = nullptr;
	int row_ = 0;
	int col_ = 0;

public:
	tile_view(const std::vector<std::vector<double>>& m, const int row, const int col, const int size)
		: data_(&m), row_(row * size), col_(col * size)
	{
	}

	tile_view(const tile_view& t, const int row, const int col, const int size)
		: data_(t.data_), row_(t.row_ + row * size), col_(t.col_ + col * size)
	{
	}

	tile_view() = default;
	tile_view(const tile_view&) = default;
	tile_view(tile_view&&) = default;
	tile_view& operator=(const tile_view&) = default;
	tile_view& operator=(tile_view&&) = default;
	~tile_view() = default;

	double operator()(const int row, const int col) const
	{
		return (*data_)[row_ + row][col_ + col];
	}
};