#include <vector>
#include "utility.h"
#include "tile.h"

template <bool Transposed>
auto mult_tiled_impl(const std::vector<std::vector<double>>& a,
	const std::vector<std::vector<double>>& b)
{
	const auto size = static_cast<int>(a.size());
	const auto tile_size = 16;
	const auto tile_count = size / tile_size;

	auto c = std::vector<std::vector<double>>(size, std::vector<double>(size, 0.0));
#pragma omp parallel for
	for (auto tile_y = 0; tile_y < tile_count; tile_y++)
	{
		for (auto tile_x = 0; tile_x < tile_count; tile_x++)
		{
			for (auto tile_k = 0; tile_k < tile_count; tile_k++)
			{
				tile_view tile_a;
				tile_view tile_b;
				if constexpr (!Transposed)
				{
					tile_a = tile_view{ a, tile_y, tile_k, tile_size };
					tile_b = tile_view{ b, tile_k, tile_x, tile_size };
				}
				else
				{
					tile_a = tile_view{ a, tile_y, tile_k, tile_size };
					tile_b = tile_view{ b, tile_x, tile_k, tile_size };
				}
				auto tile_c = tile{ &c, tile_y, tile_x, tile_size };

				for (auto y = 0; y < tile_size; y++)
				{
					for (auto x = 0; x < tile_size; x++)
					{
						for (auto k = 0; k < tile_size; k++)
						{
							if constexpr (!Transposed)
							{
								tile_c(y, x) += tile_a(y, k) * tile_b(k, x);
							}
							else
							{
								tile_c(y, x) += tile_a(y, k) * tile_b(x, k);
							}
						}
					}
				}
			}
		}
	}

	return c;
}


auto mult_tiled(const std::vector<std::vector<double>>& a,
	const std::vector<std::vector<double>>& b)
{
	return mult_tiled_impl<false>(a, b);
}

auto mult_tiled_transposed(const std::vector<std::vector<double>>& a,
	const std::vector<std::vector<double>>& b)
{
	return mult_tiled_impl<true>(a, b);
}

int main() {
	test("Tiled Parallel", mult_tiled, 2048);
	test("Tiled Parallel Transposed", mult_tiled_transposed, 2048);
	return 0;
}
