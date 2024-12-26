#include <iostream>
#include <fstream>
#include <vector>
#include <array>

static constexpr int NUM_PINS = 5;
static constexpr int MAX_PIN_HEIGHT = 5;

using Device = std::array<int, NUM_PINS>;

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

        keys.clear();
        locks.clear();

        while (file)
        {
            DeviceDescriptor desc;
            for (auto& line : desc)
            {
                std::getline(file, line);
            }

            std::string emptyLine;
            std::getline(file, emptyLine);

            parseDescriptor(desc);
        }
    }

    std::vector<Device> const& getLocks() const
    {
        return locks;
    }
    
    std::vector<Device> const& getKeys() const
    {
        return keys;
    }

private:
    using DeviceDescriptor = std::array<std::string, MAX_PIN_HEIGHT + 2>;
    static constexpr char CHAR_FULL = '#';
    static constexpr char CHAR_EMPTY = '.'; 

    void parseDescriptor(DeviceDescriptor const& desc)
    {
        if (desc[0][0] == CHAR_FULL)
            locks.push_back(parseLock(desc));
        else
            keys.push_back(parseKey(desc));
    }

    Device parseLock(DeviceDescriptor const& desc)
    {
        Device device;
        for (auto& p : device)
            p = -1;

        for (int i = 1; i < desc.size(); i++)
        {
            for (int pin = 0; pin < NUM_PINS; pin++)
            {
                if (desc[i][pin] != desc[0][pin] && device[pin] == -1)
                    device[pin] = i-1;
            }
        }
        return device;
    }

    Device parseKey(DeviceDescriptor const& desc)
    {
        Device device = parseLock(desc);
        for (auto& pin : device)
            pin = 5 - pin;
        
        return device;
    }

    std::vector<Device> keys;
    std::vector<Device> locks;
};


class Solution
{
public:
    void parseFile(const char* path)
    {
        parser.parseFile(path);
    }

    size_t part1() const
    {
        size_t nonOverlapping = 0;
        for (auto const& key : parser.getKeys())
        {
            for (auto const& lock : parser.getLocks())
            {
                if (not overlaps(key, lock))
                    nonOverlapping++;
            }
        }
        return nonOverlapping;
    }

    size_t part2() const
    {
        return 0;
    }

private:
    bool overlaps(Device const& a, Device const& b) const
    {
        for (int pin = 0; pin < NUM_PINS; pin++)
        {
            if (a[pin] + b[pin] > MAX_PIN_HEIGHT)
                return true;
        }
        return false;
    }

    Parser parser;
};


int main()
{
    Solution solution;
    solution.parseFile("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
