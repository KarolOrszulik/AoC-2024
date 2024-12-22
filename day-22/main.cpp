#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

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

class PriceChangeHistory
{
public:
    static constexpr int N = 4;

    bool operator==(PriceChangeHistory const& other) const
    {
        return deltas == other.deltas;
    }

    void add(int newDelta)
    {
        deltas.push(newDelta);
        if (deltas.size() > N)
            deltas.pop();
    }

    static std::vector<PriceChangeHistory> getAllPossible()
    {
        std::vector<PriceChangeHistory> result;

        for (int i = MIN; i <= MAX; i++)
        for (int ii = MIN; ii <= MAX; ii++)
        for (int iii = MIN; iii <= MAX; iii++)
        for (int iv = MIN; iv <= MAX; iv++)
        {
            PriceChangeHistory pch;
            pch.add(i);
            pch.add(ii);
            pch.add(iii);
            pch.add(iv);
            result.push_back(pch);
        }
        
        return result;
    }

private:
    static constexpr int MIN = -9;
    static constexpr int MAX = 9;

    std::queue<int> deltas;
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
            for (int i = 0; i < 2000; i++)
                n = processNumber(n);
            sum += n;
        }
        return sum;
    }

    size_t part2()
    {
        Parser parser;
        parser.parseFile(inputPath.c_str());

        size_t maxBananas = 0;
        int progress = 0;
        for (PriceChangeHistory strategy : PriceChangeHistory::getAllPossible())
        {
            std::cout 
                << "calculating " << progress++ << "/" << PriceChangeHistory::getAllPossible().size()
                << "; current max: " << maxBananas << std::endl;

            size_t numBananas = totalBananasForStrategy(strategy, parser.getNumbers());
            if (numBananas > maxBananas)
                maxBananas = numBananas;
        }

        return maxBananas;
    }

private:
    size_t totalBananasForStrategy(PriceChangeHistory const& strategy, std::vector<size_t> const& nums) const
    {
        size_t numBananas = 0;
        for (size_t n : nums)
        {
            size_t newBananas = numBananasForStrategy(strategy, n);
            numBananas += newBananas;
        }
        return numBananas;
    }

    size_t numBananasForStrategy(PriceChangeHistory const& strategy, size_t n) const
    {
        PriceChangeHistory history;
        int lastPrice = 0;
        for (int i = 0; i <= 2000; i++)
        {
            const int price = n % 10;
            if (i != 0)
            {
                const int delta = price - lastPrice;
                history.add(delta);
                if (history == strategy)
                    return price;
            }
            lastPrice = price;
            n = processNumber(n);
        }
        return 0;
    }

    size_t processNumber(size_t n) const
    {
        size_t result;

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
