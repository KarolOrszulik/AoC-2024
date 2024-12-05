#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

struct MulOperation
{
    int a, b;

    MulOperation(int a, int b)
        : a(a), b(b) {}

    int value() const { return a * b; }
};

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

std::vector<MulOperation> findValidMuls(std::string const& string)
{
    std::regex validMulRegex(R"(mul\((\d{1,3}),(\d{1,3})\))");
    std::vector<MulOperation> result;

    for (std::sregex_iterator it(string.begin(), string.end(), validMulRegex);
         it != std::sregex_iterator();
         ++it)
    {
        std::smatch match = *it;

        int a = std::stoi(match[1].str());
        int b = std::stoi(match[2].str());

        result.emplace_back(a, b);
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

int sumOfMulCommands(std::vector<MulOperation> const& mulCommands)
{
    int sum = 0;
    for (auto const& mul : mulCommands)
    {
        sum += mul.value();
    }
    return sum;
}

int main()
{
    std::string inputCode = loadFileToString("../input.txt");
    std::vector<MulOperation> allMuls = findValidMuls(inputCode);

    std::string trimmedCode = trimDisabledCode(inputCode);
    std::vector<MulOperation> trimmedMuls = findValidMuls(trimmedCode);

    std::cout << sumOfMulCommands(allMuls) << std::endl;
    std::cout << sumOfMulCommands(trimmedMuls) << std::endl;
}
