// https://adventofcode.com/2019/day/15

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#define VISUALIZE 0

#if VISUALIZE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

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

enum class Movement
{
	None = 0,
	North = 1,
	South = 2,
	West = 3,
	East = 4,
	Count
};

enum class Status
{
	HitWall = 0,
	MovedOneStep = 1,
	FoundOxygenSystem = 2,
};

struct Point
{
	short x;
	short y;
};

Point movementStep[int(Movement::Count)] =
{
	{ 0, 0 },
	{ 0, -1 },
	{ 0, +1 },
	{ -1, 0 },
	{ +1, 0 },
};

Movement movementBack[int(Movement::Count)] =
{
	Movement::None,
	Movement::South,
	Movement::North,
	Movement::East,
	Movement::West,
};

Movement movementTurnRight[int(Movement::Count)] =
{
	Movement::None,
	Movement::East,
	Movement::West,
	Movement::North,
	Movement::South,
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

Point Explore(const std::map<int64_t, int64_t>& program, std::unordered_map<Point, Status>& environment, std::unordered_map<Point, Movement>& entered)
{
	std::map<int64_t, int64_t> memory = program;
	std::vector<Movement> stack;

	std::vector<int64_t> input, output;

#if VISUALIZE
	// set up the handles for reading/writing:
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

	// hide the cursor
	CONSOLE_CURSOR_INFO cursorInfo = { 100, FALSE };
	SetConsoleCursorInfo(hOut, &cursorInfo);
#endif

	Point position = { 0, 0 };
	Movement movement = Movement::North;
	stack.push_back(movement);

	environment[position] = Status::MovedOneStep;
	entered[position] = Movement::None;

	Point goal = { 0, 0 };

#if VISUALIZE
	constexpr int x_offset = 25, y_offset = 25;

	{
		WORD attrib = FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED;
		wchar_t tile = '^';
		DWORD written;
		WriteConsoleOutputAttribute(hOut, &attrib, 1, { short(position.x + x_offset), short(position.y + y_offset) }, &written);
		WriteConsoleOutputCharacter(hOut, &tile, 1, { short(position.x + x_offset), short(position.y + y_offset) }, &written);
	}
#endif

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		if (input.empty())
		{
			input.push_back(int64_t(movement));
		}

		state = RunInstruction(memory, input, output, pc, relativebase);

		if (state == State::Run && !output.empty())
		{
			Status status = Status(output[0]);

			Point prev_pos = position;

			// next position
			Point step = movementStep[int(movement)];
			Point tile_pos = { position.x + step.x, position.y + step.y };

			// is this a newly-visited tile?
			bool newTile = environment.find(tile_pos) == environment.end();

			// set the content of the tile
			environment[tile_pos] = status;

#if VISUALIZE
			wchar_t tile_char;
			WORD tile_attrib;
#endif
			switch (status)
			{
			case Status::HitWall:
#if VISUALIZE
				tile_char = '#';
				tile_attrib = FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE;
#endif
				break;

			case Status::MovedOneStep:
#if VISUALIZE
				tile_char = '.';
				tile_attrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
#endif
				position = tile_pos;
				break;

			case Status::FoundOxygenSystem:
#if VISUALIZE
				tile_char = 'O';
				tile_attrib = FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_GREEN;
#endif
				position = tile_pos;
				goal = position;
				break;
			}

#if VISUALIZE
			if (newTile)
			{
				// display the tile
				DWORD written;
				WriteConsoleOutputCharacter(hOut, &tile_char, 1, { short(tile_pos.x + x_offset), short(tile_pos.y + y_offset) }, &written);
				WriteConsoleOutputAttribute(hOut, &tile_attrib, 1, { short(tile_pos.x + x_offset), short(tile_pos.y + y_offset) }, &written);
			}
#endif

			// if entering the tile for the first time...
			if (entered.find(position) == entered.end())
			{
				// save the direction we entered
				entered[position] = movement;
				stack.push_back(movement);
			}
			else if (stack.empty())
			{
				// halt when no moves available
				state = State::Halt;
			}
			else
			{
				// next movement
				movement = stack.back();
				stack.pop_back();
				movement = movementTurnRight[int(movement)];
				if (movement == movementBack[int(entered[position])])
				{
					movement = movementTurnRight[int(movement)];
				}
				if (movement == entered[position])
				{
					movement = movementBack[int(entered[position])];
				}
				else
				{
					stack.push_back(movement);
				}
			}

			output.clear();

			input.clear();
			input.push_back(int64_t(movement));
		}
	}
	while (state == State::Run);

	return goal;
}

// PART 1
void Part1(const std::map<int64_t, int64_t>& program, const std::unordered_map<Point, Status>& environment, const std::unordered_map<Point, Movement>& entered, Point goal)
{
	// trace back from the goal
	Point position = goal;
	int steps = 0;
	for (;;)
	{
		Movement movement = entered.at(position);
		if (movement == Movement::None)
			break;
		Point step = movementStep[int(movement)];
		position.x -= step.x;
		position.y -= step.y;
		++steps;
	}

	std::cout << "Part 1: " << steps << " steps" << std::endl;
}

// PART 2
void Part2(const std::map<int64_t, int64_t>& program, const std::unordered_map<Point, Status>& environment, const std::unordered_map<Point, Movement>& entered, Point goal)
{
	std::unordered_map<Point, int> goal_dist;
	goal_dist[goal] = 0;

	std::vector<Point> open_queue;
	open_queue.push_back(goal);
	std::unordered_set<Point> closed_set;

	Point position = goal;

	auto OpenQueuePredicate = [&](Point const& a, Point const& b)
	{
		const int a_dist = goal_dist[a];
		const int b_dist = goal_dist[b];
		if (a_dist > b_dist)
			return true;
		if (a_dist < b_dist)
			return false;
		if (a.y < b.y)
			return true;
		if (a.y > b.y)
			return false;
		if (a.x < b.x)
			return true;
		if (a.x > b.x)
			return false;
		return false;
	};

	int max_dist = 0;

	while (!open_queue.empty())
	{
		std::pop_heap(open_queue.begin(), open_queue.end(), OpenQueuePredicate);
		position = open_queue.back();
		open_queue.pop_back();

		int cur_dist = goal_dist[position];

		// add neighbors
		for (int i = 1; i <= 4; ++i)
		{
			Point step = movementStep[i];
			Point neighbor = { position.x + step.x, position.y + step.y };
			if (environment.at(neighbor) == Status::MovedOneStep && goal_dist.find(neighbor) == goal_dist.end())
			{
				goal_dist[neighbor] = cur_dist + 1;
				max_dist = std::max(max_dist, cur_dist + 1);
				open_queue.push_back(neighbor);
				std::push_heap(open_queue.begin(), open_queue.end(), OpenQueuePredicate);
			}
		}
	}

	std::cout << "Part 2: " << max_dist << " minutes" << std::endl;
}

int main()
{
	std::ifstream inputFile;
	inputFile.open("input.txt", std::ios::in);
	std::map<int64_t, int64_t> program;
	ReadInput(program, inputFile);

	std::unordered_map<Point, Status> environment;
	std::unordered_map<Point, Movement> entered;
	Point goal = Explore(program, environment, entered);

	Part1(program, environment, entered, goal);
	Part2(program, environment, entered, goal);

	return 0;
}
