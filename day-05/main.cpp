#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <numeric>

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
        return std::accumulate(
            updates.begin(),
            updates.end(),
            int {},
            [this] (int sum, Update const& update) {
                return sum + updateValueForPartOne(update);
            });
    }

    int part2()
    {
        return std::accumulate(
            updates.begin(),
            updates.end(),
            int {},
            [this] (int sum, Update const& update) {
                return sum + updateValueForPartTwo(update);
            });
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
        for (std::sregex_iterator it(numbers.begin(), numbers.end(), numberRegex);
            it != std::sregex_iterator();
            ++it)
        {
            const int number = std::stoi(it->str());
            result.push_back(number);
        }

        updates.push_back(result);
    }

    int updateValueForPartOne(Update const& update)
    {
        if (isUpdateCorrect(update))
            return middlePage(update);
        
        return 0;
    }

    int updateValueForPartTwo(Update const& update)
    {
        if (isUpdateCorrect(update))
            return 0;
        
        return middlePage(fixUpdate(update));
    }

    int middlePage(Update const& update)
    {
        return update[update.size() / 2];
    }

    

    bool isUpdateCorrect(Update const& update)
    {
        return std::pair{-1, -1} == getFirstError(update);
    }

    std::pair<int, int> getFirstError(Update const& update)
    {
        for (int i = 0; i < update.size() - 1; i++)
        {
            const int current = update[i];
            for (int j = i + 1; j < update.size(); j++)
            {
                const int second = update[j];
                if (rules[current].count(second) == 0)
                    return {i, j};
            }
        }
        return {-1, -1};
    }

    Update fixUpdate(Update const& update)
    {
        Update fixed = update;
        while (!isUpdateCorrect(fixed))
        {
            const std::pair errorIdx = getFirstError(fixed);
            std::swap(fixed[errorIdx.first], fixed[errorIdx.second]);
        }
        return fixed;
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
