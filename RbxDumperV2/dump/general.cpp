#include "../globals.h"

inline auto isAlnumString = [](const string& str) -> bool {
    for (char c : str) {
        if (!isalnum(static_cast<unsigned char>(c))) return false;
    }
    return true;
};

enum class AttributeType : __int32 {
    String = 0,
    Boolean,
    Number
};

void Dump::General() {
    Globals::offsets.add("This", "Instance", 0x8, "unsigned __int64");

    uintptr_t nameOffset = 0;
    uintptr_t nameBase = 0;

    for (int i = 0; i < 0x200; i++)
    {
        uintptr_t one = Mem::Read<uintptr_t>(Globals::dataModel + i);
        if (!Mem::IsValid(one)) continue;

        for (int j = 0; j < 0x40; j++)
        {
            string s = Mem::ReadString(one + j);
            if (s == "Ugc")
            {
                nameBase = i;
                nameOffset = j;
                goto foundname;
            }
        }
    }

foundname:
    if (nameOffset || nameBase)
    {
        Globals::offsets.add("NameContainer", "Instance", nameBase, "unsigned __int64");
        Globals::offsets.add("Name", "Instance", nameOffset, "unsigned __int64");
    }
    else
    {
        Logger::Log(Logger::Level::Warning, "!!!!! Name failed to bruteforce !!!!!");
    }

    uintptr_t childrenStartOffset = 0;
    uintptr_t nameBaseOff = Globals::offsets.get("NameContainer", "Instance").value;
    uintptr_t nameFieldOff = Globals::offsets.get("Name", "Instance").value;

    for (int i = 0; i < 0x1000; i++)
    {
        uintptr_t childrenListPtr = Mem::Read<uintptr_t>(Globals::dataModel + i);
        if (!Mem::IsValid(childrenListPtr)) continue;

        uintptr_t listStart = Mem::Read<uintptr_t>(childrenListPtr);
        if (!Mem::IsValid(listStart)) continue;

        bool found = false;
        for (int i2 = 0; i2 < 100; i2++)
        {
            uintptr_t child = Mem::Read<uintptr_t>(listStart + i2 * 0x10);
            if (!Mem::IsValid(child)) continue;

            uintptr_t childNameBase = Mem::Read<uintptr_t>(child + nameBaseOff);
            if (!Mem::IsValid(childNameBase)) continue;

            string name = Mem::ReadString(childNameBase + nameFieldOff);

            if (name == "Workspace" || name == "Players" || name == "Run Service")
            {
                childrenStartOffset = i;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    Globals::offsets.add("ChildrenStart", "Instance", childrenStartOffset, "unsigned __int64");
    Globals::offsets.add("ChildrenEnd", "Instance", 0x8, "unsigned __int64");

    uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
    uintptr_t workspaceString = (Mem::Read<uintptr_t>(workspace + nameBaseOff) + nameFieldOff);

    Helper::DoBasicScan(BasicScanConfig<int>(
        "StringLength",
        "Misc",
        { workspaceString },
        { 9 }
    ));

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Parent",
        "Instance",
        { workspace },
        { Globals::dataModel }
    ));

    uintptr_t classDescriptorOffset = 0;
    uintptr_t classNameOffset = 0x8;

    while (true) {
        string className = Mem::ReadString(Mem::Read<uintptr_t>(Mem::Read<uintptr_t>(Globals::dataModel + classDescriptorOffset) + classNameOffset));
        if (className == "DataModel") {
            break;
        }

        classDescriptorOffset++;
    }

    Globals::offsets.add("ClassDescriptor", "Instance", classDescriptorOffset, "unsigned __int64");
    Globals::offsets.add("ClassName", "Instance", classNameOffset, "string");

    uintptr_t baseClassOffset = 0x150;
    while (true) {
        uintptr_t classDescriptor = Mem::Read<uintptr_t>(workspace + classDescriptorOffset);
        uintptr_t baseClass = Mem::Read<uintptr_t>(classDescriptor + baseClassOffset);
        string baseClassName = Mem::ReadString(Mem::Read<uintptr_t>(baseClass + classNameOffset));

        if (baseClassName == "WorldRoot") {
            break;
        }
        baseClassOffset++;
    }
    Globals::offsets.add("ClassBase", "Instance", baseClassOffset, "unsigned __int64");

    // Compatibility, no longer dumping since i cant be bothered to fix it
    Globals::offsets.add("Cache", "MeshContentProvider", 0, "unsigned __int64");
    Globals::offsets.add("LRUCache", "MeshContentProvider", 0, "unsigned __int64");
    Globals::offsets.add("MeshData", "MeshContentProvider", 0, "unsigned __int64");
    Globals::offsets.add("ToMeshData", "MeshContentProvider", 0, "unsigned __int64");
    Globals::offsets.add("AssetID", "MeshContentProvider", 0, "string");

    Globals::offsets.add("FaceEnd", "MeshData", 0, "unsigned __int64");
    Globals::offsets.add("FaceStart", "MeshData", 0, "unsigned __int64");
    Globals::offsets.add("VertexEnd", "MeshData", 0, "unsigned __int64");
    Globals::offsets.add("VertexStart", "MeshData", 0, "unsigned __int64");

    uintptr_t knownInstance = Roblox::FindFirstChild(workspace, "Position");
    bool found = false;

    // broken rn, roblox changed attribute method again probably
    if (knownInstance && Mem::IsValid(knownInstance))
    {
        for (int compMap = 0x20; compMap <= 0x50; compMap += 4)
        {
            uintptr_t component = Mem::Read<uintptr_t>(knownInstance + compMap);
            if (!component || !Mem::IsValid(component)) continue;

            uintptr_t start = Mem::Read<uintptr_t>(component);
            uintptr_t end = Mem::Read<uintptr_t>(component + 8);

            if (!start || !end || end <= start || (end - start) > 0x2000) continue;

            for (int attrStride = 0x30; attrStride <= 0x70; attrStride += 8)
            {
                for (int keyOff = 0x0; keyOff <= 0x10; keyOff += 4)
                {
                    for (int valueOff = 0x10; valueOff <= 0x30; valueOff += 4)
                    {
                        int foundCount = 0;

                        for (uintptr_t index = 0; index < end - start; index += 0x10)
                        {
                            uintptr_t entry = Mem::Read<uintptr_t>(start + index);
                            if (!entry || !Mem::IsValid(entry)) continue;

                            uintptr_t listing = Mem::Read<uintptr_t>(entry + 0x10);
                            if (!listing || !Mem::IsValid(listing)) continue;

                            for (int step = 0; step < attrStride * 32; step += attrStride)
                            {
                                uintptr_t namePtr = Mem::Read<uintptr_t>(listing + step + keyOff);
                                if (!namePtr || !Mem::IsValid(namePtr)) break;

                                std::string attrName = Mem::ReadString(namePtr);
                                if (attrName.empty() || attrName.length() > 64) break;

                                uintptr_t valueAddr = listing + step + valueOff;
                                std::string attrValue = Mem::ReadString(valueAddr);

                                if ((attrName == "test" && attrValue == "meow") ||
                                    (attrName == "test2" && attrValue == "meow2"))
                                {
                                    foundCount++;
                                }

                                if (foundCount >= 2)
                                {
                                    Globals::offsets.add("ComponentMap", "Instance", compMap, "unsigned __int64");
                                    Globals::offsets.add("Length", "AttributesMap", 0, "unsigned __int64");
                                    Globals::offsets.add("Attributes", "AttributesMap", 0x10, "unsigned __int64");
                                    Globals::offsets.add("Key", "Attribute", keyOff, "unsigned __int64");
                                    Globals::offsets.add("Value", "Attribute", valueOff, "unsigned __int64");
                                    Globals::offsets.add("Size", "Attribute", attrStride, "unsigned __int64");

                                    found = true;
                                    break;
                                }
                            }
                            if (found) break;
                        }
                        if (found) break;
                    }
                    if (found) break;
                }
                if (found) break;
            }
            if (found) break;
        }
    }

    if (!found) {
        Logger::Log(Logger::Level::Warning, "Failed to find attribute offsets!");
    }

    uintptr_t boxHandleAdornment = Roblox::FindFirstChild(workspace, "BoxHandleAdornment");
    uintptr_t posPart = Roblox::FindFirstChild(workspace, "Position");

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Adornee",
        "Misc",
        { boxHandleAdornment },
        { posPart }
    ));
}
