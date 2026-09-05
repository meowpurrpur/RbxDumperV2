#include "../globals.h"

void Dump::Parts() {
    uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");

    // Position
    uintptr_t testPart1 = Roblox::FindFirstChild(workspace, "Position");

    uintptr_t primitiveOffset = 0x100;
    uintptr_t positionOffset = 0;

    for (int i = 0; i < 0x1000; i++) {
        uintptr_t primitive = Mem::Read<uintptr_t>(testPart1 + i);
        bool found = false;

        for (int j = 0x0; j <= 0xFFF; j++) {
            Vector3 position = Mem::Read<Vector3>(primitive + j);

            if (abs(position.x - 255.0f) <= 0.1f &&
                abs(position.y - 84.7f) <= 0.1f &&
                abs(position.z + 255.0f) <= 0.1f) {

                found = true;
                positionOffset = j;
                primitiveOffset = i;
                break;
            }
        }

        if (found) break;
    }

    Globals::offsets.add("Primitive", "BasePart", primitiveOffset, "unsigned __int64");
    Globals::offsets.add("Position", "Primitive", positionOffset, "Vector3");

    // Position
    uintptr_t testPart1Prim = Mem::Read<uintptr_t>(testPart1 + primitiveOffset);

    Globals::offsets.add("Validate", "Primitive", 0x6, "unsigned __int64");
    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Owner",
        "Primitive",
        { testPart1Prim },
        { testPart1 }
    ));

    // Size
    uintptr_t testPart2 = Roblox::FindFirstChild(workspace, "Size");
    // Size
    uintptr_t testPart2Prim = Mem::Read<uintptr_t>(testPart2 + primitiveOffset);

    Helper::DoBasicScan(BasicScanConfig<Vector3>(
        "Size",
        "Primitive",
        { testPart2Prim },
        { Vector3(77.1f, 4.2f, 99.f) }
    ));

    // Rotation
    uintptr_t testPart3 = Roblox::FindFirstChild(workspace, "Rotation");
    // Rotation
    uintptr_t testPart3Prim = Mem::Read<uintptr_t>(testPart3 + primitiveOffset);

    uintptr_t rotationOffset = 0;
    while (true) {
        Matrix3x3 rotationMatrix = Mem::Read<Matrix3x3>(testPart3Prim + rotationOffset);
        Vector3 rotation = rotationMatrix.MatrixToEulerAngles();

        if (abs(rotation.x + 78.81f) <= 0.1f &&
            abs(rotation.y - 16.93f) <= 0.1f &&
            abs(rotation.z - 21.32f) <= 0.1f) {

            break;
        }

        rotationOffset++;
    }
    Globals::offsets.add("Rotation", "Primitive", rotationOffset, "Matrix3x3");

    // 67
    uintptr_t testPart4 = Roblox::FindFirstChild(workspace, "67");
    // 67
    uintptr_t testPart4Prim = Mem::Read<uintptr_t>(testPart4 + primitiveOffset);

    Helper::DoBasicScan(BasicScanConfig<float>(
        "Transparency",
        "BasePart",
        { testPart4, testPart1 },
        { 0.231f, 0.749f }
    ));

    uintptr_t flagsOffset = 0x100;
    while (true) {
        bool collide1 = (Mem::Read<BYTE>(testPart1Prim + flagsOffset) & 0x8) != 0;
        bool collide2 = (Mem::Read<BYTE>(testPart2Prim + flagsOffset) & 0x8) != 0;
        bool collide3 = (Mem::Read<BYTE>(testPart3Prim + flagsOffset) & 0x8) != 0;
        bool collide4 = (Mem::Read<BYTE>(testPart4Prim + flagsOffset) & 0x8) != 0;

        bool anchored1 = (Mem::Read<BYTE>(testPart1Prim + flagsOffset) & 0x2) != 0;
        bool anchored2 = (Mem::Read<BYTE>(testPart2Prim + flagsOffset) & 0x2) != 0;
        bool anchored3 = (Mem::Read<BYTE>(testPart3Prim + flagsOffset) & 0x2) != 0;
        bool anchored4 = (Mem::Read<BYTE>(testPart4Prim + flagsOffset) & 0x2) != 0;

        if (collide1 == true && collide2 == false && collide3 == true && collide4 == true
            && anchored1 == true && anchored2 == true && anchored3 == true && anchored4 == false) {
            break;
        }

        flagsOffset++;
    }
    Globals::offsets.add("Flags", "Primitive", flagsOffset, "BYTE");

    Globals::offsets.add("Anchored", "PrimitiveFlags", 0x2);
    Globals::offsets.add("CanCollide", "PrimitiveFlags", 0x8);
    Globals::offsets.add("CanTouch", "PrimitiveFlags", 0x10);
    Globals::offsets.add("CanQuery", "PrimitiveFlags", 0x20);

    uintptr_t localPlayer = Roblox::GetChildren(Roblox::FindFirstChild(Globals::dataModel, "Players"))[0];
    uintptr_t character = Roblox::FindFirstChild(workspace, Roblox::GetName(localPlayer));
    uintptr_t humanoidRootPart = Roblox::FindFirstChild(character, "HumanoidRootPart");
    uintptr_t humanoidRootPartPrim = Mem::Read<uintptr_t>(humanoidRootPart + primitiveOffset);

    uintptr_t meshPart = Roblox::FindFirstChild(workspace, "MeshPart");

    Helper::DoBasicScan(BasicScanConfig<uint8_t>(
        "Color3",
        "BasePart",
        { testPart1 },
        { 121 }
    ));

    uintptr_t testPart5 = Roblox::FindFirstChild(workspace, "Anchored");
    uintptr_t testPart5Prim = Mem::Read<uintptr_t>(testPart5 + primitiveOffset);

    Helper::DoBasicScan(BasicScanConfig<int>(
        "Material",
        "Primitive",
        { testPart1Prim, testPart2Prim, testPart3Prim, testPart4Prim },
        { 2, 2, 2, 4 }
    ));

    Helper::DoBasicScan(BasicScanConfig<int>(
        "Shape",
        "BasePart",
        { testPart1, testPart2, testPart3 },
        { 1, 0, 2 }
    ));

    uintptr_t testPart6 = Roblox::FindFirstChild(workspace, "Velocity");
    uintptr_t testPart6Prim = Mem::Read<uintptr_t>(testPart6 + primitiveOffset);

    Helper::DoBasicScan(BasicScanConfig<Vector3>(
        "AssemblyLinearVelocity",
        "Primitive",
        { testPart6Prim },
        { Vector3(100.f, 59.2f, 2.f) }
    ));

    Helper::DoBasicScan(BasicScanConfig<Vector3>(
        "AssemblyAngularVelocity",
        "Primitive",
        { testPart6Prim },
        { Vector3(67.f, 67.69f, 6.f) }
    ));

    Helper::DoBasicScan(BasicScanConfig<string>(
        "MeshId",
        "MeshPart",
        { meshPart },
        { "rbxassetid://5281167063" }
    ));

    Helper::DoBasicScan(BasicScanConfig<string>(
        "Texture",
        "MeshPart",
        { meshPart },
        { "rbxassetid://73879578225090" }
    ));

    uintptr_t thePart = Roblox::FindFirstChild(workspace, "ThePart");
    uintptr_t thePart2 = Roblox::FindFirstChild(workspace, "ThePart2");
    uintptr_t thePart3 = Roblox::FindFirstChild(workspace, "ThePart3");
    uintptr_t thePart4 = Roblox::FindFirstChild(workspace, "ThePart4");

    Helper::DoBasicScan(BasicScanConfig<bool>(
        "Massless",
        "BasePart",
        { thePart, thePart2, thePart3, thePart4 },
        { true, false, true, true }
    ));

    Helper::DoBasicScan(BasicScanConfig<bool>(
        "CastShadow",
        "BasePart",
        { thePart, thePart2, thePart3, thePart4 },
        { true, false, true, false }
    ));

    Helper::DoBasicScan(BasicScanConfig<bool>(
        "Locked",
        "BasePart",
        { thePart, thePart2, thePart3, thePart4 },
        { false, true, false, true }
    ));

    Helper::DoBasicScan(BasicScanConfig<float>(
        "Reflectance",
        "BasePart",
        { thePart },
        { 0.832f }
    ));

    uintptr_t value = Roblox::FindFirstChild(workspace, "Value");
    Helper::DoBasicScan(BasicScanConfig<string>(
        "Value",
        "Misc",
        { value },
        { "Value :3" }
    ));

    uintptr_t model = Roblox::FindFirstChild(workspace, "Model");
    uintptr_t part0 = Roblox::GetChildren(model)[0];

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "PrimaryPart",
        "Model",
        { model },
        { part0 }
    ));

    Helper::DoBasicScan(BasicScanConfig<float>(
        "Scale",
        "Model",
        { model },
        { 1.622f }
    ));

    uintptr_t specialMesh = Roblox::FindFirstChild(workspace, "Mesh");

    Helper::DoBasicScan(BasicScanConfig<Vector3>(
        "Scale",
        "SpecialMesh",
        { specialMesh },
        { Vector3(17.2f, 13.f, 23.f) }
    ));

    Helper::DoBasicScan(BasicScanConfig<string>(
        "MeshId",
        "SpecialMesh",
        { specialMesh },
        { "rbxassetid://5281167063" }
    ));

    uintptr_t attachment = Roblox::FindFirstChild(workspace, "Attachment");

    Helper::DoBasicScan(BasicScanConfig<Vector3>(
        "Position",
        "Attachment",
        { attachment },
        { Vector3(12.23f, 24.23f, 1.23f) }
    ));

    uintptr_t weldFolder = Roblox::FindFirstChild(workspace, "Welds");
    uintptr_t weldPart1 = Roblox::FindFirstChild(weldFolder, "Part1");
    uintptr_t weldPart2 = Roblox::FindFirstChild(weldFolder, "Part2");
    uintptr_t weld = Roblox::FindFirstChild(weldFolder, "Weld");
    uintptr_t weldConstraint = Roblox::FindFirstChild(weldFolder, "WeldConstraint");

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Part0",
        "Weld",
        { weld },
        { weldPart1 }
    ));

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Part1",
        "Weld",
        { weld },
        { weldPart2 }
    ));

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Part0",
        "WeldConstraint",
        { weldConstraint },
        { weldPart1 }
    ));

    Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
        "Part1",
        "WeldConstraint",
        { weldConstraint },
        { weldPart2 }
    ));

    uintptr_t union2 = Roblox::FindFirstChild(workspace, "Union2");
    Helper::DoBasicScan(BasicScanConfig<string>(
        "AssetId",
        "UnionOperation",
        { union2 },
        { "https://www.roblox.com//asset/?id=83292086558510" }
    ));
}
