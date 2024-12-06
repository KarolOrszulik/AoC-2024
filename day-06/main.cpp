#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Guard
{
    struct Position
    {
        int x = 0;
        int y = 0;
    } position;

    enum class Orientation
    {
        NORTH,
        EAST,
        SOUTH,
        WEST
    } orientation = Orientation::NORTH;

    Position nextPosition()
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
    }

    void rotateClockwise()
    {
        int orient = static_cast<int>(orientation);
        ++orient %= 4;
        orientation = static_cast<Orientation>(orient);
    }
};

class Solution
{
public:
    enum class Tile
    {
        OUTSIDE,
        EMPTY,
        OBSTACKLE,
        GUARD
    };

    void loadInputFromFile(std::string const& path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
            board.push_back(std::move(line));
        
        placeGuard();
    }

private:
    static constexpr char GUARD_CHAR = '^';

    void placeGuard()
    {
        for (int y = 0; y < board.size(); y++)
        {
            for (int x = 0; x < board[y].size(); x++)
            {
                if (board[y][x] == GUARD_CHAR)
                {
                    guard.position = {x, y};
                    return;
                }
            }
        }
    }



    Guard guard;
    std::vector<std::string> board;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input_small.txt");
}
