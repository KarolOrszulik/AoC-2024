#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

class CPU
{
public:
    using memory_t = long;
    using register_t = long;

    CPU(register_t aReg, register_t bReg, register_t cReg, std::vector<memory_t> const& memory)
        : memory(memory)
    {
        r[A] = aReg;
        r[B] = bReg;
        r[C] = cReg;
    }

    std::string executeProgram()
    {
        while (!isHalted())
            step();
        return flushOutputBuffer();
    }

private:
    enum Instruction
    {
        adv = 0,
        bxl,
        bst,
        jnz,
        bxc,
        out,
        bdv,
        cdv
    };

    enum Register
    {
        A = 0,
        B,
        C
    };

    bool isHalted() const
    {
        return ip >= memory.size();
    }

    void step()
    {
        ipIncrement = 2;

        auto [opcode, operand] = fetch();
        auto [instruction, operandValue] = decode(opcode, operand);
        execute(instruction, operandValue);

        ip += ipIncrement; // ipIncrement may have been reset to 0 for this one operation by a jump instruction
    }

    std::string flushOutputBuffer()
    {
        std::string output = outputBuffer;
        outputBuffer.clear();
        return output;
    }

    std::pair<memory_t, memory_t> fetch()
    {
        memory_t opcode = memory[ip];
        memory_t operand = memory[ip+1];
        return { opcode, operand };
    }

    std::pair<Instruction, register_t> decode(memory_t opcode, memory_t operand)
    {
        Instruction in = static_cast<Instruction>(opcode);

        register_t operandValue;
        if (in == bxl || in == jnz)
            operandValue = evalLiteralOperand(operand);
        else
            operandValue = evalComboOperand(operand);
        
        return { in, operandValue };
    }

    register_t evalLiteralOperand(register_t operand)
    {
        return operand;
    }

    register_t evalComboOperand(register_t operand)
    {
        constexpr register_t threshold = 4;

        if (operand < threshold)
            return operand;
        
        return r[A + (operand - threshold)];
    }

    void execute(Instruction i, register_t operandValue)
    {
        switch (i)
        {
        case adv:
            executeXDV(A, operandValue);
            break;
        case bdv:
            executeXDV(B, operandValue);
            break;
        case cdv:
            executeXDV(C, operandValue);
            break;
        case bxl:
            executeBXL(operandValue);
            break;
        case bxc:
            executeBXC();
            break;
        case bst:
            executeBST(operandValue);
            break;
        case jnz:
            executeJNZ(operandValue);
            break;
        case out:
            executeOUT(operandValue);
            break;
        }
    }

    void executeXDV(Register reg, register_t op)
    {
        register_t numerator = r[A]; // always read from A
        register_t denumerator = 1 << op;
        r[reg] = numerator / denumerator; 
    }

    void executeBXL(register_t op)
    {
        r[B] ^= op;
    }

    void executeBST(register_t op)
    {
        r[B] = op % 8;
    }

    void executeJNZ(register_t op)
    {
        if (r[A])
        {
            ip = op;
            ipIncrement = 0;
        }
    }

    void executeBXC()
    {
        r[B] ^= r[C];
    }

    void executeOUT(register_t op)
    {
        if (!outputBuffer.empty())
            outputBuffer += ",";

        outputBuffer += std::to_string(op % 8);
    }

    std::vector<memory_t> memory;
    register_t r[3];
    size_t ip = 0;
    int ipIncrement = 2;
    std::string outputBuffer;
};

class CPUParser
{
public:
    CPUParser()
        : numberRegex("\\d+")
    {}

    CPU parseFromFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            throw;
        }

        constexpr int NUM_LINES = 5;
        std::string lines[NUM_LINES];
        for (int i = 0; i < NUM_LINES; i++)
            std::getline(file, lines[i]);
        
        constexpr int NUM_REGISTERS = 3;
        long reg[NUM_REGISTERS];
        for (int i = 0; i < NUM_REGISTERS; i++)
        {
            std::sregex_iterator it(lines[i].begin(), lines[i].end(), numberRegex);
            reg[i] = std::stol(it->str());
        }

        return CPU(reg[0], reg[1], reg[2], parseMemoryFromString(lines[4]));
    }

    std::vector<CPU::memory_t> parseMemoryFromString(std::string const& str)
    {
        std::vector<CPU::memory_t> result;
        for (std::sregex_iterator it(str.begin(), str.end(), numberRegex); it != std::sregex_iterator(); ++it)
        {
            result.push_back(std::stol(it->str()));
        }
        return result;
    }

private:
    std::regex numberRegex;
};

class Solution
{
public:
    std::string part1(CPU& cpu) const
    {
        return cpu.executeProgram();
    }

    std::string part2(CPU& cpu) const
    {
        return "";
    }
};

int main()
{
    CPU cpu = CPUParser().parseFromFile("../input.txt");
    Solution solution;

    std::cout << solution.part1(cpu) << std::endl;
    std::cout << solution.part2(cpu) << std::endl;
}
