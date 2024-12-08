#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct Position
{
    int x = 0;
    int y = 0;

    bool operator==(Position const& other) const
    {
        return x == other.x && y == other.y;
    }
};

namespace std
{
    template <>
    struct hash<Position>
    {
        size_t operator()(Position const& p) const
        {
            return ((size_t)p.x << 32) | (size_t)p.y;
        }
    };
}

struct Map
{
    std::unordered_map<char, std::vector<Position>> antennaPositions;
    int width = 0;
    int height = 0;

    void loadFromFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        std::string line;
        for (int y = 0; std::getline(file, line); y++)
        {
            height++;
            width = line.length();
            for (int x = 0; x < line.length(); x++)
            {
                const char c = line[x];

                if (c == '.')
                    continue;

                antennaPositions[c].push_back({x, y});
            }
        }
    }

    bool isValidPosition(Position p) const
    {
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height;
    }
};


class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        map.loadFromFile(path);
    }

    int part1() const
    {
        return countUniqueAntinodes();
    }

    int part2() const
    {
        return countUniqueAntinodesPart2();
    }

private:
    int countUniqueAntinodes() const
    {
        std::unordered_set<Position> antinodePositions;

        for (auto const& [c, antennas] : map.antennaPositions)
        {
            for (int i = 0; i < antennas.size() - 1; i++)
            {
                for (int j = i + 1; j < antennas.size(); j++)
                {
                    const Position first = antennas[i];
                    const Position second = antennas[j];

                    const int run  = second.x - first.x;
                    const int rise = second.y - first.y;

                    const Position antinodeA = {second.x + run, second.y + rise};
                    const Position antinodeB = {first.x - run,  first.y - rise};

                    if (map.isValidPosition(antinodeA))
                        antinodePositions.insert(antinodeA);
                    
                    if (map.isValidPosition(antinodeB))
                        antinodePositions.insert(antinodeB);
                }
            }
        }

        return antinodePositions.size();
    }


    int countUniqueAntinodesPart2() const
    {
        std::unordered_set<Position> antinodePositions;

        for (auto const& [c, antennas] : map.antennaPositions)
        {
            for (int i = 0; i < antennas.size() - 1; i++)
            {
                for (int j = i + 1; j < antennas.size(); j++)
                {
                    const Position origin = antennas[i];
                    const Position reference = antennas[j];

                    antinodePositions.insert(origin);

                    const int run  = reference.x - origin.x;
                    const int rise = reference.y - origin.y;

                    Position tested = reference;
                    while (map.isValidPosition(tested))
                    {
                        antinodePositions.insert(tested);
                        tested.x += run;
                        tested.y += rise;
                    }

                    tested = origin;
                    while (map.isValidPosition(tested))
                    {
                        antinodePositions.insert(tested);
                        tested.x -= run;
                        tested.y -= rise;
                    }
                }
            }
        }

        return antinodePositions.size();
    }

    Map map;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
