#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>

using ID_t = size_t;
static constexpr ID_t EMPTY_ID = -1;

// only works for part 1
class VectorDiskImage
{
public:
    void loadFromFile(std::string const& path)
    {
        std::ifstream file(path);
        disk.clear();

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        ID_t currID = 0;
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

    size_t getChecksum() const
    {
        size_t sum = 0;
        for (size_t i = 0; i < disk.size(); i++)
        {
            size_t num = disk[i];
            if (num == EMPTY_ID)
                break;

            sum += i * num;
        }
        return sum;
    }

private:
    size_t findFirstEmptySpace()
    {
        for (size_t i = 0; i < disk.size(); i++)
            if (disk[i] == EMPTY_ID)
                return i;
        
        return -1;
    }

    std::vector<ID_t> disk;
};


// works for both parts but for part 1 is much slower than VectorDiskImage
class ListDiskImage
{
public:
    void loadFromFileForPart(std::string const& path, int part)
    {
        disk.clear();
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open input file" << std::endl;
            return;
        }

        size_t currIdx = 0;
        bool isFile = true;

        char c;
        while (file >> c)
        {
            size_t size = c - '0';
            size_t id = isFile ? currIdx : EMPTY_ID;
            

            if (part == 1)
                createFilesForPart1(size, id);
            else if (part == 2)
                createFileForPart2(size, id);
            else
                throw "Invalid part number, only 1 and 2 permitted";


            if (isFile)
                currIdx++;

            isFile = !isFile;
        }
    }

    void defragment()
    {
        for (auto it = disk.rbegin(); it != disk.rend(); ++it)
        {
            if (it->id == EMPTY_ID)
                continue;

            const auto [emptySpotIt, emptySpotIdx] = firstEmptySpaceOfMinSize(it->size);
            const size_t itIdx = disk.size() - 1 - std::distance(disk.rbegin(), it);

            if (emptySpotIt == disk.end() || emptySpotIdx >= itIdx)
                continue;

            // decrease empty space size and insert copied file descriptor at its beginning
            emptySpotIt->size -= it->size;
            disk.insert(emptySpotIt, *it);

            // set old file's id to empty space to be removed in the next step
            it->id = EMPTY_ID;

            // merge all subsequent empty spaces into 1 big one and several 0-size ones
            mergeEmptySpace();
        }

        // delete 0-sized files (not actually necessary?) - confirmed, works without, and slightly faster, too
        // eraseZeroLengthDescriptors();
    }

    size_t getChecksum() const
    {
        size_t sum = 0;
        size_t idx = 0;
        for (Descriptor const& file : disk)
        {           
            for (int i = 0; i < file.size; i++)
            {
                sum += file.id == EMPTY_ID ? 0 : file.id * idx;
                idx++;
            }
        }
        return sum;
    }

private:
    struct Descriptor
    {
        size_t size;
        ID_t id;
    };

    void createFilesForPart1(size_t size, ID_t id)
    {
        for (size_t i = 0; i < size; i++)
            disk.push_back({1, id});
    }

    void createFileForPart2(size_t size, ID_t id)
    {
        disk.push_back({size, id});
    }

    std::pair<std::list<Descriptor>::iterator, size_t> firstEmptySpaceOfMinSize(size_t minSize)
    {
        size_t idx = 0;
        for (auto it = disk.begin(); it != disk.end(); ++it, ++idx)
        {
            if (it->id == EMPTY_ID && it->size >= minSize)
                return { it, idx };
        }
        return { disk.end(), -1 };
    }

    void mergeEmptySpace()
    {
        for (auto it = disk.rbegin(); it != disk.rend(); ++it)
        {
            if (it->id != EMPTY_ID)
                continue;
            
            auto nextIt = std::next(it);

            if (nextIt != disk.rend() && nextIt->id == EMPTY_ID)
            {
                nextIt->size += it->size;
                it->size = 0;
            }
        }
    }

    void eraseZeroLengthDescriptors()
    {
        disk.erase(std::remove_if(disk.begin(), disk.end(), [](auto const& d) { return d.size == 0; }), disk.end());
    }

    std::list<Descriptor> disk;
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
        // #define USE_LIST_IMPLEMENTATION

#ifdef USE_LIST_IMPLEMENTATION
            ListDiskImage image;
            image.loadFromFileForPart(inputPath, 1); // works slowly
#else
            VectorDiskImage image;
            image.loadFromFile(inputPath);
#endif
            image.defragment();
            return image.getChecksum();
    }

    size_t part2() const
    {
        ListDiskImage image;
        image.loadFromFileForPart(inputPath, 2);
        image.defragment();
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
