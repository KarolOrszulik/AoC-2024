#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

namespace std
{
    template<>
    struct hash<std::pair<int, int>>
    {
        size_t operator()(std::pair<int,int> const& p) const
        {
            return std::hash<int>{}(p.first) << 16 | std::hash<int>{}(p.second);
        }
    };
}

class Heightmap
{
public:
    static constexpr int TRAIL_START = 0;
    static constexpr int TRAIL_END   = 9;

    void loadFromFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            height++;
            width = line.length();
            for (char c : line)
            {
                map.push_back(c - '0');
            }
        }
    }

    int at(int x, int y) const
    {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return -1;
        
        return map[y * width + x];
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

private:
    std::vector<int> map;
    int width = 0;
    int height = 0;
};


class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        heightmap.loadFromFile(path);
    }

    size_t part1()
    {
        size_t sum = 0;
        for (int y = 0; y < heightmap.getHeight(); y++)
        {
            for (int x = 0; x < heightmap.getWidth(); x++)
            {
                if (heightmap.at(x, y) == Heightmap::TRAIL_START)
                {
                    visitedTrailEnds.clear();
                    size_t score = countTrailsStartingAt(x, y, Heightmap::TRAIL_START);
                    sum += score;
                }
            }
        }
        return sum;
    }

    size_t part2() const
    {
        return 0;
    }

private:
    size_t countTrailsStartingAt(int x, int y, int height = Heightmap::TRAIL_START)
    {
        if (heightmap.at(x, y) != height)
        {
            return 0;
        }

        if (height == Heightmap::TRAIL_END && !hasVisitedTrailEndAt(x, y))
        {
            visitedTrailEnds.insert({x, y});
            return 1;
        }
        
        size_t trails = 0;
        trails += countTrailsStartingAt(x+1, y, height+1);
        trails += countTrailsStartingAt(x-1, y, height+1);
        trails += countTrailsStartingAt(x, y+1, height+1);
        trails += countTrailsStartingAt(x, y-1, height+1);
        return trails;
    }

    bool hasVisitedTrailEndAt(int x, int y)
    {
        return visitedTrailEnds.count({x, y}) != 0;
    }

    Heightmap heightmap;
    std::unordered_set<std::pair<int, int>> visitedTrailEnds;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
