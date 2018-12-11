#include "utility.h"

template <bool Transposed>
auto mult_impl(const std::vector<std::vector<double>>& a,
               const std::vector<std::vector<double>>& b)
{
	const auto size = static_cast<int>(a.size());

	auto c = std::vector<std::vector<double>>(size, std::vector<double>(size, 0.0));

	for (auto i = 0; i < size; i++)
	{
		for (auto j = 0; j < size; j++)
		{
			for (auto k = 0; k < size; k++)
			{
				if constexpr (!Transposed)
				{
					c[i][j] += a[i][k] * b[k][j];
				}
				else
				{
					c[i][j] += a[i][k] * b[j][k];
				}
			}
		}
	}

	return c;
}

auto mult_transposed(const std::vector<std::vector<double>>& a,
                     const std::vector<std::vector<double>>& b)
{
	return mult_impl<true>(a, b);
}

int main()
{
	test("Normal Transposed", mult_transposed, 2048, true);
	return 0;
}
