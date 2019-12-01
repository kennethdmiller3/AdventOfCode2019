// https://adventofcode.com/2019/day/1

#include <iostream>
#include <vector>

// read module masses from the input stream
void ReadInput(std::vector<int>& output, std::istream& input)
{
	int mass;
	while (input >> mass)
	{
		output.push_back(mass);
	}
}

// PART 1
void Part1(const std::vector<int>& masses)
{
	int total = 0;
	for (int mass : masses)
	{
		int fuel = mass / 3 - 2;
		total += fuel;
	}

	std::cout << "Part 1: total fuel " << total << std::endl;
}

// PART 2
void Part2(const std::vector<int>& masses)
{
	int total = 0;
	for (int mass : masses)
	{
		int fuel = mass / 3 - 2;
		while (fuel > 0)
		{
			total += fuel;
			fuel = fuel / 3 - 2;
		}
	}

	std::cout << "Part 2: total fuel " << total << std::endl;
}

int main()
{
	std::vector<int> masses;
	ReadInput(masses, std::cin);

	Part1(masses);
	Part2(masses);

	return 0;
}
