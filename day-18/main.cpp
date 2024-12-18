#include <iostream>
#include <fstream>
#include <vector>
#include <limits>

static constexpr bool IS_TEST_INPUT = false;

static constexpr int MAP_SIZE = IS_TEST_INPUT ? 7 : 71;
static constexpr int PART_1_WALLS = IS_TEST_INPUT ? 12 : 1024;

struct Position
{
    int x;
    int y;

    bool operator==(Position const& other) const
    {
        return x == other.x && y == other.y;
    }

    bool isValid() const
    {
        return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
    }
};

class Tile
{
public:
    static constexpr long MAX_DISTANCE = std::numeric_limits<long>::max();

    enum class Type
    {
        INVALID,
        EMPTY,
        WALL,
    };

    Tile(Type t = Type::INVALID)
        : type(t), distanceToTarget(MAX_DISTANCE)
    {}

    Type getType() const
    {
        return type;
    }

    void makeWall()
    {
        type = Type::WALL;
    }

    long getDistanceToTarget() const
    {
        return distanceToTarget;
    }

    void setDistanceToTarget(long newDistance)
    {
        if (type != Type::EMPTY)
            return;

        if (newDistance < 0)
            newDistance = MAX_DISTANCE;
        
        distanceToTarget = newDistance;
    }

private:
    Type type;
    long distanceToTarget;
};

class Map
{
public:
    Map()
    {
        tiles = std::vector<Tile>(MAP_SIZE * MAP_SIZE, Tile(Tile::Type::EMPTY));
    }

    void loadWallsFromFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }

        walls.clear();

        int x, y;
        char comma;
        while (file >> x >> comma >> y)
        {
            walls.push_back({x, y});
        }
    }

    void placeWalls(int numWalls = -1)
    {
        if (numWalls == -1)
            numWalls = walls.size();

        for (int i = 0; i < numWalls; i++)
        {
            Position pos = walls[i];
            at(pos).makeWall();
        }
    }

    size_t distanceFromTo(Position from, Position to)
    {
        at(to).setDistanceToTarget(0);

        for (int i = 0; i < MAP_SIZE * MAP_SIZE; i++)
        {
            optimizePaths();
        }

        return at(from).getDistanceToTarget();
    }

    Tile& at(Position pos)
    {
        return tiles[pos.x + pos.y * MAP_SIZE];
    }

private:
    bool optimizePaths()
    {
        bool doneOptimizing = true;
        for (int y = 0; y < MAP_SIZE; y++)
        {
            for (int x = 0; x < MAP_SIZE; x++)
            {
                if (at({x, y}).getType() == Tile::Type::WALL)
                    continue;
                
                long minDistance = Tile::MAX_DISTANCE;
                for (auto const [dx, dy] : std::initializer_list<std::pair<int, int>>{ {1,0}, {0,1}, {-1,0}, {0,-1}})
                {
                    Position pos { x+dx, y+dy };
                    if (!pos.isValid())
                        continue;

                    Tile::Type type = at(pos).getType();

                    if (type == Tile::Type::WALL)
                        continue;
                    
                    const long distance = at(pos).getDistanceToTarget();
                    if (distance < minDistance)
                        minDistance = distance;
                }
                if (minDistance == Tile::MAX_DISTANCE)
                    continue;
                
                minDistance++;

                if (minDistance < at({x,y}).getDistanceToTarget())
                {
                    doneOptimizing = false; // we were not done optimizing because an update was necessary
                    at({x,y}).setDistanceToTarget(minDistance);
                }
            }
        }
        return doneOptimizing;
    }

    std::vector<Tile> tiles;
    std::vector<Position> walls;
};

class Solution
{
public:
    void setInputFilePath(const char* path)
    {
        inputPath = path;
    }

    size_t part1() const
    {
        Map map;
        map.loadWallsFromFile(inputPath.c_str());
        map.placeWalls(PART_1_WALLS);
        return map.distanceFromTo({0,0}, {MAP_SIZE-1, MAP_SIZE-1});
    }

    size_t part2() const
    {
        return 0;
    }

private:
    std::string inputPath;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
