#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
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

    static vec2l fromString(std::string const& string)
    {
        std::regex twoNumbersRegex(R"((\d+).*?(\d+))");

        std::smatch match;
        std::regex_search(string, match, twoNumbersRegex);

        return vec2l { std::stoll(match[1].str()), std::stoll(match[2].str()) };
    }
};


struct Button
{
    vec2l deltas;
    int price = 0;
};


class ClawMachine
{
public:
    ClawMachine(std::string const& btnAstr, std::string const& btnBstr, std::string const& targetStr)
    {
        buttonA.deltas = vec2l::fromString(btnAstr);
        buttonA.price = 3;

        buttonB.deltas = vec2l::fromString(btnBstr);
        buttonB.price = 1;

        target = vec2l::fromString(targetStr);
    }

    std::optional<size_t> getMinimumWinningPrice() const
    {
        const std::vector<vec2l> possibleForX = possibleCoefficients(target.x, {buttonA.deltas.x, buttonB.deltas.x});
        const std::vector<vec2l> possibleForY = possibleCoefficients(target.y, {buttonA.deltas.y, buttonB.deltas.y});

        std::vector<vec2l> possible;
        for (auto const posX : possibleForX)
        {
            for (auto const posY : possibleForY)
            {
                if (posX == posY)
                    possible.push_back(posX);
            }
        }

        if (possible.empty())
            return {};

        std::vector<int> prices;
        prices.reserve(possible.size());

        for (auto const p : possible)
            prices.push_back(buttonA.price * p.x + buttonB.price * p.y);
        
        return *std::min_element(prices.begin(), prices.end());
    }

    // input: target, n[x, y]
    // output: all possible pairs a[x, y] such that n[x]*a[x] + n[y]+a[y] = target
    // std::vector<vec2l> possibleCoefficients(long long T, vec2l n) const
    // {
    //     std::vector<vec2l> result;

    //     const long long MAX_OF_FIRST = T / n.x + 1;
    //     for (long long i = 0; i <= MAX_OF_FIRST; i++)
    //     {
    //         const long long MAX_OF_SECOND = (T - i * n.x) / n.y + 1;
    //         for (long long j = 0; j < MAX_OF_SECOND; j++)
    //         {
    //             if (i * n.x + j * n.y == T)
    //             {
    //                 result.push_back({i, j});
    //                 continue;
    //             }
    //         }
    //     }

    //     return result;
    // }

    std::vector<vec2l> possibleCoefficients(long long T, vec2l n) const
    {
        std::vector<vec2l> result;

        auto opt = findFirstPossibleCoefficient(T, n);
        if (!opt.has_value())
        {
            return {};
        }

        long da, db;
        for (long div = 1; div <= n.x && div <= n.y; div++)
        {
            if (n.x % div == 0 && n.y % div == 0)
            {
                da = n.y / div;
                db = n.x / div;
            }
        }

        auto [a, b] = opt.value();
        while (b >= 0)
        {
            result.push_back({a, b});
            a += da;
            b -= db;
        }

        return result;
    }

    std::optional<vec2l> findFirstPossibleCoefficient(long long T, vec2l n) const
    {
        const long long MAX_OF_FIRST = T / n.x + 1;
        for (long long i = 0; i <= MAX_OF_FIRST; i++)
        {
            const long long MIN_OF_SECOND = (T - i * n.x) / n.y + 1;
            for (long long j = MIN_OF_SECOND; j >= 0; j--)
            {
                if (i * n.x + j * n.y == T)
                {
                    return {{i, j}};
                }
            }
        }
        return {};
    }

    void moveTarget(vec2l shift)
    {
        target = target + shift;
    }

private:
    Button buttonA;
    Button buttonB;
    vec2l target;
};


class Arcade
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

        std::string btnA, btnB, target, empty;
        do
        {
            std::getline(file, btnA);
            std::getline(file, btnB);
            std::getline(file, target);
            std::getline(file, empty);

            machines.emplace_back(btnA, btnB, target);
        } while (file);
    }

    size_t totalCostOfWinningAllMachines() const
    {
        size_t total = 0;
        for (auto const& machine : machines)
        {
            auto price = machine.getMinimumWinningPrice();
            total += price.value_or(0);
        }
        return total;
    }

    void moveTargets(vec2l shift)
    {
        for (auto& m : machines)
            m.moveTarget(shift);
    }

private:
    std::vector<ClawMachine> machines;
};


class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        arcade.loadFromFile(path);
    }

    size_t part1() const
    {
        return arcade.totalCostOfWinningAllMachines();
    }

    size_t part2()
    {
        arcade.moveTargets({ 10000000000000, 10000000000000 });
        return arcade.totalCostOfWinningAllMachines();
    }

private:
    Arcade arcade;
};


int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
