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

    bool isMakingChristmasTree()
    {
        std::unordered_set<vec2l> robotPositions;
        for (auto const& r : robots)
            robotPositions.insert(r.position);
        
        return robotPositions == getChristmasTreePositions();
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

    static std::unordered_set<vec2l> getChristmasTreePositions()
    {
        static std::unordered_set<vec2l> positions;

        if (!positions.empty())
            return positions;
        
        // needles
        for (int y = 0; y < HALL_HEIGHT - 1; y++)
        {
            const int treeWidth = 1 + 2*(y-1);
            const int blankBeforeTree = (HALL_WIDTH - treeWidth) / 2;

            for (int x = blankBeforeTree; x < blankBeforeTree + treeWidth; x++)
            {
                positions.insert({x, y});
            }
        }

        // trunk
        positions.insert({HALL_WIDTH / 2, HALL_HEIGHT - 1});

        return positions;
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
        while (!hall.isMakingChristmasTree())
        {
            seconds++;
            for (auto& robot : hall.robots)
                robot.move();
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
