#include <iostream>
#include <fstream>
#include <vector>


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

class Solution
{
public:
    void setInputFilePath(const char* path)
    {
        inputPath = path;
    }

    size_t part1() const
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

    size_t part2() const
    {
        return 0;
    }

private:
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
        a = mix(a, b);
        a = prune(a);
        return a;
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
