#include <iostream>
#include <fstream>
#include <list>
#include <cmath>

class Solution
{
public:
    void setInputFilePath(const char* path)
    {
        inputFilePath = path;
    }

    size_t part1()
    {
        return partCommon(25);
    }

    size_t part2()
    {
        return partCommon(75);
    }

private:
    std::string inputFilePath;
    std::list<size_t> stones;
    using stone_it = decltype(stones)::iterator;

    void loadStonesFromFile()
    {
        stones.clear();

        std::ifstream file(inputFilePath);
        
        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        size_t number;
        while (file >> number)
            stones.push_back(number);
    }

    size_t partCommon(size_t numIterations)
    {
        loadStonesFromFile();

        for (size_t i = 0; i < numIterations; i++)
        {
            std::cout << "Starting iteration: " << i+1 << "...";
            updateStones();
            std::cout << "Done with stone count of " << stones.size() << std::endl;
        }
        
        return stones.size();
    }

    void updateStones()
    {
        for (auto it = stones.begin(); it != stones.end(); ++it)
            processSingleStone(it);
    }

    size_t numDigits(size_t n)
    {
        size_t powerOf10 = 10;
        size_t numDigits = 1;
        while (n >= powerOf10)
        {
            numDigits++;
            powerOf10 *= 10;
        }
        return numDigits;
    }

    size_t nthPowerOf10(size_t n)
    {
        size_t result = 1;
        while (n--)
            result *= 10;
        return  result;
    }

    void processSingleStone(stone_it it)
    {
        if (*it == 0)
        {
            processZero(it);
        }
        else
        {
            const size_t numLength = numDigits(*it);

            if (numLength % 2 == 0)
            {
                processEvenNumDigits(it, numLength);
            }
            else
            {
                processOddNumDigits(it);
            }
        }
    }

    void processZero(stone_it it)
    {
        *it = 1;
    }

    void processEvenNumDigits(stone_it it, size_t numLength)
    {
        const size_t power = nthPowerOf10(numLength / 2);
        const size_t leftPart = (*it) / power;
        const size_t rightPart = (*it) % power;

        *it = rightPart;
        stones.insert(it, leftPart);
    }

    void processOddNumDigits(stone_it it)
    {
        *it *= 2024;
    }
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
