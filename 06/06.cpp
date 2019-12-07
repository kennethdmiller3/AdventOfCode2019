// https://adventofcode.com/2019/day/6

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

// read orbits from the input stream
void ReadInput(std::unordered_map<std::string, std::string>& output, std::istream& input)
{
	std::string orbit;
	while (std::getline(input, orbit))
	{
		std::istringstream orbitstream(orbit);
		std::string orbited, orbiting;
		std::getline(orbitstream, orbited, ')');
		std::getline(orbitstream, orbiting, ')');
		output[orbiting] = orbited;
	}
}

void Part1(const std::unordered_map<std::string, std::string>& orbiting)
{
	// this implementation is stupid. :P
	int count = 0;
	for (std::pair<std::string, std::string> const& orbit : orbiting)
	{
		std::unordered_map<std::string, std::string>::const_iterator itor = orbiting.find(orbit.first);
		while (itor != orbiting.end())
		{
			itor = orbiting.find(itor->second);
			++count;
		}
	}

	std::cout << "Part 1: count=" << count << std::endl;
}

void Part2(const std::unordered_map<std::string, std::string>& orbiting)
{
	std::unordered_map<std::string, int> distances;

	std::unordered_map<std::string, std::string>::const_iterator itor;

	// find the path from SAN to COM
	itor = orbiting.find("SAN");
	int san_distance = 0;
	while (itor != orbiting.end())
	{
		// save distance to this node
		distances[itor->second] = san_distance;

		// go to parent node
		itor = orbiting.find(itor->second);
		++san_distance;
	}

	// find the path from YOU to COM and stop when we find a common node
	itor = orbiting.find("YOU");
	int you_distance = 0;
	while (itor != orbiting.end())
	{
		if (distances.find(itor->second) != distances.end())
		{
			// found the common node
			// add SAN distance to YOU distance and stop
			you_distance += distances[itor->second];
			break;
		}
		else
		{
			// go to parent node
			itor = orbiting.find(itor->second);
			++you_distance;
		}
	}

	std::cout << "Part 2: distance=" << you_distance << std::endl;
}

int main()
{
	std::unordered_map<std::string, std::string> orbiting;
	ReadInput(orbiting, std::cin);

	Part1(orbiting);
	Part2(orbiting);

	return 0;
}
