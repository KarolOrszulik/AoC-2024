#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>


class DiskImage
{
public:
    using id_t = size_t;
    static constexpr id_t EMPTY_ID = -1;

    void loadFromFile(std::string const& path)
    {
        std::ifstream file(path);
        disk.clear();

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        id_t currID = 0;
        bool isFile = true;

        char c;
        while (file >> c)
        {
            const int numFileFragments = c - '0';

            for (int i = 0; i < numFileFragments; i++)
                disk.push_back(isFile ? currID : EMPTY_ID);

            if (isFile)
                currID++;

            isFile = !isFile;
        }
    }

    void defragment()
    {
        size_t emptyIdx = findFirstEmptySpace();
        if (emptyIdx == -1)
            return;

        for (int i = disk.size() - 1; i >= 0; i--)
        {
            if (i <= emptyIdx)
                return;

            std::swap(disk[i], disk[emptyIdx]);

            while (disk[emptyIdx] != EMPTY_ID)
                emptyIdx++;
        }
    }

    void defragmentWithoutSplittingFiles()
    {
        id_t minFileID = -1;

        assert(minFileID > 0); // make sure id is unsigned and overflowed to max

        for (int i = disk.size() - 1; i >= 0;)
        {
            auto const [fileID, fileStart, fileSize] = findFirstFileAtOrBeforePos(i);

            if (fileID < minFileID)
            {
                minFileID = fileID;
                const int emptyIdx = findFirstEmptySpace(fileSize);

                if (emptyIdx != -1 && emptyIdx < fileStart)
                    for (int j = 0; j < fileSize; j++)
                        std::swap(disk[fileStart + j], disk[emptyIdx + j]);
            }
            
            i = fileStart - 1;
        }
    }

    size_t getChecksum() const
    {
        size_t sum = 0;
        size_t slot = 0;
        for (size_t i = 0; i < disk.size(); i++)
        {
            size_t num = disk[i];

            if (num != EMPTY_ID)
                sum += slot * num;
            
            slot++;
        }
        return sum;
    }

private:
    int findFirstEmptySpace(int minSize = 1)
    {
        for (int i = 0; i <= disk.size() - minSize; i++)
        {
            if (disk[i] != EMPTY_ID)
                continue;

            for (int j = 0; j < minSize; j++)
            {
                if (disk[i+j] != EMPTY_ID)
                    break;

                if (j == minSize - 1)
                    return i;
            }
        }
        return -1;
    }

    std::tuple<id_t, int, int> findFirstFileAtOrBeforePos(int pos)
    {
        // this algorithm assumes there IS a file at or before the given position
        // for the input format this task uses, this will always be the case
        // because the disk always starts with a file

        while (disk[pos] == EMPTY_ID)
            pos--;
        
        const int fileEnd = pos;
        const id_t fileID = disk[pos];

        while (pos >= 0 && disk[pos] == fileID)
            pos--;
        
        const int fileStart = pos + 1;
        const int fileSize = fileEnd - fileStart + 1;

        return { fileID, fileStart, fileSize };
    }

    std::vector<id_t> disk;
};


class Solution
{
public:
    void setInputFilePath(std::string const& path)
    {
        inputPath = path;
    }

    size_t part1() const
    {
        DiskImage image;
        image.loadFromFile(inputPath);
        image.defragment();
        return image.getChecksum();
    }

    size_t part2() const
    {
        DiskImage image;
        image.loadFromFile(inputPath);
        image.defragmentWithoutSplittingFiles();
        return image.getChecksum();
    }

private:
    std::string inputPath;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
