// https://adventofcode.com/2019/day/11

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>

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

enum class Color : int8_t
{
	Black = 0,
	White = 1,
	Count
};

enum class Direction : int8_t
{
	Up,
	Right,
	Down,
	Left,
	Count
};

enum class Turn : int8_t
{
	Left,
	Right
};

struct Point
{
	short x;
	short y;
};

bool operator==(const Point& lhs, const Point& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

namespace std
{
	template<> struct hash<Point>
	{
		std::size_t operator()(const Point& k) const
		{
			return hash<short>()(k.x) ^ (hash<short>()(k.y) << 1);
		}
	};
}

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

void Run(const std::map<int64_t, int64_t>& program, std::unordered_map<Point, Color>& hull, Point position, Direction direction)
{
	std::map<int64_t, int64_t> memory = program;

	std::vector<int64_t> input, output;

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		input.clear();
		input.push_back(int(hull[position]));

		state = RunInstruction(memory, input, output, pc, relativebase);

		if (state == State::Run && output.size() == 2)
		{
			Color color = Color(output[0]);
			Turn turn = Turn(output[1]);
			output.clear();

			// paint the current tile
			hull[position] = color;

			// turn
			switch (turn)
			{
			case Turn::Left:
				direction = Direction((int(direction) + int(Direction::Count) - 1) % int(Direction::Count));
				break;
			case Turn::Right:
				direction = Direction((int(direction) + 1) % int(Direction::Count));
				break;
			}

			// move
			switch (direction)
			{
			case Direction::Up:
				position.y -= 1;
				break;
			case Direction::Right:
				position.x += 1;
				break;
			case Direction::Down:
				position.y += 1;
				break;
			case Direction::Left:
				position.x -= 1;
				break;
			}
		}
	}
	while (state == State::Run);
}

// PART 1
void Part1(const std::map<int64_t, int64_t>& program)
{
	std::unordered_map<Point, Color> hull;

	Run(program, hull, { 0, 0 }, Direction::Up);

	std::cout << "Part 1: result " << hull.size() << std::endl;
}

// PART 2
void Part2(const std::map<int64_t, int64_t>& program)
{
	std::unordered_map<Point, Color> hull;

	// set the initial location to white
	hull[{ 0, 0 }] = Color::White;

	// paint the identifier
	Run(program, hull, { 0, 0 }, Direction::Up);

	// get bounds of the painted area
	Point lower = { 0, 0 }, upper = { 0, 0 };
	for (const std::pair<Point, Color>& entry : hull)
	{
		const Point& position = entry.first;
		lower.x = std::min(lower.x, position.x);
		lower.y = std::min(lower.y, position.y);
		upper.x = std::max(upper.x, position.x);
		upper.y = std::max(upper.y, position.y);
	}

	// print the region
	std::cout << "Part 2: " << std::endl;
	for (short y = lower.y; y <= upper.y; ++y)
	{
		for (short x = lower.x; x <= upper.x; ++x)
		{
			Color color = hull[{x, y}];
			char output = color == Color::White ? '#' : '.';
			std::cout << output;
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::map<int64_t, int64_t> program;
	ReadInput(program, std::cin);

	Part1(program);
	Part2(program);

	return 0;
}
