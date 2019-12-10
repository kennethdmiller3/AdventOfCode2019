// https://adventofcode.com/2019/day/10

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

short rows = 0;
short cols = 0;

struct Point
{
    short x;
    short y;
};

struct Info
{
    Point point;
    float angle;
    int dist2;
};

bool CompareAngleAndDist(const Info &a, const Info &b)
{
	if (a.angle < b.angle)
		return false;
	if (a.angle > b.angle)
		return true;
	if (a.dist2 > b.dist2)
		return false;
	if (a.dist2 < b.dist2)
		return true;
	return false;
}

void ReadInput(std::vector<char>& output, std::istream& input)
{
    std::string line;
    while (std::getline(input, line))
    {
        cols = (short)line.size();
        rows++;
        for (const char c : line)
        {
            output.push_back(c);
        }
    }
}

Point Part1(const std::vector<char>& map)
{
    std::vector<Point> asteroids;

    for (short y = 0; y < rows; ++y)
    {
        for (short x = 0; x < cols; ++x)
        {
            if (map[y * cols + x] == '#')
            {
                // found an asteroid
                asteroids.push_back({ short(x), short(y) });
            }
        }
    }

    int bestcount = 0;
    Point bestpoint;

    for (const Point& current : asteroids)
    {
        int count = 0;

#if 1
        // count visible targets using angles, which is O(N log N)

        std::vector<Info> targets;
        targets.reserve(asteroids.size() - 1);

        // get the position of each asteroid and compute
        // the angle and distance relative to the current
        for (const Point& target : asteroids)
        {
            int dx = target.x - current.x, dy = target.y - current.y;
            if (dx == 0 && dy == 0)
                continue;
            targets.push_back({ target, atan2f(float(dx), float(dy)), dx * dx + dy * dy });
        }

        // sort targets by angle and distance
        std::sort(targets.begin(), targets.end(), CompareAngleAndDist);

        // count unique angles
        float angle = -FLT_MAX;
        for (const Info& target : targets)
        {
            if (angle != target.angle)
            {
                angle = target.angle;
                ++count;
            }
        }
#else
        // original try, which was O(N^2)

        for (const Point& target : asteroids)
        {
            if (target.x == current.x && target.y == current.y)
                continue;

            short x0 = std::min(current.x, target.x);
            short x1 = std::max(current.x, target.x);
            short y0 = std::min(current.y, target.y);
            short y1 = std::max(current.y, target.y);

            // check for blockage
            bool blocked = false;
            for (short y = y0; y <= y1; ++y)
            {
                for (short x = x0; x <= x1; ++x)
                {
                    if (map[y * cols + x] == '#')
                    {
                        if (current.x == x && current.y == y)
                            continue;
                        if (target.x == x && target.y == y)
                            continue;
                        int dx = int(x - current.x);
                        int dy = int(y - current.y);
                        int dtx = int(target.x - current.x);
                        int dty = int(target.y - current.y);
                        int dt2 = dtx * dtx + dty * dty;
                        int dot = dx * dtx + dy * dty;

                        // project onto line
                        if (dx * dt2 == dot * dtx && 
                            dy * dt2 == dot * dty)
                        {
                            // blocked LOS
                            blocked = true;
                            goto done;
                        }
                    }
                }
            }
        done:
            if (!blocked)
            {
                ++count;
            }
        }
#endif

        if (bestcount < count)
        {
            bestcount = count;
            bestpoint = current;
        }
    }

    std::cout << "Part 1: location=" << bestpoint.x << "," << bestpoint.y << " count=" << bestcount << std::endl;
    return bestpoint;
}

void Part2(const std::vector<char>& map, Point station)
{
    std::vector<Info> targets;

    // get the position of each asteroid and compute
    // the angle and distance relative to the station
    for (short y = 0; y < rows; ++y)
    {
        for (short x = 0; x < cols; ++x)
        {
            if (map[y * cols + x] == '#')
            {
                // found an asteroid
                int dx = x - station.x, dy = y - station.y;
                if (dx == 0 && dy == 0)
                    continue;
                targets.push_back({{ short(x), short(y) }, atan2f(float(dx), float(dy)), dx * dx + dy * dy });
            }
        }
    }

    // sort targets by angle and distance
    std::sort(targets.begin(), targets.end(), CompareAngleAndDist);

    // sweep through asteroids, picking off one from each angle group until we find the 200th one
    Point result = station;
    int i = 0;
    int count = 0;
    float angle = -FLT_MAX;
    while (!targets.empty())
    {
        const Info& a = targets[i];
        if (angle != a.angle)
        {
            // moved to a different angle group;
            // pick off the closest asteroid in the group
            if (++count == 200)
            {
                result = a.point;
                break;
            }
            angle = a.angle;
            targets.erase(targets.begin() + i);
        }
        else
        {
            if (++i >= targets.size())
                i = 0;
        }
    }

    std::cout << "Part 2: result=" << result.x * 100 + result.y << std::endl;
}

int main()
{
    std::vector<char> map;
    ReadInput(map, std::cin);

    Point station = Part1(map);
    Part2(map, station);

    return 0;
}
