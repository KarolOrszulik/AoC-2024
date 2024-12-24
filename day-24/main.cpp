#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using Circuit = std::unordered_map<std::string, bool>;

enum class Type
{
    AND,
    OR,
    XOR
};

struct LogicGate
{
    std::string inA;
    std::string inB;
    std::string out;
    Type type;

    bool update(Circuit& circuit) const
    {
        if (circuit.count(inA) == 0 || circuit.count(inB) == 0)
            return false;

        auto a = circuit[inA];
        auto b = circuit[inB];

        if (circuit.count(out) == 0)
        {
            bool outputValue = getOutput(a, b);
            circuit[out] = outputValue;
            return true;
        }
        return false;
    }

    bool getOutput(bool a, bool b) const
    {
        switch (type)
        {
        case Type::AND:
            return a && b;
        case Type::OR:
            return a || b;
        case Type::XOR:
            return a ^ b;
        }
        throw; // should be unreachable;
    }

    static Type strToType(std::string const& str)
    {
        switch (str.front())
        {
        case 'A':
            return Type::AND;
        case 'O':
            return Type::OR;
        case 'X':
            return Type::XOR;
        }
        throw;
    }
};

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

        circuit.clear();

        bool stillReadingInputs = true;
        std::string line;

        while (std::getline(file, line))
        {
            if (line.empty())
            {
                stillReadingInputs = false;
                continue;
            }

            if (stillReadingInputs)
                parseLineAsInput(line);
            else
                parseLineAsConnection(line);
        }
    }

    Circuit& getCircuit()
    {
        return circuit;
    }

    std::vector<LogicGate> const& getLogicGates() const
    {
        return gates;
    }

private:
    void parseLineAsInput(std::string const& line)
    {
        std::string inputID;
        bool state;

        std::istringstream iss(line);
        iss >> inputID >> state;

        inputID.pop_back(); // drop the trailing colon

        circuit[inputID] = state;
    }

    void parseLineAsConnection(std::string const& line)
    {
        std::string inA, type, inB, arrow, out;

        std::istringstream iss(line);
        iss >> inA >> type >> inB >> arrow >> out;

        gates.push_back(LogicGate{inA, inB, out, LogicGate::strToType(type)});
    }

    Circuit circuit;
    std::vector<LogicGate> gates;
};

class Solution
{
public:
    void parseFile(const char* path)
    {
        parser.parseFile(path);
    }

    size_t part1()
    {
        updateEntireCircuit();
        return getNumberForLetter('z');
    }

    size_t part2()
    {
        return 0;
    }

private:
    void updateEntireCircuit()
    {
        bool doneUpdating = false;
        while (!doneUpdating)
        {
            doneUpdating = true;
            for (auto const& gate : parser.getLogicGates())
            {
                bool didUpdate = gate.update(parser.getCircuit());
                if (didUpdate)
                    doneUpdating = false;
            }
        }
    }

    size_t getNumberForLetter(char letter)
    {
        size_t result = 0;
        for (auto const [output, value] : parser.getCircuit())
        {
            if (output.front() != letter || value == false)
                continue;

            size_t outputNumber = std::stoi(std::string(output.begin() + 1, output.end()));
            result |= 1ULL << outputNumber;
        }
        return result;
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
