#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <optional>
#include <unordered_map>

struct vec2l
{
    long long x = 0;
    long long y = 0;

    vec2l operator+(vec2l const& other) const
    {
        return vec2l { x + other.x, y + other.y };
    }

    bool operator==(vec2l const& other) const
    {
        return x == other.x && y == other.y;
    }
};

namespace std
{
    template<>
    struct hash<vec2l>
    {
        size_t operator()(vec2l const& vec) const
        {
            return vec.x << 32 | vec.y;
        }
    };
}


enum class Tile
{
    INVALID = 0,
    EMPTY = '.',
    WALL = '#',
    BOX = 'O',
    ROBOT = '@',
};

enum class Direction
{
    NORTH = '^',
    EAST = '>',
    SOUTH = 'v',
    WEST = '<',
};

class Warehouse
{
public:
    void loadFromFile(const char* path)
    {
        std::ifstream file(path);
        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }

        tiles.clear();
        directions = std::queue<Direction>(); // why does it not have a clear() method?
        size.x = 0;
        size.y = 0;

        bool doneReadingMap = false;
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty())
            {
                doneReadingMap = true;
                continue;
            }

            if (!doneReadingMap)
            {
                size.x = line.length();

                for (int i = 0; i < line.length(); i++)
                {
                    Tile tile = static_cast<Tile>(line[i]);
                    if (tile == Tile::ROBOT)
                        robotPos = {i, size.y};

                    tiles.push_back(tile);
                }

                size.y++;
            }
            else
            {
                for (char c : line)
                {
                    directions.push(static_cast<Direction>(c));
                }
            }
        }
    }

    bool doneMoving() const
    {
        return directions.empty();
    }

    void moveRobot()
    {
        auto optPos = firstEmptySpotToMove();

        vec2l delta = getDeltas()[opposite(directions.front())];
        directions.pop();

        if (!optPos.has_value())
            return;
        
        vec2l pos = optPos.value();

        Tile moved;
        do
        {
            moved = at(pos + delta);           
            setTile(pos, moved);
            setTile(pos + delta, Tile::EMPTY);

            if (at(pos) == Tile::ROBOT)
                robotPos = pos;
                
            pos = pos + delta;

        } while (moved != Tile::ROBOT);
    }

    size_t sumOfGPScoordinates() const
    {
        size_t total = 0;
        for (int y = 0; y < size.y; y++)
        {
            for (int x = 0; x < size.x; x++)
            {
                if (at({x, y}) != Tile::BOX)
                    continue;
                
                total += GPScoordinate({x, y});
            }
        }
        return total;
    }

private:

    std::optional<vec2l> firstEmptySpotToMove() const
    {
        vec2l pos = robotPos;
        vec2l delta = getDeltas()[directions.front()];
        while (true)
        {
            pos = pos + delta;
            Tile atPos =  at(pos);

            if (atPos == Tile::EMPTY)
                return pos;
            
            if (atPos == Tile::INVALID || atPos == Tile::WALL)
                return {};
        }
    }

    static std::unordered_map<Direction, vec2l> getDeltas()
    {
        return {
            { Direction::NORTH, { 0, -1} },
            { Direction::SOUTH, { 0,  1} },
            { Direction::EAST,  { 1,  0} },
            { Direction::WEST,  {-1,  0} },
        };
    }

    Tile at(vec2l pos) const
    {
        if (pos.x < 0 || pos.x >= size.x || pos.y < 0 || pos.y >= size.y)
            return Tile::INVALID;
        
        return tiles[pos.x + pos.y * size.x];
    }

    void setTile(vec2l pos, Tile tile)
    {
        if (at(pos) == Tile::INVALID)
            return;
        tiles[pos.x + pos.y * size.x] = tile;
    }

    Direction opposite(Direction dir) const
    {
        switch (dir)
        {
            case Direction::NORTH:
                return Direction::SOUTH;
            case Direction::SOUTH:
                return Direction::NORTH;
            case Direction::EAST:
                return Direction::WEST;
            case Direction::WEST:
                return Direction::EAST;
        }
        throw; // unreachable
    }

    size_t GPScoordinate(vec2l pos) const
    {
        return pos.x + 100*pos.y;
    }

    std::vector<Tile> tiles;
    std::queue<Direction> directions;
    vec2l size;
    vec2l robotPos;
};

class Solution
{
public:
    void setInputFilePath(const char* path)
    {
        inputPath = path;
    }

    size_t part1()
    {
        warehouse.loadFromFile(inputPath.c_str());

        while (!warehouse.doneMoving())
        {
            warehouse.moveRobot();
        }

        return warehouse.sumOfGPScoordinates();
    }

    size_t part2()
    {
        return 0;
    }

private:
    std::string inputPath;
    Warehouse warehouse;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
