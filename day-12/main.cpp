#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <array>
#include <functional>

struct Position
{
    int x;
    int y;

    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y;
    }

    static constexpr std::array<Position, 4> getPositionDeltas()
    {
        return {{ {0, -1}, {1, 0}, {0, 1}, {-1, 0} }};
    }
};

namespace std
{
    template <>
    struct hash<Position>
    {
        size_t operator()(Position const& p) const
        {
            return (size_t)p.x << 32 | (size_t)p.y;
        }
    };
}

struct Region
{
    std::unordered_set<Position> positions;

    size_t getArea() const
    {
        return positions.size();
    }

    size_t getPerimeter() const
    {
        size_t perimeter = 0;
        for (auto const pos : positions)
        {
            for (auto const [dx, dy] : Position::getPositionDeltas())
                perimeter += 1 - positions.count({pos.x + dx, pos.y + dy});
        }
        return perimeter;
    }

    size_t getNumSides() const
    {
        return 1;
    }
};

class Farm
{
public:
    void loadFromFile(const char* path)
    {
        map.clear();

        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file " << path << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            height++;
            width = line.length();

            for (char c : line)
                map.push_back(c);
        }
    }

    std::vector<Region> getRegions() const
    {
        std::vector<Region> regions;
        std::unordered_set<Position> visitedPositions;
        
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                if (visitedPositions.count({x, y}))
                    continue;
                
                Region currentRegion = getRegionContaining({x, y});

                for (auto pos : currentRegion.positions)
                    visitedPositions.insert(pos);
                
                regions.emplace_back(std::move(currentRegion));
            }
        }

        return regions;
    }

private:
    char at(Position p) const
    {
        if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height)
            return 0;

        return map[p.y * width + p.x];
    }

    Region getRegionContaining(Position pos) const
    {
        Region region;
        const char targetChar = at(pos);
        
        floodFillFrom(pos, targetChar, region);

        return region;
    }

    void floodFillFrom(Position pos, char target, Region& currentRegion) const
    {
        if (at(pos) != target || currentRegion.positions.count(pos) > 0)
            return;
        
        currentRegion.positions.insert(pos);
        
        for (auto const [dx, dy] : Position::getPositionDeltas())
        {
            Position newPos = { pos.x + dx, pos.y + dy };
            floodFillFrom(newPos, target, currentRegion);
        }
    }

    std::vector<char> map;
    int width = 0;
    int height = 0;
};

class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        farm.loadFromFile(path);
    }

    size_t part1() const
    {
        return partCommon([this](Region const& r) { return this->regionPricePart1(r); });
    }

    size_t part2() const
    {
        return partCommon([this](Region const& r) { return this->regionPricePart2(r); });
    }

private:
    size_t partCommon(std::function<size_t(Region const&)> price) const
    {
        size_t sum = 0;
        for (auto r : farm.getRegions())
        {
            sum += price(r);
        }
        return sum;
    }

    size_t regionPricePart1(Region const& r) const
    {
        return r.getArea() * r.getPerimeter();
    }

    size_t regionPricePart2(Region const& r) const
    {
        return r.getArea() * r.getNumSides();
    }

    Farm farm;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
