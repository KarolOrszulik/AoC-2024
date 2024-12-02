#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using Raport = std::vector<int>;
using Data   = std::vector<Raport>;

Raport parseRaport(std::string const& string)
{
    std::istringstream iss(string);
    Raport raport;
    int reading;

    while (iss >> reading)
        raport.push_back(reading);
    
    return raport;
}

Data loadData(std::string const& path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Can't open input file" << std::endl;
        return {};
    }

    Data data;
    std::string line;

    while (std::getline(file, line))
    {
        data.emplace_back(parseRaport(line));
    }

    return data;
}

bool isRaportSafe(Raport const& raport)
{
    constexpr int MIN_DIFFERENCE = 1;
    constexpr int MAX_DIFFERENCE = 3;

    if (raport.size() < 2)
        return true;

    const bool isDecreasing = raport[0] > raport[1];

    for (int i = 0; i < raport.size() - 1; i++)
    {
        int diff = raport[i+1] - raport[i];

        if (isDecreasing)
            diff *= -1;
        
        if (diff < MIN_DIFFERENCE || diff > MAX_DIFFERENCE)
            return false;
    }

    return true;
}

bool isRaportSafeDamped(Raport const& raport)
{
    for (int i = 0; i < raport.size(); i++)
    {
        Raport reduced = raport;
        reduced.erase(reduced.begin() + i);
        
        if (isRaportSafe(reduced))
            return true;
    }

    return false;
}

int part1(Data const& data)
{
    return std::count_if(data.begin(), data.end(), [](Raport const& r) { return isRaportSafe(r); });
}

int part2(Data const& data)
{
    return std::count_if(data.begin(), data.end(), [](Raport const& r) { return isRaportSafeDamped(r); });
}

int main()
{
    Data data = loadData("../input.txt");
    std::cout << part1(data) << std::endl;
    std::cout << part2(data) << std::endl;
}
