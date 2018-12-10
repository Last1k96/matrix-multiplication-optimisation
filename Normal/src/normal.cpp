#include <vector>
#include "utility.h"

auto mult(const std::vector<std::vector<double>>& a,
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
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	return c;
}


int main() {

	test("Normal", mult, 2048);
	return 0;
}
