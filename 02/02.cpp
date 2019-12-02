// https://adventofcode.com/2019/day/2

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>

// read instructions from the input stream
void ReadInput(std::vector<int>& output, std::istream& input)
{
	std::string entry;
	while (std::getline(input, entry, ','))
	{
		std::stringstream sstream(entry);
		int value;
		sstream >> value;
		output.push_back(value);
	}
}

// run
bool Run(std::vector<int>& program)
{
	for (int i = 0; i < program.size(); i += 4)
	{
		int op = program[i];
		int in1 = program[i + 1];
		int in2 = program[i + 2];
		int out = program[i + 3];

		switch (op)
		{
		case 1: // add
			program[out] = program[in1] + program[in2];
			break;

		case 2: // multiply
			program[out] = program[in1] * program[in2];
			break;
		
		case 99: // end
			return true;

		default:
			// ERROR
			return false;
		}
	}
	return true;
}

// PART 1
void Part1(const std::vector<int>& program)
{
	// make a copy of the data
	std::vector<int> memory = program;

	memory[1] = 12;
	memory[2] = 2;

	Run(memory);

	std::cout << "Part 1: result " << memory[0] << std::endl;
}

// PART 2
void Part2(const std::vector<int>& program)
{
	for (int noun = 0; noun < 100; ++noun)
	{
		for (int verb = 0; verb < 100; ++verb)
		{
			// make a copy of the program
			std::vector<int> memory = program;
			memory[1] = noun;
			memory[2] = verb;

			Run(memory);

			if (memory[0] == 19690720)
			{
				std::cout << "Part 2: noun=" << noun << " verb=" << verb << " result=" << noun * 100 + verb << std::endl;
				break;
			}
		}
	}
}

int main()
{
	std::vector<int> program;
	ReadInput(program, std::cin);

	Part1(program);
	Part2(program);

	return 0;
}
