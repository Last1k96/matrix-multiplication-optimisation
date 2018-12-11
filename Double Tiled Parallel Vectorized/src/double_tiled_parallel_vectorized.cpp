#include "utility.h"
#include "tile.h"

template <bool Transposed>
auto mult_2_tiled_impl(const std::vector<std::vector<double>>& a,
                       const std::vector<std::vector<double>>& b)
{
	const auto size = static_cast<int>(a.size());
	const auto tile_size = 128;
	const auto tile_count = size / tile_size;

	auto c = std::vector<std::vector<double>>(size, std::vector<double>(size, 0.0));

#pragma omp parallel for
	for (auto tile_y = 0; tile_y < tile_count; ++tile_y)
	{
		for (auto tile_x = 0; tile_x < tile_count; ++tile_x)
		{
			for (auto tile_k = 0; tile_k < tile_count; ++tile_k)
			{
				const auto subtile_count = 2;
				const auto subtile_size = tile_size / subtile_count;
				for (auto subtile_y = 0; subtile_y < subtile_count; ++subtile_y)
				{
					for (auto subtile_x = 0; subtile_x < subtile_count; ++subtile_x)
					{
						for (auto subtile_k = 0; subtile_k < subtile_count; ++subtile_k)
						{
							for (auto y = 0; y < subtile_size; ++y)
							{
								for (auto x = 0; x < subtile_size; ++x)
								{
									auto sum = 0.0;
									for (auto k = 0; k < subtile_size; ++k)
									{
										if constexpr (!Transposed)
										{
											sum += a[tile_y * tile_size + subtile_y * subtile_size + y]
													[tile_k * tile_size + subtile_k * subtile_size + k]
												 * b[tile_k * tile_size + subtile_k * subtile_size + k]
													[tile_x * tile_size + subtile_x * subtile_size + x];
										}
										else
										{
											sum += a[tile_y * tile_size + subtile_y * subtile_size + y]
													[tile_k * tile_size + subtile_k * subtile_size + k]
												 * b[tile_x * tile_size + subtile_x * subtile_size + x]
													[tile_k * tile_size + subtile_k * subtile_size + k];
										}
									}
									c[tile_y * tile_size + subtile_y * subtile_size + y]
									 [tile_x * tile_size + subtile_x * subtile_size + x] += sum;
								}
							}
						}
					}
				}
			}
		}
	}

	return c;
}

auto mult_2_tiled(const std::vector<std::vector<double>>& a,
                  const std::vector<std::vector<double>>& b)
{
	return mult_2_tiled_impl<false>(a, b);
}

auto mult_2_tiled_transposed(const std::vector<std::vector<double>>& a,
                             const std::vector<std::vector<double>>& b)
{
	return mult_2_tiled_impl<true>(a, b);
}

int main()
{
	test("Double Tiled Parallel Vectorized", mult_2_tiled, 2048, false);
	test("Double Tiled Parallel Vectorized Transposed", mult_2_tiled_transposed, 2048, true);
	return 0;
}
