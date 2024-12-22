#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <unordered_map>

class Parser
{
public:
    void parseFile(const char* path)
    {
        std::ifstream file(path);
        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }

        numbers.clear();
        for (std::string line; std::getline(file, line);)
        {
            size_t num = std::stoull(line);
            numbers.push_back(num);
        }
    }

    std::vector<size_t> const& getNumbers() const
    {
        return numbers;
    }

private:
    std::vector<size_t> numbers;
};


static constexpr int STRATEGY_SIZE = 4;
struct PriceChangeHistory : public std::array<int, STRATEGY_SIZE>
{
    PriceChangeHistory(std::array<int, STRATEGY_SIZE> arr) : std::array<int, STRATEGY_SIZE>(arr) {}

    static std::vector<PriceChangeHistory> getAllPossible(int min = -9, int max = 9)
    {
        std::vector<PriceChangeHistory> result;

        for (int i = min; i <= max; i++)
        for (int ii = min; ii <= max; ii++)
        for (int iii = min; iii <= max; iii++)
        for (int iv = min; iv <= max; iv++)
        {
            result.push_back({{i,ii,iii,iv}});
        }
        
        return result;
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
        Parser parser;
        parser.parseFile(inputPath.c_str());

        size_t sum = 0;
        for (size_t n : parser.getNumbers())
        {
            for (int i = 0; i < NUM_ITERATIONS; i++)
                n = processNumber(n);
            sum += n;
        }
        return sum;
    }

    size_t part2()
    {
        Parser parser;
        parser.parseFile(inputPath.c_str());

        std::unordered_map<size_t, std::vector<PriceAndDelta>> numToHistory;

        for (size_t n : parser.getNumbers())
        {
            auto& history = numToHistory[n];
            int lastPrice = toPrice(n);
            for (int i = 0; i < NUM_ITERATIONS; i++)
            {
                n = processNumber(n);
                const int price = toPrice(n);
                const int delta = price - lastPrice;
                history.push_back(PriceAndDelta{price, delta});
                lastPrice = price;
            }
        }

        auto all = PriceChangeHistory::getAllPossible();
        int progress = 0;
        size_t maxBananas = 0;
        for (PriceChangeHistory strategy : all)
        {
            size_t numBananas = 0;

            for (size_t n : parser.getNumbers())
                numBananas += priceAtHistory(numToHistory[n], strategy);

            if (numBananas > maxBananas)
                maxBananas = numBananas;

            progress++;
            if (progress % 100 == 0)
                std::cout << "Progress: " << progress << "/" << all.size() << "\n";
        }

        return maxBananas;
    }

private:
    static constexpr int NUM_ITERATIONS = 2000;

    struct PriceAndDelta
    {
        int price;
        int delta;
    };

    size_t toPrice(size_t n)
    {
        return n % 10;
    }

    size_t priceAtHistory(std::vector<PriceAndDelta> const& history, PriceChangeHistory const& strategy)
    {
        const size_t N = strategy.size();
        for (int i = 3; i < history.size(); i++)
        {
            if (history[i].delta   == strategy[0] &&
                history[i-1].delta == strategy[1] &&
                history[i-2].delta == strategy[2] &&
                history[i-3].delta == strategy[3])
                return history[i].price;
        }
        return 0;
    }

    size_t processNumber(size_t n) const
    {
        n = mixAndPrune(n, n*64);
        n = mixAndPrune(n, n/32);
        n = mixAndPrune(n, n*2048);
        return n;
    }

    size_t mixAndPrune(size_t a, size_t b) const
    {
        return prune(mix(a, b));
    }

    size_t mix(size_t a, size_t b) const
    {
        return a ^ b;
    }

    size_t prune(size_t n) const
    {
        return n % 16777216;
    }

    std::string inputPath;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
