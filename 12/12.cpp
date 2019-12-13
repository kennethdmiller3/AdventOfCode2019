// https://adventofcode.com/2019/day/12

#include <iostream>
#include <vector>
#include <unordered_set>
#include <array>
#include <assert.h>

// position and velocity data for a "moon"
struct Moon
{
    int pos_x;
    int pos_y;
    int pos_z;
    int vel_x;
    int vel_y;
    int vel_z;
};

// get the sign of a value
int Sign(int x)
{
    if (x > 0)
        return +1;
    if (x < 0)
        return -1;
    return 0;
}

// greatest common divisor of two values
// (non-recursive implementation)
int64_t GCD(int64_t a, int64_t b)
{
    for (;;)
    {
        if (a == b)
            return a;
        if (a > b)
            a -= b;
        else
            b -= a;
    }
}

namespace std
{
    template<> struct hash<std::array<int, 8>>
    {
        std::size_t operator()(const std::array<int, 8>& k) const
        {
            std::size_t h = 0;
            for (int i = 0; i < 8; ++i)
                h ^= hash<int>()(k[i] << i);
            return h;
        }
    };
}

void ReadInput(std::array<Moon, 4>& output, std::istream& input)
{
    char line[256];
    int id = 0;
    while (input.getline(line, 256))
    {
        int x, y, z;
        sscanf_s(line, "<x=%d, y=%d, z=%d>", &x, &y, &z);
        output[id++] = { x, y, z, 0, 0, 0 };
    }
}

void Step(std::array<Moon, 4>& state)
{
    // apply gravity
    for (int id1 = 0; id1 < 4; ++id1)
    {
        Moon& moon = state[id1];

        for (int id2 = 0; id2 < 4; ++id2)
        {
            if (id1 == id2)
                continue;

            const Moon& other = state[id2];

            moon.vel_x += Sign(other.pos_x - moon.pos_x);
            moon.vel_y += Sign(other.pos_y - moon.pos_y);
            moon.vel_z += Sign(other.pos_z - moon.pos_z);
        }
    }

    // apply velocity
    for (Moon& moon : state)
    {
        moon.pos_x += moon.vel_x;
        moon.pos_y += moon.vel_y;
        moon.pos_z += moon.vel_z;
    }
}

void Part1(const std::array<Moon, 4>& moons)
{
    std::array<Moon, 4> state = moons;

    for (int step = 0; step < 1000; ++step)
    {
        Step(state);
    }

    int energy = 0;
    for (const Moon& moon : state)
    {
        int potential = abs(moon.pos_x) + abs(moon.pos_y) + abs(moon.pos_z);
        int kinetic = abs(moon.vel_x) + abs(moon.vel_y) + abs(moon.vel_z);
        energy += potential * kinetic;
    }

    std::cout << "Part1: energy=" << energy << std::endl;
}

void Part2(const std::array<Moon, 4>& moons)
{
    std::array<Moon, 4> state = moons;

    std::unordered_set<std::array<int, 8>> history_x, history_y, history_z;

    int step = 0;
    int64_t repeat_x = -1, repeat_y = -1, repeat_z = -1;
    while(repeat_x < 0 || repeat_y < 0 || repeat_z < 0)
    {
        Step(state);

        // check for a repeat on each axis
        if (repeat_x < 0)
        {
            // look for a repeat of the x-axis state
            std::array<int, 8> state_x = {
                state[0].pos_x, state[0].vel_x,
                state[1].pos_x, state[1].vel_x,
                state[2].pos_x, state[2].vel_x,
                state[3].pos_x, state[3].vel_x
            };
            if (history_x.find(state_x) != history_x.end())
            {
                std::cout << "x axis repeated on step " << step << std::endl;
                repeat_x = step;
            }
            else
            {
                history_x.emplace(state_x);
            }
        }
        if (repeat_y < 0)
        {
            // look for a repeat of the y-axis state
            std::array<int, 8> state_y = {
                state[0].pos_y, state[0].vel_y,
                state[1].pos_y, state[1].vel_y,
                state[2].pos_y, state[2].vel_y,
                state[3].pos_y, state[3].vel_y
            };
            if (history_y.find(state_y) != history_y.end())
            {
                std::cout << "y axis repeated on step " << step << std::endl;
                repeat_y = step;
            }
            else
            {
                history_y.emplace(state_y);
            }
        }
        if (repeat_z < 0)
        {
            // look for a repeat of the z-axis state
            std::array<int, 8> state_z = {
                state[0].pos_z, state[0].vel_z,
                state[1].pos_z, state[1].vel_z,
                state[2].pos_z, state[2].vel_z,
                state[3].pos_z, state[3].vel_z
            };
            if (history_z.find(state_z) != history_z.end())
            {
                std::cout << "z axis repeated on step " << step << std::endl;
                repeat_z = step;
            }
            else
            {
                history_z.emplace(state_z);
            }
        }

        ++step;
    }

    // compute the least common multiple of the repeats;
    // the values can get very large so we need 64-bit integers
    int64_t gcd_xy = GCD(repeat_x, repeat_y);
    int64_t repeat_xy = repeat_x / gcd_xy * repeat_y;
    int64_t gcd_xyz = GCD(repeat_xy, repeat_z);
    int64_t repeat_xyz = repeat_xy / gcd_xyz * repeat_z;
    
    std::cout << "Part2: steps=" << repeat_xyz << std::endl;
}

int main()
{
    std::array<Moon, 4> moons;
    ReadInput(moons, std::cin);

    Part1(moons);
    Part2(moons);

    return 0;
}
