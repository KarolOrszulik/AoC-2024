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
    std::regex validMulRegex(R"(mul\(\d{1,3},\d{1,3}\))");
    std::vector<std::string> result;

    for (std::sregex_iterator it(string.begin(), string.end(), validMulRegex);
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

std::string removeNewlinesFromString(std::string const& string)
{
    std::string result = string;
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.cend());

    return result;
}

std::string trimDisabledCode(std::string const& code)
{
    std::string trimmedCode = removeNewlinesFromString(code);

    std::regex disabledCodeRegex(R"(don\'t\(\).*?(do\(\)|$))");
    trimmedCode = std::regex_replace(trimmedCode, disabledCodeRegex, "");

    return trimmedCode;
}

int sumOfMulCommands(std::vector<std::string> const& mulCommands)
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
    std::string inputCode = loadFileToString("../input.txt");
    std::vector<std::string> allMuls = findValidMuls(inputCode);

    std::string trimmedCode = trimDisabledCode(inputCode);
    std::vector<std::string> trimmedMuls = findValidMuls(trimmedCode);

    std::cout << sumOfMulCommands(allMuls) << std::endl;
    std::cout << sumOfMulCommands(trimmedMuls) << std::endl;
}
