#pragma once

#include "Zydis/Zydis.h"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <array>
#include <cstdint>

namespace Dissasembler
{

    struct Instruction
    {
        uint64_t address;
        std::string text;
        ZydisDecodedInstruction info;
        std::array<ZydisDecodedOperand, ZYDIS_MAX_OPERAND_COUNT> operands;
    };

    class Decoder
    {
    private:
        ZydisDecoder decoderEngine;
        ZydisFormatter formatter;
        std::vector<uint8_t> buffer;
        std::unordered_map<std::string, uint64_t> stringTable;

    public:
        // initialize with buffer
        Decoder(const std::vector<uint8_t>& buf)
            : buffer(buf)
        {
            ZydisDecoderInit(&decoderEngine, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);
            ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);
        }

        // disassemble from offset with size
        std::vector<Instruction> Disassemble(size_t startOffset, size_t size, uint64_t virtualStart = 0)
        {
            std::vector<Instruction> instructions;
            size_t offset = startOffset;
            size_t bufferSize = buffer.size();
            uint64_t address = virtualStart;

            if (offset >= bufferSize) return instructions;

            size_t maxSize = min(size, bufferSize - offset);

            while (offset < startOffset + maxSize)
            {
                ZydisDecodedInstruction instInfo;
                ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

                ZyanStatus status = ZydisDecoderDecodeFull(
                    &decoderEngine,
                    buffer.data() + offset,
                    bufferSize - offset,
                    &instInfo,
                    operands
                );

                if (!ZYAN_SUCCESS(status) || instInfo.length == 0 || instInfo.length > (bufferSize - offset))
                {
                    offset += 1;
                    address += 1;
                    continue;
                }

                char buf[256];
                ZydisFormatterFormatInstruction(&formatter, &instInfo, operands, instInfo.operand_count_visible,
                    buf, sizeof(buf), address, nullptr);

                Instruction inst;
                inst.address = address;
                inst.text = buf;
                inst.info = instInfo;
                for (size_t i = 0; i < ZYDIS_MAX_OPERAND_COUNT; i++)
                    inst.operands[i] = operands[i];

                instructions.push_back(inst);

                offset += instInfo.length;
                address += instInfo.length;
            }

            return instructions;
        }

        void PrintDisassembly(size_t startOffset, size_t size, uint64_t virtualStart = 0)
        {
            auto insts = Disassemble(startOffset, size, virtualStart);
            for (const auto& inst : insts)
                std::cout << std::hex << inst.address << "  " << inst.text << "\n";
        }

        uintptr_t GetMemoryDisplacement(const Dissasembler::Instruction& inst)
        {
            for (size_t i = 0; i < inst.info.operand_count_visible; i++) {
                const auto& op = inst.operands[i];
                if (op.type == ZYDIS_OPERAND_TYPE_MEMORY)
                    return op.mem.disp.value;
            }
            return 0;
        }
    };

    inline std::vector<Dissasembler::Instruction> FindString(
        const std::string& name,
        Decoder& dec,
        size_t bytesAbove = 100,
        size_t bytesTotal = 200)
    {
        auto& buffer = PatternScanning::buffer;

        auto it = std::search(buffer.begin(), buffer.end(), name.begin(), name.end());
        if (it == buffer.end()) {
            return {};
        }

        uintptr_t stringOffset = std::distance(buffer.begin(), it);

        uintptr_t ref = PatternScanning::FindStringReference(buffer, stringOffset);
        if (!ref) {
            return {};
        }

        size_t start = (ref > bytesAbove) ? ref - bytesAbove : 0;

        return dec.Disassemble(start, bytesTotal, start);
    }

    inline std::vector<Dissasembler::Instruction> FindFunc(
        uintptr_t offset,
        size_t bytesAbove = 100,
        size_t bytesTotal = 200)
    {
        auto& buffer = PatternScanning::buffer;
        uintptr_t ref = PatternScanning::FindBranchReference(buffer, offset);
        if (!ref)
            return {};

        size_t start = (ref > bytesAbove) ? ref - bytesAbove : 0;

        Dissasembler::Decoder dec(buffer);
        return dec.Disassemble(start, bytesTotal, start);
    }
}
