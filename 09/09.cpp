// https://adventofcode.com/2019/day/9

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <array>
#include <map>

constexpr int64_t N = 5;

enum class Opcode
{
	Add = 1,
	Multiply = 2,
	Input = 3,
	Output = 4,
	JumpIfTrue = 5,
	JumpIfFalse = 6,
	LessThan = 7,
	Equals = 8,
	RelativeBaseOffset = 9,
	Halt = 99
};

enum class Mode
{
	Position = 0,
	Immediate = 1,
	Relative = 2,
};

enum class State
{
	Run,
	Halt,
	Error
};

// read instructions from the input stream
void ReadInput(std::map<int64_t, int64_t>& output, std::istream& input)
{
	std::string entry;
	int64_t pc = 0;
	while (std::getline(input, entry, ','))
	{
		std::stringstream sstream(entry);
		int64_t value;
		sstream >> value;
		output[pc++] = value;
	}
}

// run one instruction
State RunInstruction(std::map<int64_t, int64_t>& program, std::vector<int64_t>& input, std::vector<int64_t>& output, int64_t& pc, int64_t& relativebase)
{
	int64_t instruction = program[pc++];

	// decode the instruction
	const Opcode opcode = Opcode(instruction % 100); instruction /= 100;
	const Mode mode1 = Mode(instruction % 10); instruction /= 10;
	const Mode mode2 = Mode(instruction % 10); instruction /= 10;
	const Mode mode3 = Mode(instruction % 10); instruction /= 10;

	switch (opcode)
	{
	case Opcode::Add:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		int64_t out = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (mode3 == Mode::Relative)
			out += relativebase;
		program[out] = in1 + in2;
		return State::Run;
	}

	case Opcode::Multiply:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		int64_t out = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (mode3 == Mode::Relative)
			out += relativebase;
		program[out] = in1 * in2;
		return State::Run;
	}

	case Opcode::Input:
	{
		if (input.empty())
		{
			// wait for innput
			--pc;
		}
		else
		{
			// get input
			int64_t out = program[pc++];
			if (mode1 == Mode::Relative)
				out += relativebase;
			program[out] = input.front();
			input.erase(input.begin());
		}
		return State::Run;
	}

	case Opcode::Output:
	{
		int64_t in = program[pc++];
		if (mode1 == Mode::Relative)
			in += relativebase;
		if (mode1 != Mode::Immediate)
			in = program[in];
		output.push_back(in);
		return State::Run;
	}

	case Opcode::JumpIfTrue:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (in1 != 0)
			pc = in2;
		return State::Run;
	}

	case Opcode::JumpIfFalse:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (in1 == 0)
			pc = in2;
		return State::Run;
	}

	case Opcode::LessThan:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		int64_t out = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (mode3 == Mode::Relative)
			out += relativebase;
		program[out] = in1 < in2;
		return State::Run;
	}

	case Opcode::Equals:
	{
		int64_t in1 = program[pc++];
		int64_t in2 = program[pc++];
		int64_t out = program[pc++];
		if (mode1 == Mode::Relative)
			in1 += relativebase;
		if (mode1 != Mode::Immediate)
			in1 = program[in1];
		if (mode2 == Mode::Relative)
			in2 += relativebase;
		if (mode2 != Mode::Immediate)
			in2 = program[in2];
		if (mode3 == Mode::Relative)
			out += relativebase;
		program[out] = in1 == in2;
		return State::Run;
	}

	case Opcode::RelativeBaseOffset:
	{
		int64_t in = program[pc++];
		if (mode1 == Mode::Relative)
			in += relativebase;
		if (mode1 != Mode::Immediate)
			in = program[in];
		relativebase += in;
		return State::Run;
	}

	case Opcode::Halt:
	{
		// HALT
		--pc;
		return State::Halt;
	}

	default:
	{
		// ERROR
		--pc;
		return State::Error;
	}
	}
}

// run
State Run(std::map<int64_t, int64_t>& program, std::vector<int64_t>& input, std::vector<int64_t>& output)
{
	std::vector<int64_t>::iterator input_itor = input.begin();

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		state = RunInstruction(program, input, output, pc, relativebase);
	}
	while (state == State::Run && pc < program.size());

	return state;
}

// PART 1
void Part1(const std::map<int64_t, int64_t>& program)
{
	std::map<int64_t, int64_t> memory = program;

	std::vector<int64_t> input, output;
	input.push_back(1);

	Run(memory, input, output);

	std::cout << "Part 1: result " << output.back() << std::endl;
}

// PART 2
void Part2(const std::map<int64_t, int64_t>& program)
{
	std::map<int64_t, int64_t> memory = program;

	std::vector<int64_t> input, output;
	input.push_back(2);

	Run(memory, input, output);

	std::cout << "Part 2: result " << output.back() << std::endl;
}

int main()
{
	std::map<int64_t, int64_t> program;
	ReadInput(program, std::cin);

	Part1(program);
	Part2(program);

	return 0;
}
