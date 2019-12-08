// 08.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

// image dimensions
constexpr size_t W = 25;
constexpr size_t H = 6;
constexpr size_t layersize = W * H;

enum Color
{
	Black = 0,
	White = 1,
	Transparent = 2
};

// read the image from the input stream
void ReadInput(std::string& output, std::istream& input)
{
	input >> output;
}

// PART 1
void Part1(const std::string& image)
{
	// number of layers
	size_t len = size_t(image.length());
	size_t layers = len / layersize;

	// find the layer with the fewest zeroes
	size_t bestzeroes = SIZE_MAX;
	size_t bestlayer = 0;
	for (size_t layer = 0; layer < layers; ++layer)
	{
		size_t zeroes = 0;
		for (size_t i = (layer) * layersize; i < (layer + 1) * layersize; ++i)
		{
			if (image[i] == '0')
				++zeroes;
		}
		if (bestzeroes > zeroes)
		{
			bestzeroes = zeroes;
			bestlayer = layer;
		}
	}

	// count the number of pixels of each color
	size_t counts[3] = { 0 };
	for (size_t i = (bestlayer) * layersize; i < (bestlayer + 1) * layersize; ++i)
	{
		++counts[image[i] - '0'];
	}

	std::cout << "Part 1: ones * twos = " << counts[1] * counts[2] << std::endl;
}

// PART 2
void Part2(const std::string& image)
{
	// number of layers
	size_t len = size_t(image.length());
	size_t layers = len / layersize;

	std::cout << "Part 2:" << std::endl;

	for (size_t y = 0; y < H; ++y)
	{
		for (size_t x = 0; x < W; ++x)
		{
			// composite the layers
			char value = ' ';
			for (size_t layer = 0; layer < layers; ++layer)
			{
				char pixel = image[layer * layersize + y * W + x] - '0';
				if (pixel == White)
				{
					value = '#';
					break;
				}
				else if (pixel == Black)
				{
					value = '.';
					break;
				}
			}
			std::cout << value;
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::string image;
	ReadInput(image, std::cin);

	Part1(image);
	Part2(image);

	return 0;
}
