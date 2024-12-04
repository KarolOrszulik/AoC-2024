#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using CharMatrix = std::vector<std::string>;

CharMatrix loadCharMatrixFromFile(std::string const& path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Could not open input file" << std::endl;
        return {};
    }

    CharMatrix result;

    std::string line;
    while (std::getline(file, line))
    {
        result.emplace_back(std::move(line));
    }

    return result;
}

char charAt(CharMatrix const& matrix, int x, int y)
{
    if (x < 0 || x >= matrix[y].size() || y < 0 || y >= matrix.size())
        return '.';
    
    return matrix[y][x];
}

int countXMAS(CharMatrix const& matrix)
{
    const std::string target = "XMAS";
    const std::string reverseTarget(target.rbegin(), target.rend());

    int xmasCount = 0;

    for (int y = 0; y < matrix.size(); y++)
    {
        for (int x = 0; x < matrix[y].size(); x++)
        {
            std::string horizontal, vertical, diagonal1, diagonal2;
            for (int i = 0; i < target.length(); i++)
            {
                horizontal.push_back(charAt(matrix, x+i, y));
                vertical.push_back(charAt(matrix, x, y+i));
                diagonal1.push_back(charAt(matrix, x+i, y+i));
                diagonal2.push_back(charAt(matrix, x+i, y-i));
            }

            for (auto const& str : {horizontal, vertical, diagonal1, diagonal2})
            {
                if (str == target || str == reverseTarget)
                    xmasCount++;
            }
        }
    }

    return xmasCount;
}

int countCrossedMAS(CharMatrix const& matrix)
{
    int count = 0;

    for (int y = 1; y < matrix.size() - 1; y++)
    {
        for (int x = 1; x < matrix[y].size() - 1; x++)
        {
            if (charAt(matrix, x, y) != 'A')
                continue;
            
            const char tl = charAt(matrix, x-1, y-1);
            const char tr = charAt(matrix, x+1, y-1);
            const char bl = charAt(matrix, x-1, y+1);
            const char br = charAt(matrix, x+1, y+1);

            int mCount = 0;
            int sCount = 0;

            for (auto c : {tl, tr, bl, br})
            {
                mCount += c == 'M';
                sCount += c == 'S';
            }

            if (mCount == 2 && sCount == 2 && tl != br)
                count++;
        }
    }

    return count;
}

int main()
{
    CharMatrix matrix = loadCharMatrixFromFile("../input.txt");

    std::cout << countXMAS(matrix) << std::endl;
    std::cout << countCrossedMAS(matrix) << std::endl;
}
