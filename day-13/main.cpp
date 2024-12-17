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
        const long xa = buttonA.deltas.x;
        const long ya = buttonA.deltas.y;

        const long xb = buttonB.deltas.x;
        const long yb = buttonB.deltas.y;
        
        const long xt = target.x;
        const long yt = target.y;

        const long det = xa*yb - xb*ya;
        const long detA = xt*yb - yt*xb;
        const long detB = yt*xa - ya*xt;

        if (det == 0 || detA % det != 0 || detB % det != 0)
            return {};
        
        return buttonA.price * (detA / det) + buttonB.price * (detB / det);
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
