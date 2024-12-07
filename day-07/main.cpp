#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <thread>
#include <future>

struct Equation
{
    size_t result;
    std::vector<size_t> operands;

    bool canBeValidForTwoOperators() const
    {
        const int numOperators = operands.size() - 1;

        using bitmask_t = size_t;
        assert(numOperators <= sizeof(bitmask_t) * 8);

        const int maxBitmasksNeeded = 1 << numOperators;
        for (bitmask_t bitmask = 0; bitmask < maxBitmasksNeeded; bitmask++)
        {
            size_t total = operands[0];

            for (int i = 0; i < operands.size() - 1; i++)
            {
                const bool isAddition = (bitmask >> i) & 1;
                const int otherOperand = operands[i+1];
                if (isAddition)
                    total += otherOperand;
                else
                    total *= otherOperand;
            }

            if (total == result)
                return true;
        }

        return false;
    };


    bool canBeValidForThreeOperators() const
    {
        if (canBeValidForTwoOperators())
            return true;

        enum class Operator
        {
            ADD = 0b00,
            MULTIPLY = 0b01,
            CONCATENATE = 0b10,
            INVALID = 0b11
        };

        const int numOperators = operands.size() - 1;

        using bitmask_t = size_t;
        constexpr int BITS_PER_OPERATOR = 2;
        assert(numOperators <= sizeof(bitmask_t) * 8 / BITS_PER_OPERATOR);

        auto ithOperator = [] (bitmask_t bitmask, int i) -> Operator {
            return static_cast<Operator>((bitmask >> (i * BITS_PER_OPERATOR)) & 0b11);
        };

        auto validateBitmast = [numOperators, ithOperator] (bitmask_t bitmask) -> bool {
            bool hasConcat = false;
            for (int i = 0; i < numOperators; i++)
            {
                Operator op = ithOperator(bitmask, i);

                if (op == Operator::CONCATENATE)
                    hasConcat = true;
                else if (op == Operator::INVALID)
                    return false;
            }
            return hasConcat;
        };

        const int numBitmasks = 1 << (numOperators * BITS_PER_OPERATOR);
        for (bitmask_t bitmask = 0; bitmask < numBitmasks; bitmask++)
        {
            if (!validateBitmast(bitmask))
                continue;

            size_t total = operands[0];

            for (int i = 0; i < numOperators; i++)
            {
                const Operator op = ithOperator(bitmask, i);
                const int otherOperand = operands[i+1];
                
                switch (op)
                {
                case Operator::ADD:
                    total += otherOperand;
                    break;
                case Operator::MULTIPLY:
                    total *= otherOperand;
                    break;
                case Operator::CONCATENATE:
                    total = std::stoul(std::to_string(total) + std::to_string(otherOperand));
                    break;
                }
            }

            if (total == result)
                return true;
        }

        return false;
    };
};

class Solution
{
public:
    void loadInputFromFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file." << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line))
        {
            parseLineAsEquation(line);
        }
    }

    size_t part1() const
    {
        size_t sum = 0;
        for (Equation const& eq : equations)
        {
            if (eq.canBeValidForTwoOperators())
                sum += eq.result;
        }
        return sum;
    }
    
    size_t part2() const
    {
        std::vector<std::future<size_t>> futures;

        for (Equation const& eq : equations)
        {
            futures.push_back(std::async([&eq] { return eq.canBeValidForThreeOperators() ? eq.result : 0; }));
        }

        size_t sum = 0;
        for (auto& future : futures)
        {
            sum += future.get();
        }

        return sum;
    }

private:
    void parseLineAsEquation(std::string const& line)
    {
        Equation equation;
        char colon;

        std::istringstream iss(line);
        iss >> equation.result >> colon;

        int operand;
        while (iss >> operand)
        {
            equation.operands.push_back(operand);
        }

        equations.emplace_back(std::move(equation));
    }

    std::vector<Equation> equations;
};

int main()
{
    Solution solution;
    solution.loadInputFromFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
