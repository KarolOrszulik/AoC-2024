#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

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

        availableTowels.clear();
        desiredCombinations.clear();

        std::string line;
        std::getline(file, line);

        parseAvailableTowels(line);

        std::getline(file, line); // empty line, ignore

        while (std::getline(file, line))
        {
            parseDesiredCombination(line);
        }
    }

    std::vector<std::string> const& getAvailableTowels() const
    {
        return availableTowels;
    }

    std::vector<std::string> const& getDesiredDesigns() const
    {
        return desiredCombinations;
    }

private:
    void parseAvailableTowels(std::string const& line)
    {
        std::regex towelRegex("[wubrg]+");

        for (std::sregex_iterator it(line.begin(), line.end(), towelRegex); it != std::sregex_iterator(); ++it)
        {
            availableTowels.push_back(it->str());
        }
    }

    void parseDesiredCombination(std::string& line)
    {
        desiredCombinations.emplace_back(std::move(line));
    }

    std::vector<std::string> availableTowels;
    std::vector<std::string> desiredCombinations;
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
        for (auto const& design : parser.getDesiredDesigns())
        {
            if (canBeConstructed(design, parser.getAvailableTowels()))
                sum++;
        }
        return sum;
    }

    size_t part2() const
    {
        Parser parser;
        parser.parseFile(inputPath.c_str());

        size_t sum = 0;
        size_t counter = 0;
        for (auto const& design : parser.getDesiredDesigns())
        {
            std::cout << "Iteration " << counter++ << ": ";
            sum += numWaysToConstruct(design, parser.getAvailableTowels());
            std::cout << "sum so far: " << sum << "\n";
        }
        return sum;
    }

private:
    bool canBeConstructed(std::string const& design, std::vector<std::string> const& towels) const
    {
        if (design.length() == 0)
            return true;

        for (const auto& towel : towels)
        {            
            if (!isPrefix(design, towel))
                continue;
            
            std::string shortenedDesign = design.substr(towel.length());

            if (canBeConstructed(shortenedDesign, towels))
                return true;
        }

        return false;
    }

    size_t numWaysToConstruct(std::string const& design, std::vector<std::string> const& towels) const
    {
        if (design.length() == 0)
            return 1;

        size_t sum = 0;

        for (const auto& towel : towels)
        {            
            if (!isPrefix(design, towel))
                continue;
            
            std::string shortenedDesign = design.substr(towel.length());
            sum += numWaysToConstruct(shortenedDesign, towels);
        }

        return sum;
    }

    bool isPrefix(std::string const& string, std::string const& prefixCandidate) const
    {
        if (prefixCandidate.length() > string.length())
            return false;
        
        for (int i = 0; i < prefixCandidate.length(); i++)
        {
            if (prefixCandidate[i] != string[i])
                return false;
        }

        return true;
    }

    std::string inputPath;
};


int main()
{
    Solution solution;
    solution.setInputFilePath("../input_small.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
