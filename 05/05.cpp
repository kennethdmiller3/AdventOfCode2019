// https://adventofcode.com/2019/day/5

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

enum Opcode
{
	Opcode_Add = 1,
	Opcode_Multiply = 2,
	Opcode_Input = 3,
	Opcode_Output = 4,
	Opcode_JumpIfTrue = 5,
	Opcode_JumpIfFalse = 6,
	Opcode_LessThan = 7,
	Opcode_Equals = 8,
	Opcode_Halt = 99
};

enum Mode
{
	Mode_Position = 0,
	Mode_Immediate = 1
};

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
bool Run(std::vector<int>& program, std::vector<int>& input, std::vector<int>& output)
{
	std::vector<int>::iterator input_itor = input.begin();

	for (int pc = 0; pc < program.size();)
	{
		int instruction = program[pc++];
		int opcode = instruction % 100; instruction /= 100;
		int mode1 = instruction % 10; instruction /= 10;
		int mode2 = instruction % 10; instruction /= 10;
		int mode3 = instruction % 10; instruction /= 10;

		switch (opcode)
		{
		case Opcode_Add:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			int out = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			program[out] = in1 + in2;
			break;
		}

		case Opcode_Multiply:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			int out = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			program[out] = in1 * in2;
			break;
		}

		case Opcode_Input:
		{
			int out = program[pc++];
			program[out] = *input_itor++;
			break;
		}

		case Opcode_Output:
		{
			int in = program[pc++];
			if (mode1 == Mode_Position)
				in = program[in];
			output.push_back(in);
			break;
		}

		case Opcode_JumpIfTrue:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			if (in1 != 0)
				pc = in2;
			break;
		}

		case Opcode_JumpIfFalse:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			if (in1 == 0)
				pc = in2;
			break;
		}

		case Opcode_LessThan:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			int out = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			program[out] = in1 < in2;
			break;
		}

		case Opcode_Equals:
		{
			int in1 = program[pc++];
			int in2 = program[pc++];
			int out = program[pc++];
			if (mode1 == Mode_Position)
				in1 = program[in1];
			if (mode2 == Mode_Position)
				in2 = program[in2];
			program[out] = in1 == in2;
			break;
		}

		case Opcode_Halt:
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
	// make a copy of the program
	std::vector<int> memory = program;

	std::vector<int> input, output;
	input.push_back(1);

	Run(memory, input, output);

	std::cout << "Part 1: result " << output.back() << std::endl;
}

// PART 2
void Part2(const std::vector<int>& program)
{
	// make a copy of the program
	std::vector<int> memory = program;

	std::vector<int> input, output;
	input.push_back(5);

	Run(memory, input, output);

	std::cout << "Part 2: result " << output.back() << std::endl;
}

int main()
{
	std::vector<int> program;
	ReadInput(program, std::cin);

	Part1(program);
	Part2(program);

	return 0;
}
