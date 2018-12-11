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
					tile_a = tile_view{a, tile_y, tile_k, tile_size};
					tile_b = tile_view{b, tile_k, tile_x, tile_size};
				}
				else
				{
					tile_a = tile_view{a, tile_y, tile_k, tile_size};
					tile_b = tile_view{b, tile_x, tile_k, tile_size};
				}
				auto tile_c = tile{&c, tile_y, tile_x, tile_size};

				const auto subtile_count = 2;
				const auto subtile_size = tile_size / subtile_count;
				for (auto subtile_y = 0; subtile_y < subtile_count; subtile_y++)
				{
					for (auto subtile_x = 0; subtile_x < subtile_count; subtile_x++)
					{
						for (auto subtile_k = 0; subtile_k < subtile_count; subtile_k++)
						{
							tile_view subtile_a;
							tile_view subtile_b;
							if constexpr (!Transposed)
							{
								subtile_a = tile_view{tile_a, subtile_y, subtile_k, subtile_size};
								subtile_b = tile_view{tile_b, subtile_k, subtile_x, subtile_size};
							}
							else
							{
								subtile_a = tile_view{tile_a, subtile_y, subtile_k, subtile_size};
								subtile_b = tile_view{tile_b, subtile_x, subtile_k, subtile_size};
							}
							auto subtile_c = tile{tile_c, subtile_y, subtile_x, subtile_size};

							for (auto y = 0; y < subtile_size; y++)
							{
								for (auto x = 0; x < subtile_size; x++)
								{
									for (auto k = 0; k < subtile_size; k++)
									{
										if constexpr (!Transposed)
										{
											subtile_c(y, x) += subtile_a(y, k) * subtile_b(k, x);
										}
										else
										{
											subtile_c(y, x) += subtile_a(y, k) * subtile_b(x, k);
										}
									}
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
	test("Double Tiled Parallel", mult_2_tiled, 2048, false);
	test("Double Tiled Parallel Transposed", mult_2_tiled_transposed, 2048, true);
	return 0;
}
