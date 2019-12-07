// https://adventofcode.com/2019/day/7

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <array>

constexpr int N = 5;

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
	Halt = 99
};

enum class Mode
{
	Position = 0,
	Immediate = 1
};

enum class State
{
	Run,
	Halt,
	Error
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

// run one instruction
State RunInstruction(std::vector<int>& program, std::vector<int>& input, std::vector<int>& output, int& pc)
{
	int instruction = program[pc++];

	// decode the instruction
	const Opcode opcode = Opcode(instruction % 100); instruction /= 100;
	const Mode mode1 = Mode(instruction % 10); instruction /= 10;
	const Mode mode2 = Mode(instruction % 10); instruction /= 10;
	const Mode mode3 = Mode(instruction % 10); instruction /= 10;

	switch (opcode)
	{
	case Opcode::Add:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		int out = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
		program[out] = in1 + in2;
		return State::Run;
	}

	case Opcode::Multiply:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		int out = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
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
			int out = program[pc++];
			program[out] = input.front();
			input.erase(input.begin());
		}
		return State::Run;
	}

	case Opcode::Output:
	{
		int in = program[pc++];
		if (mode1 == Mode::Position)
			in = program[in];
		output.push_back(in);
		return State::Run;
	}

	case Opcode::JumpIfTrue:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
		if (in1 != 0)
			pc = in2;
		return State::Run;
	}

	case Opcode::JumpIfFalse:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
		if (in1 == 0)
			pc = in2;
		return State::Run;
	}

	case Opcode::LessThan:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		int out = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
		program[out] = in1 < in2;
		return State::Run;
	}

	case Opcode::Equals:
	{
		int in1 = program[pc++];
		int in2 = program[pc++];
		int out = program[pc++];
		if (mode1 == Mode::Position)
			in1 = program[in1];
		if (mode2 == Mode::Position)
			in2 = program[in2];
		program[out] = in1 == in2;
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
State Run(std::vector<int>& program, std::vector<int>& input, std::vector<int>& output)
{
	std::vector<int>::iterator input_itor = input.begin();

	int pc = 0;
	State state = State::Run;
	do
	{
		state = RunInstruction(program, input, output, pc);
	}
	while (state == State::Run && pc < program.size());

	return state;
}

template<typename F>
void HeapPermutations(int a[], int size, int n, const F& op)
{
	if (size == 1)
	{
		op(a);
		return;
	}

	for (int i = 0; i < size; ++i)
	{
		HeapPermutations(a, size - 1, n, op);
		if (size & 1)
		{
			std::swap(a[0], a[size - 1]);
		}
		else
		{
			std::swap(a[i], a[size - 1]);
		}
	}
}

// PART 1
void Part1(const std::vector<int>& program)
{
	int highest = INT_MIN;

	int phase[N] = { 0, 1, 2, 3, 4 };
	HeapPermutations(phase, N, N, [&](int a[])
		{
			std::vector<int> input, output;
			output.push_back(0);

			for (int i = 0; i < N; ++i)
			{
				// make a copy of the program for each amplifier
				std::vector<int> memory = program;
				input.clear();
				input.push_back(phase[i]);
				input.push_back(output.back());
				output.clear();
				Run(memory, input, output);
			}

			int result = output.back();
			std::cout << result << std::endl;

			if (highest < result)
			{
				highest = result;
			}
		}
	);

	std::cout << "Part 1: highest " << highest << std::endl;
}

// PART 2
void Part2(const std::vector<int>& program)
{
	int highest = INT_MIN;

	// make a copy of the program for each amplifier
	std::array<std::vector<int>, N> memory;
	memory.fill(program);

	int phase[N] = { 5, 6, 7, 8, 9 };
	HeapPermutations(phase, N, N, [&](int a[])
		{
			// data for each stage
			std::vector<int> data[N];

			// phase settings for each stage
			for (int i = 0; i < N; ++i)
			{
				data[i].push_back(phase[i]);
			}

			// initial input for amplifier A
			data[0].push_back(0);

			// state for each amplifier
			int pc[N] = { 0, 0, 0, 0, 0 };
			State state[N] = { State::Run, State::Run, State::Run, State::Run, State::Run };

			// run until the last amplifier halts
			do
			{
				for (int i = 0; i < N; ++i)
				{
					state[i] = RunInstruction(memory[i], data[i], data[(i + 1) % N], pc[i]);
				}
			}
			while (state[N - 1] == State::Run);

			// get the output from the last amplifier
			int result = data[0].back();
			std::cout << result << std::endl;

			if (highest < result)
			{
				highest = result;
			}
		}
	);

	std::cout << "Part 2: highest " << highest << std::endl;
}

int main()
{
	std::vector<int> program;
	ReadInput(program, std::cin);

	Part1(program);
	Part2(program);

	return 0;
}
