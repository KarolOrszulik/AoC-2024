#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <algorithm>

struct Computer
{
    char id[2];

    bool operator==(Computer const& other) const
    {
        return id[0] == other.id[0] && id[1] == other.id[1];
    }
};

namespace std
{
    template<>
    struct hash<Computer>
    {
        size_t operator()(Computer const& c) const
        {
            return std::hash<char>{}(c.id[0]) ^ std::hash<char>{}(c.id[1]);
        }
    };
}

class Parser
{
public:
    using ConnectionMap = std::unordered_map<Computer, std::unordered_set<Computer>>;

    void parseFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            return;
        }

        connections.clear();

        Computer c1, c2;
        char comma;
        while (file >> c1.id[0] >> c1.id[1] >> comma >> c2.id[0] >> c2.id[1])
        {
            connections[c1].insert(c2);
            connections[c2].insert(c1);
        }
    }

    ConnectionMap const& getConnections() const
    {
        return connections;
    }

private:
    ConnectionMap connections;
};


struct Clique
{
    std::unordered_set<Computer> computers;

    bool operator==(Clique const& other) const
    {
        return computers == other.computers;
    }
};

namespace std
{
    template<>
    struct hash<Clique>
    {
        size_t operator()(Clique const& c) const
        {
            size_t hash = 0;
            for (auto computer : c.computers)
                hash ^= std::hash<Computer>{}(computer);

            return hash;
        }
    };
}


class Solution
{
public:
    void parseInput(const char* path)
    {
        parser.parseFile(path);
    }

    size_t part1() const
    {
        auto cliques = find3Cliques();

        size_t sum = 0;
        for (auto clique : cliques)
        {
            for (Computer computer : clique.computers)
                if (computer.id[0] == 't')
                {
                    sum++;
                    break;
                }
        }

        return sum;
    }

    size_t part2() const
    {
        return 0;
    }

private:
    std::unordered_set<Clique> find3Cliques() const
    {
        std::unordered_set<Clique> cliques;
        for (auto const& [c1, c1Connections] : parser.getConnections())
        {
            for (auto const& c2 : c1Connections)
            {
                if (c1 == c2)
                    continue;

                for (auto const& c3 : c1Connections)
                {
                    if (c1 == c3 || c2 == c3)
                        continue;

                    if (parser.getConnections().at(c2).count(c3) > 0)
                    {
                        Clique clique;
                        clique.computers.insert(c1);
                        clique.computers.insert(c2);
                        clique.computers.insert(c3);
                        cliques.insert(clique);
                    }
                }
            }
        }

        return cliques;
    }

    Parser parser;
};

int main()
{
    Solution solution;
    solution.parseInput("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;
}
