// https://adventofcode.com/2019/day/16

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

void ReadInput(std::vector<int>& output, std::istream& input)
{
    std::string line;
    std::getline(input, line);
    for (const char c : line)
    {
        output.push_back(c - '0');
    }
}

void FFT(std::vector<int>& in, int start = 0)
{
    int count = int(in.size());

    std::vector<int> out;
    out.resize(count);

    for (int phase = 1; phase <= 100; ++phase)
    {
        std::cout << phase << " ";

        for (int i = 0; i < start; ++i)
        {
            out[i] = 0;
        }

#if 1
        // calculate partial sums
        std::vector<int> sums;
        sums.resize(count - start + 1);
        sums[0] = 0;
        for (int i = 0; i < count - start; ++i)
        {
            sums[i + 1] = sums[i] + in[start + i];
        }

        for (int i = start; i < count; ++i)
        {
            int sum = 0;

            // sum values within spans using the partial sums
            int span = 1;
            int j_start = i;
            const int j_step = i + 1;
            while (j_start < count)
            {
                int j_next = std::min(j_start + j_step, count);
                switch (span & 3)
                {
                case 1:
                    sum += sums[j_next - start] - sums[j_start - start];
                    break;
                case 3:
                    sum -= sums[j_next - start] - sums[j_start - start];
                    break;
                }
                j_start = j_next;
                ++span;
            }

            sum = abs(sum % 10);
            out[i] = sum;
        }
#else
		// naive O(N^2), very slow!
		for (int i = start; i < count; ++i)
		{
            int sum = 0;

            for (int j = i; j < count; ++j)
			{
				switch (((j + 1) / (i + 1)) & 3)
				{
				case 1:
					sum += in[j];
					break;

				case 3:
					sum -= in[j];
					break;
	            }
            }

            sum = abs(sum % 10);
            out[i] = sum;
        }
#endif

        std::swap(in, out);
    }

    std::cout << std::endl;
}

void Part1(const std::vector<int>& signal)
{
    std::vector<int> result;

    result = signal;
    FFT(result);

    std::cout << "Part 1: ";
    for (int i = 0; i < 8; ++i)
    {
        std::cout << result[i];
    }
    std::cout << std::endl;
}

void Part2(const std::vector<int>& signal)
{
    int offset = 0;
    for (int i = 0; i < 7; ++i)
    {
        offset *= 10;
        offset += signal[i];
    }
    std::cout << "Part 2: offset " << offset << std::endl;

    std::vector<int> result;
    for (int i = 0; i < 10000; ++i)
    {
        result.insert(result.end(), signal.begin(), signal.end());
    }
    FFT(result, offset);

    std::cout << "Part 2: message ";
    for (int i = offset; i < offset + 8; ++i)
    {
        std::cout << result[i];
    }
    std::cout << std::endl;
}

int main()
{
    std::vector<int> signal;
    ReadInput(signal, std::cin);

    Part1(signal);
    Part2(signal);

    return 0;
}
