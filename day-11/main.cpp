#include <iostream>
#include <fstream>
#include <unordered_map>
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
    void loadStonesFromFile()
    {
        stoneNumToCount.clear();

        std::ifstream file(inputFilePath);
        
        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        size_t number;
        while (file >> number)
            stoneNumToCount[number]++;
    }

    size_t partCommon(size_t numIterations)
    {
        loadStonesFromFile();

        for (size_t i = 0; i < numIterations; i++)
        {
            updateStones();
        }
        
        return getStoneCount();
    }

    void updateStones()
    {
        std::unordered_map<size_t, size_t> newStones;

        for (auto const [number, count] : stoneNumToCount)
        {
            if (number == 0)
            {
                newStones[1] += count;
                continue;
            }

            size_t numLength = numDigits(number);

            if (numLength % 2 == 1)
            {
                newStones[number * 2024] += count;
            }
            else
            {
                const size_t power = nthPowerOf10(numLength / 2);
                const size_t leftPart = number / power;
                const size_t rightPart = number % power;

                newStones[leftPart] += count;
                newStones[rightPart] += count;
            }
        }

        stoneNumToCount = std::move(newStones);
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

    size_t getStoneCount() const
    {
        size_t total = 0;
        for (const auto [number, count] : stoneNumToCount)
            total += count;
        return total;
    }

    std::string inputFilePath;
    std::unordered_map<size_t, size_t> stoneNumToCount;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
