// https://adventofcode.com/2019/day/13

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <algorithm>

#include <Windows.h>

#define INTERACTIVE_MODE 0

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

enum class Tile : int8_t
{
	Empty = 0,
	Wall = 1,
	Block = 2,
	Paddle = 3,
	Ball = 4,
	Count
};

CHAR_INFO tileInfo[int(Tile::Count)]
{
	{ ' ', 0 },
	{ '#', FOREGROUND_RED },
	{ '$', FOREGROUND_RED | FOREGROUND_GREEN },
	{ '=', FOREGROUND_BLUE },
	{ 'O', FOREGROUND_GREEN },
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

// PART 1
void Part1(const std::map<int64_t, int64_t>& program)
{
	std::map<int64_t, int64_t> memory = program;

	std::unordered_map<Point, Tile> screen;

	std::vector<int64_t> input, output;

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
		state = RunInstruction(memory, input, output, pc, relativebase);

		if (state == State::Run && output.size() == 3)
		{
			screen[{short(output[0]), short(output[1])}] = Tile(output[2]);
			output.clear();
		}
	}
	while (state == State::Run);

	int blocks = 0;
	for (const auto &pixel : screen)
	{ 
		if (pixel.second == Tile::Block)
			++blocks;
	}
	std::cout << "Part 1: blocks=" << blocks << std::endl;
}

// PART 2
void Part2(const std::map<int64_t, int64_t>& program)
{
	std::map<int64_t, int64_t> memory = program;
	memory[0] = 2;

	// set up the handles for reading/writing:
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);

	// hide the cursor
	CONSOLE_CURSOR_INFO cursorInfo = { 100, FALSE };
	SetConsoleCursorInfo(hOut, &cursorInfo);

	std::vector<int64_t> input, output;
	input.push_back(0);

	int64_t score = 0;

	int64_t ball_x = 0;
	int64_t paddle_x = 0;

	int64_t pc = 0;
	int64_t relativebase = 0;
	State state = State::Run;
	do
	{
#if INTERACTIVE_MODE == 0
		// automatic mode
		input.clear();
		input.push_back(ball_x - paddle_x);
#else
		// interactive mode
		DWORD numEvents = 0;
		if (GetNumberOfConsoleInputEvents(hIn, &numEvents) && numEvents > 0)
		{
			INPUT_RECORD eventBuffer[16];
			ReadConsoleInput(hIn, eventBuffer, 16, &numEvents);
			for (DWORD i = 0; i < numEvents; ++i)
			{
				switch (eventBuffer[i].EventType)
				{
				case KEY_EVENT:
					if (eventBuffer[i].Event.KeyEvent.bKeyDown)
					{
						switch (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode)
						{
						case 'A':
						case VK_LEFT:
							input.clear();
							input.push_back(-1);
							break;

						case 'D':
						case VK_RIGHT:
							input.clear();
							input.push_back(+1);
							break;

						case 'S':
						case VK_UP:
							input.clear();
							input.push_back(0);
							break;
						}
					}
				}
			}
		}
#endif

		state = RunInstruction(memory, input, output, pc, relativebase);

		if (state == State::Run && output.size() == 3)
		{
			if (output[0] == -1 && output[1] == 0)
			{
				score = output[2];
				TCHAR buf[16] = { 0 };
				_itow_s(int(score), buf, 10);
				DWORD written;
				WriteConsoleOutputCharacter(hOut, buf, DWORD(wcslen(buf)), { 0, 1 }, &written);
			}
			else
			{
				if (Tile(output[2]) == Tile::Ball)
					ball_x = output[0];
				else if (Tile(output[2]) == Tile::Paddle)
					paddle_x = output[0];
				SMALL_RECT writeArea = { short(output[0]), short(output[1] + 2), short(output[0]), short(output[1] + 2) };
				WriteConsoleOutput(hOut, &tileInfo[output[2]], { 1, 1 }, { 0, 0 }, &writeArea);
			}
			output.clear();
		}
	}
	while (state == State::Run);

	std::cout << "Part 2: score=" << score << std::endl;
}

int main()
{
	std::ifstream inputFile;
	inputFile.open("input.txt", std::ios::in);
	std::map<int64_t, int64_t> program;
	ReadInput(program, inputFile);

	Part1(program);
	Part2(program);

	return 0;
}
