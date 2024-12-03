#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

std::string loadFileToString(std::string const& path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Could not open input file" << std::endl;
        return {};
    }

    std::stringstream ss;
    ss << file.rdbuf();

    return ss.str();
}

std::vector<std::string> findValidMuls(std::string const& string)
{
    std::regex validMul(R"(mul\(\d{1,3},\d{1,3}\))");
    std::vector<std::string> result;

    for (std::sregex_iterator it(string.begin(), string.end(), validMul);
         it != std::sregex_iterator();
         ++it)
    {
        std::smatch match = *it;
        result.emplace_back(std::move(match.str()));
    }

    return result;
}

int executeMul(std::string const& mulCommand)
{
    std::regex number(R"(\d+)");

    int result = 1;
    for (std::sregex_iterator it(mulCommand.begin(), mulCommand.end(), number);
         it != std::sregex_iterator();
         ++it)
    {
        result *= std::stoi(it->str());
    }

    return result;
}

std::string trimDisabledCode(std::string const& code)
{
    std::regex disabledCode(R"(don\'t\(\).*?do\(\))");

    struct DisabledRegion
    {
        int position, length;
    };

    std::vector<DisabledRegion> regions;

    for (std::sregex_iterator it(code.begin(), code.end(), disabledCode);
         it != std::sregex_iterator();
         ++it)
    {
        regions.emplace_back(it->position(), it->length());
    }

    std::string trimmedCode = code;
    for (int i = regions.size() - 1; i >= 0; i--)
    {
        trimmedCode.erase(regions[i].position, regions[i].length);
    }
    
}

int part1(std::vector<std::string> const& mulCommands)
{
    int sum = 0;
    for (auto const& mul : mulCommands)
    {
        sum += executeMul(mul);
    }
    return sum;
}

int main()
{
    std::string input = loadFileToString("../input.txt");
    std::vector<std::string> allMuls = findValidMuls(input);

    std::cout << part1(allMuls) << std::endl;
}
