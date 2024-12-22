#include <iostream>
#include <fstream>
#include <regex>
#include <unordered_set>
#include <optional>

static constexpr long HALL_WIDTH = 101;
static constexpr long HALL_HEIGHT = 103;

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

struct Robot
{
    vec2l position;
    vec2l velocity;

    void move()
    {
        position = position + velocity;

        position.x = (position.x + HALL_WIDTH) % HALL_WIDTH;
        position.y = (position.y + HALL_HEIGHT) % HALL_HEIGHT;
    }

    void initFromString(std::string const& str)
    {
        std::regex number(R"(-?\d+)");

        std::sregex_iterator it(str.begin(), str.end(), number);
        position.x = std::stol((it++)->str());
        position.y = std::stol((it++)->str());
        velocity.x = std::stol((it++)->str());
        velocity.y = std::stol((it++)->str());
    }
};

struct BathroomHall
{
    std::vector<Robot> robots;

    void loadFromFile(const char* path)
    {
        robots.clear();
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            Robot robot;
            robot.initFromString(line);
            robots.push_back(std::move(robot));
        }
    }

    size_t getProductOfQuadrantCounts()
    {
        constexpr long ITERATIONS = 100;
        constexpr long NUM_QUADRANTS = 4;

        size_t quadrants[NUM_QUADRANTS] = {};

        for (auto& robot : robots)
        {
            for (long i = 0; i < ITERATIONS; i++)
                robot.move();

            auto quadrantIdx = getQuadrantIdx(robot.position);
            if (quadrantIdx.has_value())
                quadrants[quadrantIdx.value()]++;
        }
        
        size_t total = 1;
        for (auto q : quadrants)
        {
            total *= q;
        }
        return total;
    }

    void moveRobots()
    {
        for (auto& robot : robots)
            robot.move();
    }

    size_t numRobotsWithAtLeast2Neighbours() const
    {
        size_t num = 0;
        for (Robot const& robot : robots)
        {
            size_t neigh = 0;
            for (Robot const& other : robots)
            {
                int dx = robot.position.x - other.position.x;
                int dy = robot.position.y - other.position.y;

                if (dx == 0 && (dy == 1 || dy == -1) || dy == 0 && (dx == 1 || dx == -1))
                {
                    neigh++;
                }
            }
            if (neigh >= 2)
                num++;
        }
        return num;
    }

    void print() const
    {
        for (int y = 0; y < HALL_HEIGHT; y++)
        {
            for (int x = 0; x < HALL_WIDTH; x++)
            {
                bool isRobotHere = false;
                for (Robot const& r : robots)
                {
                    if (r.position.x == x && r.position.y == y)
                    {
                        isRobotHere = true;
                        break;
                    }
                }
                std::cout << (isRobotHere ? '#' : '.');
            }
            std::cout << "\n";
        }
    }

private:
    std::optional<int> getQuadrantIdx(vec2l pos) const
    {
        static_assert(HALL_HEIGHT % 2 == 1);
        static_assert(HALL_WIDTH % 2 == 1);

        constexpr int middleRow = HALL_HEIGHT / 2;
        constexpr int middleCol = HALL_WIDTH / 2;

        if (pos.x == middleCol || pos.y == middleRow)
            return {};
        
        const int x = pos.x < middleCol;
        const int y = pos.y < middleRow;

        return y * 2 + x;
    }
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
        hall.loadFromFile(inputPath.c_str());
        return hall.getProductOfQuadrantCounts();
    }

    size_t part2()
    {
        hall.loadFromFile(inputPath.c_str());

        size_t seconds = 0;
        size_t bestNeighbourhood = 0;
        while (true)
        {
            size_t num2neighbours = hall.numRobotsWithAtLeast2Neighbours();
            if (num2neighbours > bestNeighbourhood)
            {
                bestNeighbourhood = num2neighbours;
                hall.print();
                std::cout << "This is after " << seconds << " seconds.\n\n";
            }
            seconds++;
            hall.moveRobots();
        }
        return seconds;
    }

private:
    BathroomHall hall;
    std::string inputPath;
};


int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
