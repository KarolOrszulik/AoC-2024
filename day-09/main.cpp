#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>

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

        for (int i = disk.size() - 1; i >= 0; i--)
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

        size_t currIdx = 0;
        bool isFile = true;

        char c;
        while (file >> c)
        {
            size_t size = c - '0';
            size_t id = isFile ? currIdx : EMPTY_ID;
            disk.push_back({ size, id });

            if (isFile)
                currIdx++;

            isFile = !isFile;
        }
    }

    void defragment() override
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

    size_t getChecksum() const override
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
    using id_t = size_t;
    static constexpr id_t EMPTY_ID = -1;

    struct Descriptor
    {
        size_t size;
        id_t id;
    };

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
    // system("pwd");

    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
