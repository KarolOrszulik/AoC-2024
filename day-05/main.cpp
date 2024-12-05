#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class Solution
{
public:
    void loadInput(std::string const& path)
    {
        std::ifstream file(path);

        if (file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        std::cout << file.rdbuf();

        std::string line;

        do
        {
            std::getline(file, line);
            parseOrderingRule(line);
        } while (!line.empty());
        
        while (std::getline(file, line))
        {
            parseUpdate(line);
        }
    }

private:
    using OrderingRule = std::pair<int, int>;
    using Update = std::vector<int>;

    void parseOrderingRule(std::string const& rule)
    {
        int early, late;
        char pipe;

        std::istringstream iss(rule);
        iss >> early >> pipe >> late;

        rules.emplace_back(early, late);
    }

    void parseUpdate(std::string const& numbers)
    {
        Update result;

        std::istringstream iss(numbers);
        int n;
        while (iss >> n)
        {
            result.push_back(n);
        }

        updates.push_back(result);
    }

    std::vector<OrderingRule> rules;
    std::vector<Update> updates;
};

int main()
{
    system("pwd");
    system("ls ../../");
    Solution solution;
    solution.loadInput("../../input.txt");
}
