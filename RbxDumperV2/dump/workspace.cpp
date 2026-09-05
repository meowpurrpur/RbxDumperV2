#include "../globals.h"

void Dump::Workspace() {
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");

	uintptr_t pointer2Offset = 0;
	uintptr_t realGravityOffset = 0;
	while (true) {
		uintptr_t pointer2 = Mem::Read<uintptr_t>(workspace + pointer2Offset);
		if (!Mem::IsValid(pointer2)) {
			pointer2Offset++;
			continue;
		}

		bool found = false;
		for (int i = 0; i < 0x300; i++) {
			float gravity = Mem::Read<float>(pointer2 + i);
			if (abs(gravity - 165.231f) <= 0.001f) {
				realGravityOffset = i;
				found = true;
				break;
			}
		}

		if (found) break;
		pointer2Offset++;
	}

	Globals::offsets.add("World", "Workspace", pointer2Offset, "unsigned __int64");
	Globals::offsets.add("Gravity", "World", realGravityOffset, "float");

	uintptr_t world = Mem::Read<uintptr_t>(workspace + pointer2Offset);

	Helper::DoBasicScan(BasicScanConfig<float>(
		"worldStepsPerSec",
		"World",
		{ world },
		{ 240.f },
		0x600
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"FallenPartsDestroyHeight",
		"World",
		{ world },
		{ -231.232f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ReadOnlyGravity",
		"Workspace",
		{ workspace },
		{ 165.231f }
	));

	uintptr_t gameTimeFolder = Roblox::FindFirstChild(workspace, "GameTime");
	uintptr_t part = Roblox::GetChildren(gameTimeFolder)[0];

	double realGameTime = stod(Roblox::GetName(part));
	Helper::DoBasicScan(BasicScanConfig<double>(
		"DistributedGameTime",
		"Workspace",
		{ workspace },
		{ realGameTime },
		0x0, 0x1000, 0x1, false, 2.f
	));

	uintptr_t airOffset = 0;
	uintptr_t airDensityOffset = 0;
	for (int i = 0x50; i < 0x250; i++) {
		uintptr_t air = Mem::Read<uintptr_t>(world + i);
		if (!Mem::IsValid(air)) continue;

		bool found = false;
		for (int j = 0x8; j < 0x100; j++) {
			float airDensity = Mem::Read<float>(air + j);
			if (abs(airDensity - 0.201f) <= 0.001f) {
				airOffset = i;
				airDensityOffset = j;
				found = true;
				break;
			}
		}

		if (found) break;
	}

	Globals::offsets.add("AirProperties", "World", airOffset, "unsigned __int64");
	Globals::offsets.add("AirDensity", "AirProperties", airDensityOffset, "float");

	uintptr_t air = Mem::Read<uintptr_t>(world + airOffset);
	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"GlobalWind",
		"AirProperties",
		{ air },
		{ Vector3(154.632f, 155.632f, 156.632f) }
	));

	uintptr_t primCountFolder = Roblox::FindFirstChild(workspace, "PrimCount");
	uintptr_t part2 = Roblox::GetChildren(primCountFolder)[0];
	int primCount = stoi(Roblox::GetName(part2));

	uintptr_t primitivesPointer1Offset = 0x300;
	uintptr_t primitivesPointer2Offset = 0;
	while (true) {
		if (primitivesPointer1Offset > 0x1000) {
			primitivesPointer1Offset = 0;
			Logger::Log(Logger::Level::Warning, "Failed to find PrimitivesPointer1.");
			break;
		}

		uintptr_t primitivesPointer1 = Mem::Read<uintptr_t>(workspace + pointer2Offset);
		if (!Mem::IsValid(primitivesPointer1)) {
			primitivesPointer1Offset++;
			continue;
		}

		bool found = false;
		for (int pointer2 = 0x150; pointer2 < 0x300; pointer2 += 8) {
			uintptr_t primitivesPointer2 = Mem::Read<uintptr_t>(primitivesPointer1 + pointer2);
			if (!Mem::IsValid(primitivesPointer2)) continue;

			int totalPrims2 = 0;
			for (int i = 0x0; i < 0xFFFFFF; i = i + 0x8) {
				uintptr_t primitive = Mem::Read<uintptr_t>(primitivesPointer2 + i);
				if (!primitive || !Mem::IsValid(primitive)) break;

				int isValid = Mem::Read<int>(primitive + 0x8);
				if (isValid == 0x6) {
					totalPrims2++;
				}
			}

			if (totalPrims2 == primCount) {
				primitivesPointer2Offset = pointer2;
				found = true;
				break;
			}
		}

		if (found) break;
		primitivesPointer1Offset++;
	}

	Globals::offsets.add("Primitives", "World", primitivesPointer2Offset);

	uintptr_t allPrimCountFolder = Roblox::FindFirstChild(workspace, "AllPrimCount");
	uintptr_t part3 = Roblox::GetChildren(allPrimCountFolder)[0];
	int allPrimCount = stoi(Roblox::GetName(part3));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"PrimitiveCount",
		"DataModel",
		{ Globals::dataModel },
		{ allPrimCount }
	));

	uintptr_t terrain = Roblox::FindFirstChild(workspace, "Terrain");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"GrassLength",
		"Terrain",
		{ terrain },
		{ 0.723f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"WaterReflectance",
		"Terrain",
		{ terrain },
		{ 0.652f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"WaterTransparency",
		"Terrain",
		{ terrain },
		{ 0.323f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"WaterWaveSize",
		"Terrain",
		{ terrain },
		{ 0.123f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"WaterWaveSpeed",
		"Terrain",
		{ terrain },
		{ 35.234f }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"WaterColor",
		"Terrain",
		{ terrain },
		{ Color3(0.047f, 0.329f, 0.361f) }
	));

	uintptr_t materialColorsOffset = 0;
	for (int i = 0; i < 0x1000; i++) {
		uintptr_t colors = Mem::Read<uintptr_t>(terrain + i);
		if (!Mem::IsValid(colors)) continue;

		bool found = false;
		for (int j = 0x10; j < 0x50; j++) {
			ColorUint_8 color = Mem::Read<ColorUint_8>(colors + j);
			if (color == ColorUint_8(80, 84, 84)) {
				materialColorsOffset = i;
				found = true;
				break;
			}
		}

		if (found) break;
	}
	Globals::offsets.add("MaterialColors", "Terrain", materialColorsOffset, "unsigned __int64");

	vector<pair<string, ColorUint_8>> materials = {
		{"Asphalt", ColorUint_8(80, 84, 84)},
		{"Basalt", ColorUint_8(75, 74, 74)},
		{"Brick", ColorUint_8(138, 97, 73)},
		{"Cobblestone", ColorUint_8(134, 134, 118)},
		{"Concrete", ColorUint_8(152, 152, 152)},
		{"CrackedLava", ColorUint_8(255, 24, 67)},
		{"Glacier", ColorUint_8(221, 228, 229)},
		{"Grass", ColorUint_8(111, 126, 62)},
		{"Ground", ColorUint_8(140, 130, 104)},
		{"Ice", ColorUint_8(204, 210, 223)},
		{"LeafyGrass", ColorUint_8(106, 134, 64)},
		{"Limestone", ColorUint_8(255, 243, 192)},
		{"Mud", ColorUint_8(121, 112, 98)},
		{"Pavement", ColorUint_8(143, 144, 135)},
		{"Rock", ColorUint_8(99, 100, 102)},
		{"Salt", ColorUint_8(255, 255, 254)},
		{"Sand", ColorUint_8(207, 203, 167)},
		{"Sandstone", ColorUint_8(148, 124, 95)},
		{"Slate", ColorUint_8(88, 89, 86)},
		{"Snow", ColorUint_8(235, 253, 255)},
		{"WoodPlanks", ColorUint_8(172, 148, 108)}
	};

	uintptr_t materialColors = Mem::Read<uintptr_t>(terrain + materialColorsOffset);
	for (const auto& [materialName, materialColor] : materials) {
		Helper::DoBasicScan(BasicScanConfig<ColorUint_8>(
			materialName,
			"MaterialColors",
			{ materialColors },
			{ materialColor }
		));
	}

	uintptr_t sound = Roblox::FindFirstChild(workspace, "Sound");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SoundId",
		"Sound",
		{ sound },
		{ "rbxassetid://skibidi" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"RollOffMaxDistance",
		"Sound",
		{ sound },
		{ 2312.321f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"RollOffMinDistance",
		"Sound",
		{ sound },
		{ 2423.213f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"PlaybackSpeed",
		"Sound",
		{ sound },
		{ 1.237f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Volume",
		"Sound",
		{ sound },
		{ 0.523f }
	));

	uintptr_t soundGroup = Roblox::FindFirstChild(workspace, "SoundGroup");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"SoundGroup",
		"Sound",
		{ sound },
		{ soundGroup }
	));

	uintptr_t sound2 = Roblox::FindFirstChild(workspace, "Sound2");
	uintptr_t sound3 = Roblox::FindFirstChild(workspace, "Sound3");
	uintptr_t sound4 = Roblox::FindFirstChild(workspace, "Sound4");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"IsPlaying",
		"Sound",
		{ sound, sound2, sound3, sound4 },
		{ false, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Looped",
		"Sound",
		{ sound, sound2, sound3, sound4 },
		{ false, true, true, false }
	));

	uintptr_t spawnLocation = Roblox::FindFirstChild(workspace, "SpawnLocation");
	uintptr_t spawnLocation2 = Roblox::FindFirstChild(workspace, "SpawnLocation2");
	uintptr_t spawnLocation3 = Roblox::FindFirstChild(workspace, "SpawnLocation3");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"AllowTeamChangeOnTouch",
		"SpawnLocation",
		{ spawnLocation, spawnLocation2, spawnLocation3 },
		{ true, true, false }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Enabled",
		"SpawnLocation",
		{ spawnLocation, spawnLocation2, spawnLocation3 },
		{ true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Neutral",
		"SpawnLocation",
		{ spawnLocation, spawnLocation2, spawnLocation3 },
		{ false, true, false }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"ForcefieldDuration",
		"SpawnLocation",
		{ spawnLocation, spawnLocation2 },
		{ 4345, 4350 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"TeamColor",
		"SpawnLocation",
		{ spawnLocation, spawnLocation2 },
		{ 307, 315 }
	));

	uintptr_t surfaceAppearance = Roblox::FindFirstChild(workspace, "SurfaceAppearance");
	uintptr_t surfaceAppearance2 = Roblox::FindFirstChild(workspace, "SurfaceAppearance2");
	uintptr_t surfaceAppearance3 = Roblox::FindFirstChild(workspace, "SurfaceAppearance3");

	Helper::DoBasicScan(BasicScanConfig<int>(
		"AlphaMode",
		"SurfaceAppearance",
		{ surfaceAppearance, surfaceAppearance2, surfaceAppearance3 },
		{ 0, 2, 1 }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"Color",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ Color3(45, 172, 102) }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"ColorMap",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ "rbxassetid://73879578225090" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"EmissiveMaskContent",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ "rbxassetid://73879578225091" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"EmissiveStrength",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ 3.532f }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"EmissiveTint",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ Color3(18, 100, 231) }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"MetalnessMap",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ "rbxassetid://73879578225092" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"NormalMap",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ "rbxassetid://73879578225093" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"RoughnessMap",
		"SurfaceAppearance",
		{ surfaceAppearance },
		{ "rbxassetid://73879578225094" }
	));

	uintptr_t particleEmitter = Roblox::FindFirstChild(workspace, "ParticleEmitter");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Brightness",
		"ParticleEmitter",
		{ particleEmitter },
		{ 1.993f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"LightEmission",
		"ParticleEmitter",
		{ particleEmitter },
		{ 0.872f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"LightInfluence",
		"ParticleEmitter",
		{ particleEmitter },
		{ 1.332f }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Texture",
		"ParticleEmitter",
		{ particleEmitter },
		{ "rbxasset://textures/particles/sparkles_main.dds" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ZOffset",
		"ParticleEmitter",
		{ particleEmitter },
		{ 832.235f }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"Lifetime",
		"ParticleEmitter",
		{ particleEmitter },
		{ Vector2(5.32f, 10.88f) }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Rate",
		"ParticleEmitter",
		{ particleEmitter },
		{ 33.884f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Rotation",
		"ParticleEmitter",
		{ particleEmitter },
		{ 88.243f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"RotSpeed",
		"ParticleEmitter",
		{ particleEmitter },
		{ 23.856f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Speed",
		"ParticleEmitter",
		{ particleEmitter },
		{ 2.123f }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector2>(
		"SpreadAngle",
		"ParticleEmitter",
		{ particleEmitter },
		{ Vector2(4.233f, 8.354f) }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"Acceleration",
		"ParticleEmitter",
		{ particleEmitter },
		{ Vector3(90.394f, 32.234f, 12.857f) }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Drag",
		"ParticleEmitter",
		{ particleEmitter },
		{ 0.376f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"TimeScale",
		"ParticleEmitter",
		{ particleEmitter },
		{ 0.728f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"VelocityInheritance",
		"ParticleEmitter",
		{ particleEmitter },
		{ 0.238f }
	));

	uintptr_t beam = Roblox::FindFirstChild(workspace, "Beam");
	uintptr_t beamAttach1 = Roblox::FindFirstChild(workspace, "BeamAttach1");
	uintptr_t beamAttach2 = Roblox::FindFirstChild(workspace, "BeamAttach2");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Brightness",
		"Beam",
		{ beam },
		{ 1.775f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"LightEmission",
		"Beam",
		{ beam },
		{ 0.392f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"LightInfluence",
		"Beam",
		{ beam },
		{ 0.745f }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Texture",
		"Beam",
		{ beam },
		{ "rbxassetid://73879578225090" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"TextureLength",
		"Beam",
		{ beam },
		{ 1.625f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"TextureSpeed",
		"Beam",
		{ beam },
		{ 0.642f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ZOffset",
		"Beam",
		{ beam },
		{ 0.975f }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Attachment0",
		"Beam",
		{ beam },
		{ beamAttach1 }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Attachment1",
		"Beam",
		{ beam },
		{ beamAttach2 }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"CurveSize0",
		"Beam",
		{ beam },
		{ 3.324f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"CurveSize1",
		"Beam",
		{ beam },
		{ 7.885f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Width0",
		"Beam",
		{ beam },
		{ 0.328f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Width1",
		"Beam",
		{ beam },
		{ 5.775f }
	));
}
