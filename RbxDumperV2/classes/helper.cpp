#include "../globals.h"

namespace Helper {
    RbxDumper::Offset DoRTTIScan(RTTIScanConfig config)
    {
        for (uintptr_t i = config.start; i < config.end; i += config.step) {
            bool allMatch = true;

            for (size_t j = 0; j < config.targetInstances.size(); j++) {
                uintptr_t addressToCheck = config.targetInstances[j] + i;

                uintptr_t val = Mem::Read<uintptr_t>(addressToCheck);
				string valueAtAddress = GetRTTI(val);

                if (valueAtAddress != config.expectedValues[j]) {
                    allMatch = false;
                    break;
                }
            }

            if (allMatch) {
                RbxDumper::Offset offset(config.name, config.catagory, i, config.type);
                Globals::offsets.add(offset);
                return offset;
            }
        }

        Logger::Log(Logger::Level::Warning, "No offset found for " + config.name + "::" + config.catagory);

        RbxDumper::Offset offset(config.name, config.catagory, 0, config.type);
        Globals::offsets.add(offset);
        return offset;
    }

    uintptr_t DoPatternScan(string pattern, bool printInstructions, uintptr_t offset)
    {
        static Dissasembler::Decoder decoder(PatternScanning::buffer);

        uintptr_t instructionAddress = PatternScanning::Scan(pattern) + offset;
        if (!instructionAddress) {
            Logger::Log(Logger::Level::Warning, "DoPatternScan failed with pattern: " + pattern + ". InstructionAddress was 0.");
            return 0;
        }

        auto instructions = decoder.Disassemble(instructionAddress, 16, instructionAddress);
        if (instructions.empty()) {
            Logger::Log(Logger::Level::Warning, "DoPatternScan failed with pattern: " + pattern + ". Failed to decode instructions.");
            return 0;
        }

        if (printInstructions) {
            decoder.PrintDisassembly(instructionAddress, 16, instructionAddress);
        }

        auto instruction = instructions[0];
        return decoder.GetMemoryDisplacement(instruction);
    }
}
