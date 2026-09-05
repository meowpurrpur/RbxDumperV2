namespace PatternScanning {
    inline ifstream fileData;
    inline vector<unsigned char> buffer;

    inline void Init(const string& robloxPath) {
        fileData = ifstream(robloxPath, ios::binary);
        if (!fileData) return;

        buffer = vector<unsigned char>(istreambuf_iterator<char>(fileData), {});
        Logger::Log(Logger::Level::Success, "Pattern scanning setup!");
    }

    inline uintptr_t AddRip(uintptr_t address) {
        int32_t displacement;
        memcpy(&displacement, &buffer[address + 3], sizeof(int32_t));
        return address + 7 + displacement;
    }

    inline uintptr_t GetStructOffset(uintptr_t instructionOffset) {
        int32_t offset;
        memcpy(&offset, &buffer[instructionOffset + 2], sizeof(int32_t));
        return offset;
    }

    inline vector<int> ParseSignature(const string& signature) {
        vector<int> pattern;
        istringstream iss(signature);
        string byteStr;
        while (iss >> byteStr) {
            if (byteStr == "?" || byteStr == "??") {
                pattern.push_back(-1);
            }
            else {
                pattern.push_back(stoi(byteStr, nullptr, 16));
            }
        }
        return pattern;
    }

    inline uintptr_t Scan(string patternStr) {
        auto pattern = ParseSignature(patternStr);
        if (pattern.empty() || buffer.size() < pattern.size()) return SIZE_MAX;

        for (size_t i = 0x10000; i <= buffer.size() - pattern.size(); i++) {
            bool match = true;
            for (size_t j = 0; j < pattern.size(); j++) {
                if (pattern[j] != -1 && buffer[i + j] != static_cast<unsigned char>(pattern[j])) {
                    match = false;
                    break;
                }
            }
            if (match) return i;
        }

        //Logger::Log(Logger::Level::Warning, "Pattern: " + patternStr + " did not find any result.");
        return 0;
    }

    inline uint64_t GetFFlag(const string& name) {
        auto it = search(buffer.begin(), buffer.end(), name.begin(), name.end());
        if (it == buffer.end()) return 0;

        uint64_t stringOffset = distance(buffer.begin(), it);

        vector<uint8_t> pattern = { 0x48, 0x8D, 0x15 };

        for (uint64_t pos = 0; pos < buffer.size();) {
            auto p = search(buffer.begin() + pos, buffer.end(), pattern.begin(), pattern.end());
            if (p == buffer.end()) break;

            pos = distance(buffer.begin(), p);
            uint64_t target = AddRip(pos);

            if (target == stringOffset && pos >= 13 && buffer[pos - 13] == 0x4C && buffer[pos - 12] == 0x8D && buffer[pos - 11] == 0x05) {
                return AddRip(pos - 13);
            }

            pos += 1;
        }

        return 0;
    }

    inline uintptr_t FindStringReference(const std::vector<uint8_t>& buf, uintptr_t stringOffset)
    {
        for (size_t pos = 0; pos + 7 < buf.size(); pos++)
        {
            if ((buf[pos] & 0xF8) == 0x48 && buf[pos + 1] == 0x8D)
            {
                int32_t disp;
                memcpy(&disp, &buf[pos + 3], sizeof(int32_t));

                uintptr_t target = pos + 7 + disp;

                if (target == stringOffset)
                    return pos;
            }
        }

        return 0;
    }

    inline uintptr_t FindBranchReference(const std::vector<uint8_t>& buf, uintptr_t targetOffset)
    {
        for (size_t pos = 0; pos < buf.size(); pos++)
        {
            if (buf[pos] == 0x74)
            {
                if (pos + 2 > buf.size())
                    continue;

                int8_t disp;
                memcpy(&disp, &buf[pos + 1], sizeof(int8_t));

                uintptr_t ref = pos + 2 + disp;

                if (ref == targetOffset)
                    return pos;
            }

            if (buf[pos] == 0x0F && pos + 6 <= buf.size() && buf[pos + 1] == 0x84)
            {
                int32_t disp;
                memcpy(&disp, &buf[pos + 2], sizeof(int32_t));

                uintptr_t ref = pos + 6 + disp;

                if (ref == targetOffset)
                    return pos;
            }
        }

        return 0;
    }
}
