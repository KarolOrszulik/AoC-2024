#include <iostream>
#include <fstream>
#include <vector>

struct Position
{
    int x, y;
};

enum class Tile
{
    INVALID = 0,
    PATH = '.',
    WALL = '#'
};

class Maze
{
    friend class MazeParser;

public:
    Tile& at(Position pos)
    {
        return tiles[pos.x + pos.y * width];
    }

private:
    std::vector<Tile> tiles;
    int width = 0;
    int height = 0;
};


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

        numPathTiles = 0;
        maze.height = 0;
        maze.tiles.clear();

        std::string line;
        for (int y = 0; std::getline(file, line); y++)
        {
            maze.width = line.length();
            for (int x = 0; x < maze.width; x++)
            {
                const char c = line[x];
                Tile tile = static_cast<Tile>(c);

                if (c == START_CHAR)
                {
                    startPos = {x, y};
                    tile = Tile::PATH;
                }
                else if (c == END_CHAR)
                {
                    endPos = {x, y};
                    tile = Tile::PATH;
                }
                
                if (tile == Tile::PATH)
                    numPathTiles++;
                
                maze.tiles.push_back(tile);                
            }
            maze.height++;
        }
    }

    std::pair<int, int> getSize() const
    {
        return { maze.width, maze.height };
    }

    int getNumPathTiles() const
    {
        return numPathTiles;
    }

    std::vector<Tile>& getTiles()
    {
        return maze.tiles;
    }

    Tile& at(Position pos)
    {
        return maze.at(pos);
    }

private:
    static constexpr char START_CHAR = 'S';
    static constexpr char END_CHAR = 'E';

    Position startPos;
    Position endPos;

    Maze maze;

    int numPathTiles = 0;
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

        constexpr size_t THRESHOLD = 100;
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
        size_t numNeighbours = 0;
        for (auto const [dx, dy] : std::initializer_list<std::pair<int,int>>{ {1,0}, {0,1}, {-1,0}, {0,-1}})
        {
            if (parser.at({pos.x + dx, pos.y + dy}) == Tile::PATH)
                numNeighbours++;
        }
        if (numNeighbours < 2)
            return parser.getNumPathTiles();
        
        
    }

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
