// https://adventofcode.com/2019/day/17

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
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

enum class Direction
{
	Up,
	Right,
	Down,
	Left,
	Count
};

struct Point
{
	short x;
	short y;
};

Point operator+(const Point& lhs, const Point& rhs)
{
	return { lhs.x + rhs.x, lhs.y + rhs.y };
}
Point operator-(const Point& lhs, const Point& rhs)
{
	return { lhs.x - rhs.x, lhs.y - rhs.y };
}

bool operator==(const Point& lhs, const Point& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
bool operator!=(const Point& lhs, const Point& rhs)
{
	return lhs.x != rhs.x && lhs.y != rhs.y;
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

void BuildMap(const std::map<int64_t, int64_t>& program, std::string& map, Point& size, Point& position, Direction& direction)
{
	std::map<int64_t, int64_t> memory = program;

	std::vector<int64_t> input, output;

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		state = RunInstruction(memory, input, output, pc, relativebase);
	} while (state == State::Run);

	map.clear();
	map.reserve(output.size());

	size = { 0, 0 };

	short col = 0;

	for (int64_t o : output)
	{
		map.push_back(char(o));

		switch (o)
		{
		case 10:
			if (col > 0)
			{
				++size.y;
				if (size.x < col + 1)
					size.x = col + 1;
			}
			col = -1;
			break;
		case '^':
			position = { col, size.y };
			direction = Direction::Up;
			break;
		case '>':
			position = { col, size.y };
			direction = Direction::Right;
			break;
		case 'v':
			position = { col, size.y };
			direction = Direction::Down;
			break;
		case '<':
			position = { col, size.y };
			direction = Direction::Left;
			break;
		}
		++col;
	}

	std::cout << map;
}

// PART 1
void Part1(const std::map<int64_t, int64_t>& program, std::string& map, Point size, Point position, Direction direction)
{
	// find intersections
	int sum = 0;
	for (int y = 1; y < size.y - 1; ++y)
	{
		for (int x = 1; x < size.x - 1; ++x)
		{
			if (map[y * size.x + x] != '.' &&
				map[y * size.x + x - size.x] != '.' &&
				map[y * size.x + x - 1] != '.' &&
				map[y * size.x + x + 1] != '.' &&
				map[y * size.x + x + size.x] != '.')
			{
				// found an intersection
				sum += x * y;
			}
		}
	}

	std::cout << "Part 1: " << sum << std::endl;
}

Point directionstep[int(Direction::Count)]
{
	{ 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 }
};

// PART 2
void Part2(const std::map<int64_t, int64_t>& program, const std::string& map, Point size, Point position, Direction direction)
{
	std::ostringstream commandstream;

	int move_length = 0;

	Point start = position;

	for(;;)
	{
		// look ahead
		{
			const Point look = position + directionstep[int(direction)];
			char at = look.x >= 0 && look.x < size.x && look.y >= 0 && look.y < size.y ? map[look.y * size.x + look.x] : '.';
			if (at == '#')
			{
				position = look;
				++move_length;
				continue;
			}
		}

		if (move_length > 0)
		{
			commandstream << move_length << ",";
			move_length = 0;
		}

		// look left
		{
			const Direction turn = Direction((int(direction) + 3) % 4);
			const Point look = position + directionstep[int(turn)];
			char at = look.x >= 0 && look.x < size.x && look.y >= 0 && look.y < size.y ? map[look.y * size.x + look.x] : '.';
			if (at == '#')
			{
				direction = turn;
				commandstream << "L,";
				continue;
			}
		}

		// look right
		{
			const Direction turn = Direction((int(direction) + 1) % 4);
			const Point look = position + directionstep[int(turn)];
			char at = look.x >= 0 && look.x < size.x && look.y >= 0 && look.y < size.y ? map[look.y * size.x + look.x] : '.';
			if (at == '#')
			{
				direction = turn;
				commandstream << "R,";
				continue;
			}
		}

		if (position != start)
			break;

		// look back
		{
			const Direction turn = Direction((int(direction) + 2) % 4);
			const Point look = position + directionstep[int(turn)];
			char at = look.x >= 0 && look.x < size.x && look.y >= 0 && look.y < size.y ? map[look.y * size.x + look.x] : '.';
			{
				direction = turn;
				commandstream << "R,R,";
				continue;
			}
		}
	}

	commandstream.seekp(-1, commandstream.cur);
	commandstream << std::endl;
	std::cout << commandstream.str();

	std::map<int64_t, int64_t> memory = program;

	// wake up the vacuum robot
	memory[0] = 2;

	std::vector<int64_t> input, output;

	// TODO: figure out how to compress the command list automatically
	std::string compressed =
		"A,A,B,C,A,C,B,C,A,B\n"
		"L,4,L,10,L,6\n"
		"L,6,L,4,R,8,R,8\n"
		"L,6,R,8,L,10,L,8,L,8\n"
		"n\n";
	for (const char c : compressed)
		input.push_back(c);

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		state = RunInstruction(memory, input, output, pc, relativebase);
		if (!output.empty() && output[0] < 128)
		{
			std::cout << char(output[0]);
			output.clear();
		}
	}
	while (state == State::Run);

	std::cout << "Part 2: " << output[0] << " dust" << std::endl;
}

int main()
{
	std::map<int64_t, int64_t> program;
	ReadInput(program, std::cin);

	std::string map;
	Point size, position;
	Direction direction;
	BuildMap(program, map, size, position, direction);

	Part1(program, map, size, position, direction);
	Part2(program, map, size, position, direction);

	return 0;
}
