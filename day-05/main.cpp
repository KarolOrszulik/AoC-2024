#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>

class Solution
{
public:
    void loadInput(std::string const& path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

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

    int part1()
    {
        int sum = 0;
        for (auto const& update : updates)
        {
            if (isUpdateCorrect(update))
            {
                sum += middlePage(update);
            }
        }
        return sum;
    }

    int part2()
    {
        int sum = 0;
        for (auto const& update : updates)
        {
            if (isUpdateCorrect(update))
                continue;
            
            sum += middlePage(fixUpdate(update));
        }
        return sum;
    }

private:
    using OrderingRules = std::unordered_map<int, std::unordered_set<int>>;
    using Update = std::vector<int>;

    void parseOrderingRule(std::string const& rule)
    {
        int early, late;
        char pipe;

        std::istringstream iss(rule);
        iss >> early >> pipe >> late;

        rules[early].insert(late);
    }

    void parseUpdate(std::string const& numbers)
    {
        Update result;

        std::regex numberRegex("\\d+");
        for (std::sregex_iterator it(numbers.begin(), numbers.end(), numberRegex); it != std::sregex_iterator(); ++it)
        {
            result.push_back(std::stoi(it->str()));
        }

        updates.push_back(result);
    }

    bool isUpdateCorrect(Update const& update)
    {
        for (int i = 0; i < update.size() - 1; i++)
        {
            const int current = update[i];
            for (int j = i + 1; j < update.size(); j++)
            {
                const int second = update[j];
                if (rules[current].count(second) == 0)
                    return false;
            }
        }
        return true;
    }

    void fixSingleError(Update& update)
    {
        for (int i = 0; i < update.size() - 1; i++)
        {
            const int current = update[i];
            for (int j = i + 1; j < update.size(); j++)
            {
                const int second = update[j];
                if (rules[current].count(second) == 0)
                    std::swap(update[i], update[j]);
            }
        }
    }

    Update fixUpdate(Update const& update)
    {
        Update fixed = update;
        while (!isUpdateCorrect(fixed))
        {
            fixSingleError(fixed);
        }
        return fixed;
    }

    int middlePage(Update const& update)
    {
        return update[update.size() / 2];
    }

    OrderingRules rules;
    std::vector<Update> updates;
};

int main()
{
    Solution solution;
    
    solution.loadInput("../input.txt");
    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
