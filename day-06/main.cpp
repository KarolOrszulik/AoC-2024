#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

struct Position
{
    int x = 0;
    int y = 0;
};

enum class Orientation
{
    NORTH,
    EAST,
    SOUTH,
    WEST
};

class Guard
{
public:
    Guard(Position pos = {0, 0})
        : position(pos)
    {}

    Position getPosition()
    {
        return position;
    }

    Position getNextPosition()
    {
        switch (orientation)
        {
            case Orientation::NORTH:
                return Position{position.x, position.y - 1};
            case Orientation::SOUTH:
                return Position{position.x, position.y + 1};
            case Orientation::EAST:
                return Position{position.x + 1, position.y};
            case Orientation::WEST:
                return Position{position.x - 1, position.y};
        }
        throw; // unreachable
    }

    void rotateClockwise()
    {
        int orient = static_cast<int>(orientation);
        ++orient %= 4;
        orientation = static_cast<Orientation>(orient);
    }

    void stepForward()
    {
        position = getNextPosition();
    }

private:
    Position position = { 0, 0 };
    Orientation orientation = Orientation::NORTH;
};

class Board
{
public:
    enum class Tile
    {
        ILLEGAL,
        EMPTY,
        OBSTRUCTION,
    };

    void loadFromFile(std::string const& path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file." << std::endl;
            return;
        }

        std::string line;
        for (int y = 0; std::getline(file, line); y++)
        {
            std::vector<Tile> row;

            for (int x = 0; x < line.length(); x++)
            {
                switch (line[x])
                {
                    case '^':
                        initialGuardPosition = {x, y};
                        // fall through
                    case '.':
                        row.push_back(Tile::EMPTY);
                        break;
                    case '#':
                        row.push_back(Tile::OBSTRUCTION);
                        break;
                }
            }
        }

        initVisited();
    }

    Position getInitialGuardPosition()
    {
        return initialGuardPosition;
    }

    Tile at(Position pos)
    {
        if (pos.x < 0 || pos.x >= width() || pos.y < 0 || pos.y >= height())
            return Tile::ILLEGAL;
        
        return board[pos.y][pos.x];
    }

    void visit(Position pos)
    {
        if (at(pos) == Tile::ILLEGAL)
            return;
        
        visited[pos.y][pos.x] = true;
    }

    int getNumberOfVisitedTiles()
    {
        int sum = 0;
        for (int y = 0; y < height(); y++)
        {
            sum += std::count(visited[y].begin(), visited[y].end(), true);
        }
        return sum;
    }

    int width()
    {
        return board[0].size();
    }

    int height()
    {
        return board.size();
    }

private:
    void initVisited()
    {
        visited = std::vector<std::vector<bool>>(height(), std::vector<bool>(width(), false));
    }

    std::vector<std::vector<Tile>> board;
    std::vector<std::vector<bool>> visited;
    Position initialGuardPosition;
};

class Solution
{
public:
    void loadInputFromFile(std::string const& path)
    {
        board.loadFromFile(path);
        guard = board.getInitialGuardPosition();
    }

    int part1()
    {
        while(!isGuardFinished())
        {
            moveGuard();
        }
        return board.getNumberOfVisitedTiles();
    }

    int part2()
    {
        return 0;
    }

private:
    static constexpr char GUARD_CHAR = '^';

    bool isGuardFinished()
    {
        return board.at(guard.getNextPosition()) == Board::Tile::ILLEGAL;
    }

    void moveGuard()
    {
        Board::Tile nextTile = board.at(guard.getNextPosition());
        
        if (nextTile == Board::Tile::OBSTRUCTION)
        {
            guard.rotateClockwise();
        }
        else
        {
            guard.stepForward();
            board.visit(guard.getPosition());
        }
    }

    Guard guard;
    Board board;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
