#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <array>
#include <thread>
#include <atomic>

class CPU
{
public:
    static constexpr int NUM_REGISTERS = 3;

    using memory_t = unsigned char;
    using register_t = long;

    using RegistersArray = std::array<CPU::register_t, CPU::NUM_REGISTERS>;
    using Memory = std::vector<CPU::memory_t>;

    CPU(Memory const& memory, RegistersArray const& registers)
        : memory(memory), r(registers)
    {
    }

    std::string executeProgram()
    {
        while (!isHalted())
            step();
        return flushOutputBuffer();
    }

    bool isSelfReplicating()
    {
        while (!isHalted())
        {
            step();
            if (outputBuffer.empty())
                continue;

            if (outputBuffer.size() > memory.size())
                return false;
            
            if (outputBuffer.back() != memory[outputBuffer.size() - 1])
                return false;
        }
        return outputBuffer.size() == memory.size();
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
        if (outputBuffer.empty())
            return "";
        
        std::string output;

        for (size_t i = 0; i < outputBuffer.size(); i++)
        {
            if (i != 0)
                output += ',';

            output += std::to_string(outputBuffer[i]);
        }

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
        outputBuffer.push_back(op % 8);
    }

    Memory memory;
    RegistersArray r;

    size_t ip = 0;
    int ipIncrement = 2;

    Memory outputBuffer;
};

class CPUParser
{
public:
    CPUParser()
        : numberRegex("\\d+")
    {}

    void parseFile(const char* path)
    {
        std::ifstream file(path);

        if (!file)
        {
            std::cerr << "Could not open file" << std::endl;
            throw;
        }

        for (int i = 0; i < NUM_LINES; i++)
            std::getline(file, lines[i]);   
    }

    CPU getCPU() const
    {
        CPU::RegistersArray reg = getRegisters();
        CPU::Memory mem = getMemory();
        return CPU(std::move(mem), std::move(reg));
    }

    CPU::RegistersArray getRegisters() const
    {
        CPU::RegistersArray registers;
        for (int i = 0; i < registers.size(); i++)
        {
            std::sregex_iterator it(lines[i].begin(), lines[i].end(), numberRegex);
            registers[i] = std::stol(it->str());
        }
        return registers;
    }

    std::string const& getMemoryString() const
    {
        constexpr int MEMORY_LINE_NUM = 4;
        return lines[MEMORY_LINE_NUM];
    }

    CPU::Memory getMemory() const
    {
        CPU::Memory result;

        const std::string& str = getMemoryString();
        for (std::sregex_iterator it(str.begin(), str.end(), numberRegex); it != std::sregex_iterator(); ++it)
        {
            result.push_back(std::stol(it->str()));
        }
        return result;
    }

private:
    const std::regex numberRegex;

    static constexpr int NUM_LINES = CPU::NUM_REGISTERS + 2; // one empty and one for the program
    std::array<std::string, NUM_LINES> lines;
};


class Solution
{
public:
    void setInputFilePath(const char* path)
    {
        parser.parseFile(path);
    }

    std::string part1() const
    {
        CPU cpu = parser.getCPU();
        return cpu.executeProgram();
    }

    long part2() const
    {
        CPU::Memory const& memory = parser.getMemory();

        // long i = 0;
        // while (true)
        // {
        //     CPU cpu(memory, {i, 0, 0});

        //     if (cpu.isSelfReplicating())
        //         return i;
            
        //     if (i % 100'000'000 == 0)
        //         std::cout << "Progress: " << i << '\n';

        //     i++;
        // }

        const int NUM_THREADS = 16;
        constexpr long STARTING_VALUE = 1730000000000;

        std::atomic<long> result = -1;

        std::vector<std::thread> threads;

        auto worker = [&](long start, long step) {
            for (long i = start + STARTING_VALUE; result < 0; i += step) {
                CPU cpu(memory, {i, 0, 0});

                if (cpu.isSelfReplicating()) {
                    result.store(i);
                    return;
                }

                if (i % 1'000'000'000 == start)
                    std::cout << "Thread " << start << " progress: " << i << '\n';
            }
        };

        for (long t = 0; t < NUM_THREADS; ++t) {
            threads.emplace_back(worker, t, NUM_THREADS);
        }

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }

        return result.load();
    }

private:
    CPUParser parser;
};

int main()
{
    Solution solution;
    solution.setInputFilePath("../input.txt");

    std::cout << solution.part1() << std::endl;
    std::cout << solution.part2() << std::endl;

    // progress checkpoint: thread 0 started at 1'477'000'000'000
}
