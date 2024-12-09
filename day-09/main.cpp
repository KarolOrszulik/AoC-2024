#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

class DiskImage
{
    virtual void loadFromFile(std::string const&) = 0;
    virtual void defragment() = 0;
    virtual size_t getChecksum() const = 0;
};

class FragmentedDiskImage : public DiskImage
{
public:
    void loadFromFile(std::string const& path) override
    {
        std::ifstream file(path);
        disk.clear();

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        char c;

        FileFragmentID currID = 0;
        bool isFile = true;
        while (file >> c)
        {
            int num = std::stoi(std::string(1, c));
            FileFragmentID id = isFile ? currID : EMPTY_SPACE;
            for (int i = 0; i < num; i++)
            {
                disk.push_back(id);
            }
            if (isFile)
                currID++;
            isFile ^= 1;
        }
    }

    void defragment() override
    {
        auto firstEmptyIt = std::find(disk.begin(), disk.end(), EMPTY_SPACE);
        if (firstEmptyIt == disk.end())
            return;

        int emptyIdx = firstEmptyIt - disk.begin();

        for (int i = disk.size()-1; i >= 0; i--)
        {
            if (i <= emptyIdx)
                return;

            std::swap(disk[i], disk[emptyIdx]);

            while (disk[emptyIdx] != EMPTY_SPACE)
                emptyIdx++;
        }
    }

    size_t getChecksum() const override
    {
        size_t sum = 0;
        for (size_t i = 0; i < disk.size(); i++)
        {
            size_t num = disk[i];
            if (num == EMPTY_SPACE)
                break;
            
            sum += i * num;
        }
        return sum;
    }

private:
    using FileFragmentID = size_t;
    static constexpr FileFragmentID EMPTY_SPACE = -1;

    std::vector<FileFragmentID> disk;
};


class UnfragmentedDiskImage : public DiskImage
{
public:
    void loadFromFile(std::string const& path) override
    {
        std::ifstream file(path);

        if (!file) 
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        char c;

        size_t currIdx = 0;
        bool isFile = true;
        while (file >> c)
        {
            size_t size = c - '0';

            if (size > 0)
            {
                disk.push_back(Descriptor { currIdx, size, isFile });
            }

            currIdx += size;
            isFile = !isFile;
        }
    }

    void defragment() override
    {
        for (int i = disk.size() - 1; i >= 0; i)
        {
            int emptyIdx = idxOfLeftmostFittingSpace(disk[i].size);
            if (emptyIdx == -1)
                continue;
            
            Descriptor fileCopy = disk[i];

            disk[emptyIdx].start += fileCopy.size;
            disk[emptyIdx].size  -= fileCopy.size;

            disk.erase(disk.begin() + i);
            disk.emplace(disk.begin() + emptyIdx, fileCopy);

            removeZeroSizeEmptySpaces();
            mergeEmptySpaces();
        }
    }

    size_t getChecksum() const override
    {
        return 0;
    }

private:
    struct Descriptor
    {
        size_t start, size;
        bool isFile;
    };

    int idxOfLeftmostFittingSpace(size_t minSize)
    {
        for (int i = 0; i < disk.size(); i++)
        {
            if (!disk[i].isFile && disk[i].size >= minSize)
                return i;
        }
        return -1;
    }

    void removeZeroSizeEmptySpaces()
    {
        for (int i = disk.size() - 1; i >= 0; i--)
        {
            if (disk[i].isFile || disk[i].size > 0)
                continue;
            
            disk.erase(disk.begin() + i);
        }
    }

    void mergeEmptySpaces()
    {
        for (int i = disk.size() - 2; i >= 0; i--)
        {
            if (disk[i].isFile || disk[i+1].isFile)
                continue;
            
            disk[i].size += disk[i+1].size;
            disk.erase(disk.begin() + i + 1);
        }
    }

    std::vector<Descriptor> disk;
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
        FragmentedDiskImage image;
        image.loadFromFile(inputPath);
        image.defragment();
        return image.getChecksum();
    }

    size_t part2() const
    {
        UnfragmentedDiskImage image;
        image.loadFromFile(inputPath);
        image.defragment();
        return image.getChecksum();
    }

private:
    std::string inputPath;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input_small.txt");

    // std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
