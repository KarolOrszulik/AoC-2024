#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

struct Position
{
    int x, y;

    bool operator==(Position const& p) const
    {
        return x == p.x && y == p.y;
    }
};

namespace std
{
    template<>
    struct hash<Position>
    {
        size_t operator()(Position const& p) const
        {
            return (size_t)p.x << 32 | (size_t)p.y;
        }
    };
}



class MazeParser
{
public:
    void parseFile(const char* path)
    {
        std::ifstream file(path);
        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }



        std::string line;
        for (int y = 0; std::getline(file, line); y++)
        {
            width = line.length();

            for (int x = 0; x < width; x++)
            {
                const char c = line[x];
                bool isPath = c == '.';
                const Position pos = {x, y};

                if (c == START_CHAR)
                {
                    startPos = pos;
                    isPath = true;
                }
                else if (c == END_CHAR)
                {
                    endPos = pos;
                    isPath = true;
                }

                if (isPath)
                    pathPositions.insert(pos);
            }

            height++;
        }
    }

    std::pair<int, int> getSize() const
    {
        return { width, height };
    }

    bool isPath(Position pos) const
    {
        return pathPositions.count(pos);
    }

    int getNumPathTiles() const
    {
        return pathPositions.size();
    }

    std::unordered_set<Position> const& getPathPositions() const
    {
        return pathPositions;
    }

    Position getStartPos() const { return startPos; }
    Position getEndPos() const { return endPos; }

private:
    static constexpr char START_CHAR = 'S';
    static constexpr char END_CHAR = 'E';
    static constexpr char PATH_CHAR = '.';

    std::unordered_set<Position> pathPositions;

    Position startPos;
    Position endPos;

    int width = 0;
    int height = 0;
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
        parser.parseFile(inputPath.c_str());

        constexpr size_t THRESHOLD = 38;
        size_t numPathThatSaveAboveThreshold = 0;

        auto const [width, height] = parser.getSize();
        for (int y = 1; y < height - 1; y++)
        {
            for (int x = 1; x < width - 1; x++)
            {
                size_t len = lengthOfPathForCheatAt({x, y});
                if (parser.getNumPathTiles() - len >= THRESHOLD)
                    numPathThatSaveAboveThreshold++;
            }
        }

        return numPathThatSaveAboveThreshold;
    }

    size_t part2() const
    {
        return 0;
    }

private:
    size_t lengthOfPathForCheatAt(Position pos)
    {
        if (parser.isPath(pos))
            return parser.getNumPathTiles();

        size_t numNeighbours = 0;
        for (auto const [dx, dy] : std::initializer_list<std::pair<int,int>>{ {1,0}, {0,1}, {-1,0}, {0,-1}})
        {
            if (parser.isPath({pos.x + dx, pos.y + dy}))
                numNeighbours++;
        }
        if (numNeighbours < 2)
            return parser.getNumPathTiles();
        
        resetDistances();

        auto paths = parser.getPathPositions();
        paths.insert(pos);
        optimizeDistances(paths);
        return distanceToEnd[parser.getStartPos()];
    }

    void optimizeDistances(std::unordered_set<Position> paths)
    {
        const int numPathTiles = paths.size();
        const int numIterations = numPathTiles * numPathTiles;

        for (int i = 0; i < numIterations; i++)
        {
            for (Position pos : paths)
            {
                for (auto const [dx, dy] : std::initializer_list<std::pair<int,int>>{ {1,0}, {0,1}, {-1,0}, {0,-1}})
                {
                    Position newPos = {pos.x + dx, pos.y + dy};
                    if (!parser.isPath(newPos))
                        continue;
                    
                    int newDistance = distanceToEnd[newPos];
                    if (newDistance == __INT32_MAX__)
                        continue;
                    
                    newDistance++;

                    if (newDistance < distanceToEnd[pos])
                        distanceToEnd[pos] = newDistance;
                }
            }
        }
    }

    void resetDistances()
    {
        distanceToEnd.clear();
        for (Position pos : parser.getPathPositions())
        {
            distanceToEnd[pos] = __INT32_MAX__;
        }
        distanceToEnd[parser.getEndPos()] = 0;
    }

    std::unordered_map<Position, int> distanceToEnd;

    std::string inputPath;
    MazeParser parser;
};


int main()
{
    Solution solution;
    solution.setInputFilePath("../input_small.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
