#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <optional>

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

struct Robot
{
    vec2l position;
    vec2l velocity;

    static constexpr long HALL_WIDTH = 101;
    static constexpr long HALL_HEIGHT = 103;

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
};


class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        hall.loadFromFile(path);
    }

    size_t part1()
    {
        constexpr long ITERATIONS = 100;
        constexpr long NUM_QUADRANTS = 4;

        size_t quadrants[NUM_QUADRANTS] = {};

        for (auto& robot : hall.robots)
        {
            for (long i = 0; i < ITERATIONS; i++)
                robot.move();

            auto quadrantIdx = quadrant(robot.position);
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

    size_t part2() const
    {
        return 0;
    }

private:
    std::optional<int> quadrant(vec2l pos) const
    {
        static_assert(Robot::HALL_HEIGHT % 2 == 1);
        static_assert(Robot::HALL_WIDTH % 2 == 1);

        constexpr int middleRow = Robot::HALL_HEIGHT / 2;
        constexpr int middleCol = Robot::HALL_WIDTH / 2;

        if (pos.x == middleCol || pos.y == middleRow)
            return {};
        
        const int x = pos.x < middleCol;
        const int y = pos.y < middleRow;

        return y * 2 + x;
    }

    BathroomHall hall;
};


int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
