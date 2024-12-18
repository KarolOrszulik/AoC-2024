#include <iostream>
#include <fstream>
#include <vector>

static constexpr bool USE_TEST_INPUT = false;

static constexpr int MAP_SIZE = USE_TEST_INPUT ? 7 : 71;
static constexpr int PART_1_WALLS = USE_TEST_INPUT ? 12 : 1024;
static constexpr const char* INPUT_FILE = USE_TEST_INPUT ? "../input_small.txt" : "../input.txt";

struct Position
{
    int x;
    int y;

    bool isValid() const
    {
        return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE;
    }
};

class Tile
{
public:
    constexpr static long MAX_DISTANCE = __INT64_MAX__;

    enum class Type
    {
        SPACE,
        WALL
    };

    long getDistance() const
    {
        if (isWall())
            return MAX_DISTANCE;
        
        return distance;
    }

    void setDistance(long newDistance)
    {
        if (isWall())
            return;
        
        distance = newDistance;
    }

    void makeWall()
    {
        type = Type::WALL;
    }

    bool isWall() const
    {
        return type == Type::WALL;
    }

private:
    Type type = Type::SPACE;
    long distance = MAX_DISTANCE;
};


class Map
{
public:
    Map()
    {
        resetMap();
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

    void resetMap()
    {
        tiles = std::vector<Tile>(MAP_SIZE * MAP_SIZE);
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
        at(to).setDistance(0);

        bool doneOptimizing = false;
        while (!doneOptimizing)
            doneOptimizing = optimizePaths();

        return at(from).getDistance();
    }

    bool existsPathFromTo(Position from, Position to)
    {
        at(to).setDistance(0);

        bool doneOptimizing = false;
        while (!doneOptimizing)
        {
            doneOptimizing = optimizePaths();
            if (at(from).getDistance() != Tile::MAX_DISTANCE)
                return true;
        }
        return false;
    }

    Position ithWall(int i) const
    {
        return walls[i];
    }

    int getNumWalls() const
    {
        return walls.size();
    }

private:
    bool optimizePaths()
    {
        bool doneOptimizing = true;
        for (int y = 0; y < MAP_SIZE; y++)
        {
            for (int x = 0; x < MAP_SIZE; x++)
            {
                bool didOptimizePoint = optimizeAroundPoint({x, y});
                if (didOptimizePoint)
                    doneOptimizing = false;
            }
        }
        return doneOptimizing;
    }

    bool optimizeAroundPoint(Position pos)
    {
        if (at(pos).isWall())
            return false;
                
        long minDistance = Tile::MAX_DISTANCE;

        for (auto const [dx, dy] : std::initializer_list<std::pair<int, int>>{ {1,0}, {0,1}, {-1,0}, {0,-1}})
        {
            Position neighbour { pos.x+dx, pos.y+dy };
            if (!neighbour.isValid() || at(neighbour).isWall())
                continue;
            
            long distance = at(neighbour).getDistance();
            if (distance == Tile::MAX_DISTANCE)
                continue;

            if (distance < minDistance)
                minDistance = distance;
        }

        if (minDistance == Tile::MAX_DISTANCE)
            return false;
        
        minDistance += 1; // one more step to end up on current tile
        
        long distToPos = at(pos).getDistance();
        if (minDistance < distToPos)
        {
            at(pos).setDistance(minDistance);
            return true;
        }
        return false;
    }

    Tile& at(Position pos)
    {
        return tiles[pos.x + pos.y * MAP_SIZE];
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

    std::string part2() const
    {
        Map map;
        map.loadWallsFromFile(inputPath.c_str());
        for (int i = PART_1_WALLS; i < map.getNumWalls(); i++)
        {
            map.placeWalls(i+1);
            if (!map.existsPathFromTo({0,0}, {MAP_SIZE-1, MAP_SIZE-1}))
            {
                auto const [x, y] = map.ithWall(i);
                return std::to_string(x) + "," + std::to_string(y);
            }
            map.resetMap();
        }
        return "Not found";
    }

private:
    std::string inputPath;
};

int main()
{
    Solution solution;
    solution.setInputFilePath(INPUT_FILE);

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
