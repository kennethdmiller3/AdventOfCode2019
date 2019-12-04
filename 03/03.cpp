// https://adventofcode.com/2019/day/3

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct SWireSegment
{
	char dir;
	int length;

	SWireSegment(char dir = 0, int length = 0)
		: dir(dir)
		, length(length)
	{
	}
};

struct SPoint
{
	short x;
	short y;
};

bool operator==(const SPoint& lhs, const SPoint& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

namespace std
{
	template<> struct hash<SPoint>
	{
		std::size_t operator()(const SPoint& k) const
		{
			return hash<short>()(k.x) ^ (hash<short>()(k.y) << 1);
		}
	};
}

// read wires from the input stream
void ReadInput(std::vector<std::vector<SWireSegment>>& output, std::istream& input)
{
	std::string line;
	while (std::getline(input, line))
	{
		std::istringstream linestream(line);

		output.emplace_back();
		std::vector<SWireSegment>& wire = output.back();

		char entry[256];
		while (linestream.getline(entry, 256, ','))
		{
			wire.emplace_back(entry[0], atoi(entry + 1));
		}
	}
}

// PART 1
void Part1(const std::vector<std::vector<SWireSegment>>& wires)
{
	std::unordered_map<SPoint, char> grid;
	long best_dist = INT_MAX;
	SPoint best_pos = { 0, 0 };
	for (int w = 0; w < wires.size(); ++w)
	{
		SPoint pos = { 0, 0 };
		for (const SWireSegment& s : wires[w])
		{
			SPoint move;
			switch (s.dir)
			{
			case 'U': move = {  0, -1 }; break;
			case 'R': move = { +1,  0 }; break;
			case 'D': move = {  0, +1 }; break;
			case 'L': move = { -1,  0 }; break;
			};
			for (int i = 0; i < s.length; ++i)
			{
				pos.x += move.x;
				pos.y += move.y;
				auto g = grid.find(pos);
				if (g == grid.end())
				{
					grid[pos] = w;
				}
				else if (g->second != w)
				{
					// intersection
					long dist = labs(pos.x) + labs(pos.y);
					if (dist < best_dist)
					{
						best_dist = dist;
						best_pos = pos;
					}
				}
			}
		}
	}

	std::cout << "Part 1: pos=" << best_pos.x << "," << best_pos.y << " dist=" << best_dist << std::endl;
}

// PART 2
void Part2(const std::vector<std::vector<SWireSegment>>& wires)
{
	std::unordered_map<SPoint, std::pair<char, int>> grid;
	long best_delay = INT_MAX;
	SPoint best_pos = { 0, 0 };

	for (int w = 0; w < wires.size(); ++w)
	{
		SPoint pos = { 0, 0 };
		int step = 0;
		for (const SWireSegment& s : wires[w])
		{
			SPoint move;
			switch (s.dir)
			{
			case 'U': move = { 0, -1 }; break;
			case 'R': move = { +1,  0 }; break;
			case 'D': move = { 0, +1 }; break;
			case 'L': move = { -1,  0 }; break;
			};
			for (int i = 0; i < s.length; ++i)
			{
				pos.x += move.x;
				pos.y += move.y;
				++step;
				auto g = grid.find(pos);
				if (g == grid.end())
				{
					grid[pos] = std::pair<char, int>(w, step);
				}
				else if (g->second.first != w)
				{
					// intersection
					long delay = step + g->second.second;
					if (delay < best_delay)
					{
						best_delay = delay;
						best_pos = pos;
					}
				}
			}
		}
	}

	std::cout << "Part 1: pos=" << best_pos.x << "," << best_pos.y << " delay=" << best_delay << std::endl;
}

int main()
{
	std::vector<std::vector<SWireSegment>> wires;
	ReadInput(wires, std::cin);

	Part1(wires);
	Part2(wires);

	return 0;
}
