// https://adventofcode.com/2019/day/4

#include <iostream>
#include <string>

// read the range of values from the input stream
std::pair<int, int> ReadInput(std::istream& input)
{
	char line[256];
	input.getline(line, 256);
	int first, second;
	sscanf_s(line, "%d-%d", &first, &second);
	return std::pair<int, int>(first, second);
}

// PART 1
void Part1(std::pair<int, int> range)
{
	int count = 0;
	for (int i = range.first; i <= range.second; ++i)
	{
		// separate digits
		int ii = i;
		int d[6];
		d[0] = ii % 10; ii /= 10;
		d[1] = ii % 10; ii /= 10;
		d[2] = ii % 10; ii /= 10;
		d[3] = ii % 10; ii /= 10;
		d[4] = ii % 10; ii /= 10;
		d[5] = ii % 10;

		// if the digits are nondecreasing...
		if (d[0] >= d[1] && d[1] >= d[2] && d[2] >= d[3] && d[3] >= d[4] && d[4] >= d[5])
		{
			// if a neighboring pair of digits match...
			if (d[5] == d[4] || d[4] == d[3] || d[3] == d[2] || d[2] == d[1] || d[1] == d[0])
			{
				++count;
			}
		}
	}
	std::cout << "Part 1: count=" << count << std::endl;
}

// PART 2
void Part2(std::pair<int, int> range)
{
	int count = 0;

	for (int i = range.first; i <= range.second; ++i)
	{
		// separate digits
		int ii = i;
		int d[6];
		d[0] = ii % 10; ii /= 10;
		d[1] = ii % 10; ii /= 10;
		d[2] = ii % 10; ii /= 10;
		d[3] = ii % 10; ii /= 10;
		d[4] = ii % 10; ii /= 10;
		d[5] = ii % 10;

		// if the digits are nondecreasing...
		if (d[0] >= d[1] && d[1] >= d[2] && d[2] >= d[3] && d[3] >= d[4] && d[4] >= d[5])
		{
			bool has_pair = false;
			int run = 0;
			for (int j = 1; j < 6; ++j)
			{
				if (d[j] == d[j - 1])
				{
					// increase the run
					++run;
				}
				else if (run == 1)
				{
					// found a pair so we don't need to look any further
					has_pair = true;
					break;
				}
				else
				{
					// reset the run
					run = 0;
				}
			}
			if (run == 1)
			{
				has_pair = true;
			}
			count += has_pair;
		}
	}

	std::cout << "Part 1: count=" << count << std::endl;
}

int main()
{
	std::pair<int, int> range = ReadInput(std::cin);

	Part1(range);
	Part2(range);

	return 0;
}
