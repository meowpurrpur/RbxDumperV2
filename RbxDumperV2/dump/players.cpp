#include "../globals.h"

bool isInRange(double value, double min, double max) {
	return value >= min && value <= max;
}

vector<string> splitString2(const string& input, char delimiter) {
	vector<string> result;
	string current;
	for (char c : input) {
		if (c == delimiter) {
			if (!current.empty()) result.push_back(current);
			current.clear();
		}
		else {
			current += c;
		}
	}
	if (!current.empty()) result.push_back(current);
	return result;
}

namespace Structs
{
	struct Humanoid {
		char pad_0[32];
		int humanoidStateId;
		char pad_1[252];
		uintptr_t seatPart;
		char pad_2[8];
		uintptr_t moveToPart;
		char pad_3[8];
		Vector3 cameraOffset;
		char pad_4[12];
		Vector3 moveDirection;
		Vector3 targetPoint;
		char pad_5[12];
		Vector3 moveToPoint;
		char pad_6[4];
		int displayDistanceType;
		int floorMaterial;
		float health;
		float healthDisplayDistance;
		int healthDisplayType;
		float hipHeight;
		char pad_7[8];
		float jumpHeight;
		float jumpPower;
		float maxHealth;
		float maxSlopeAngle;
		float nameDisplayDistance;
		int nameOcclusion;
		char pad_8[4];
		int rigType;
		char pad_9[8];
		float walkspeed;
		bool autoJumpEnabled;
		bool autoRotate;
		char pad_10[1];
		bool breakJointsOnDeath;
		bool evaluateStateMachine;
		bool jump;
		char pad_11[1];
		bool platformStand;
		bool sit;
		bool requiresNeck;
		char pad_12[474];
		float walkspeedCheck;
		char pad_13[256];
		uintptr_t humanoidRootPart;
		char pad_14[1040];
		int humanoidState;
		char pad_15[122];
		bool isWalking;
	}; // sizeof = 2391
}

void Dump::Players() {
	uintptr_t players = Roblox::FindFirstChild(Globals::dataModel, "Players");
	uintptr_t localPlayer = Roblox::GetChildren(players)[0];

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"LocalPlayer",
		"Player",
		{ players },
		{ localPlayer }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"UserId",
		"Player",
		{ localPlayer },
		{ DumperConstants::userId }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"DisplayName",
		"Player",
		{ localPlayer },
		{ DumperConstants::userDisplayName },
		0x100
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"HealthDisplayDistance",
		"Player",
		{ localPlayer },
		{ 132.233f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"NameDisplayDistance",
		"Player",
		{ localPlayer },
		{ 342.853f }
	));

	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t character = Roblox::FindFirstChild(workspace, Roblox::GetName(localPlayer));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"ModelInstance",
		"Player",
		{ localPlayer },
		{ character }
	));

	uintptr_t teams = Roblox::FindFirstChild(Globals::dataModel, "Teams");
	uintptr_t meowTeam = Roblox::FindFirstChild(teams, "Meow");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Team",
		"Player",
		{ localPlayer },
		{ meowTeam }
	));

	Helper::DoBasicScan(BasicScanConfig<int32_t>(
		"BrickColor",
		"Team",
		{ meowTeam },
		{ 1015 }
	));

	Helper::DoBasicScan(BasicScanConfig<int32_t>(
		"TeamColor",
		"Player",
		{ localPlayer },
		{ 1015 }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"LocaleId",
		"Player",
		{ localPlayer },
		{ "GB" }
	));

	uintptr_t accountAgeFolder = Roblox::FindFirstChild(workspace, "AccountAge");
	uintptr_t accountAgePart = Roblox::GetChildren(accountAgeFolder)[0];
	int accountAge = stoi(Roblox::GetName(accountAgePart));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"AccountAge",
		"Player",
		{ localPlayer },
		{ accountAge }
	));

	uintptr_t currentCamera = Roblox::FindFirstChild(workspace, "Camera");
	uintptr_t humanoid = Roblox::FindFirstChild(character, "Humanoid");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Health",
		"Humanoid",
		{ humanoid },
		{ 52.382f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxHealth",
		"Humanoid",
		{ humanoid },
		{ 88.817f }
	));

	RbxDumper::Offset walkspeedOffset = Helper::DoBasicScan(BasicScanConfig<float>(
		"Walkspeed",
		"Humanoid",
		{ humanoid },
		{ 18.827f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"WalkspeedCheck",
		"Humanoid",
		{ humanoid },
		{ 18.827f },
		walkspeedOffset.value + 1
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"JumpPower",
		"Humanoid",
		{ humanoid },
		{ 27.322f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"JumpHeight",
		"Humanoid",
		{ humanoid },
		{ 7.812f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"HipHeight",
		"Humanoid",
		{ humanoid },
		{ 1.998f },
		0x100
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxSlopeAngle",
		"Humanoid",
		{ humanoid },
		{ 89.9f }
	));

	uintptr_t rHumanoid = Roblox::FindFirstChild(workspace, "Humanoid");
	uintptr_t rig = Roblox::FindFirstChild(workspace, "Rig");
	uintptr_t rigHumanoid = Roblox::FindFirstChild(rig, "Humanoid");

	uintptr_t rHumanoid2 = Roblox::FindFirstChild(workspace, "Humanoid2");
	uintptr_t rHumanoid3 = Roblox::FindFirstChild(workspace, "Humanoid3");
	uintptr_t rHumanoid4 = Roblox::FindFirstChild(workspace, "Humanoid4");
	uintptr_t rHumanoid5 = Roblox::FindFirstChild(workspace, "Humanoid5");
	uintptr_t rHumanoid6 = Roblox::FindFirstChild(workspace, "Humanoid6");
	uintptr_t rHumanoid7 = Roblox::FindFirstChild(workspace, "Humanoid7");

	uintptr_t noobNpc = Roblox::FindFirstChild(workspace, "Noob");
	uintptr_t humanoidNoob = Roblox::FindFirstChild(noobNpc, "Humanoid");

	uintptr_t sittingNpc = Roblox::FindFirstChild(workspace, "SIT");
	uintptr_t sittingNpcHumanoid = Roblox::FindFirstChild(sittingNpc, "Humanoid");
	uintptr_t sittingNpcRoot = Roblox::FindFirstChild(sittingNpc, "HumanoidRootPart");
	uintptr_t seat = Roblox::FindFirstChild(workspace, "Seat");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"SeatPart",
		"Humanoid",
		{ sittingNpcHumanoid },
		{ seat }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"HumanoidRootPart",
		"Humanoid",
		{ sittingNpcHumanoid },
		{ sittingNpcRoot }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"CameraOffset",
		"Humanoid",
		{ rHumanoid },
		{ Vector3(13.232f, 14.532f, 0.231f) }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"HealthDisplayDistance",
		"Humanoid",
		{ rHumanoid },
		{ 734.457f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"NameDisplayDistance",
		"Humanoid",
		{ rHumanoid },
		{ 342.789f }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"DisplayDistanceType",
		"Humanoid",
		{ rHumanoid, rHumanoid2, rHumanoid3, rHumanoid4 },
		{ 0, 1, 2, 0 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"HealthDisplayType",
		"Humanoid",
		{ rHumanoid5, rHumanoid6, rHumanoid7, rHumanoid4 },
		{ 0, 2, 1, 0 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"NameOcclusion",
		"Humanoid",
		{ rHumanoid5, rHumanoid6, rHumanoid7, rHumanoid4 },
		{ 1, 0, 1, 2 }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"DisplayName",
		"Humanoid",
		{ rHumanoid },
		{ "wow" }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"MoveDirection",
		"Humanoid",
		{ humanoidNoob },
		{ Vector3(-0.6884002089500427f, 0.43886613845825195f, 0.5774961709976196f) }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"RigType",
		"Humanoid",
		{ humanoid, rHumanoid, rigHumanoid },
		{ 1, 0, 0 },
		0x100, 0x200
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Jump",
		"Humanoid",
		{ humanoid, rHumanoid, rHumanoid3, rHumanoid4 },
		{ false, false, true, false },
		0x100, 0x200
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Sit",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ false, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"PlatformStand",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"UseJumpPower",
		"Humanoid",
		{ rHumanoid3, rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, true, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"AutomaticScalingEnabled",
		"Humanoid",
		{ rHumanoid3, rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, true, false, false, false }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"BreakJointsOnDeath",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, true, false, false }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"EvaluateStateMachine",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, false, true, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"RequiresNeck",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ false, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"AutoJumpEnabled",
		"Humanoid",
		{ rHumanoid4, rHumanoid5, rHumanoid6, rHumanoid7 },
		{ true, false, false, true }
	));

	RbxDumper::Offset autoRotateOffset = Helper::DoBasicScan(BasicScanConfig<bool>(
		"AutoRotate",
		"Humanoid",
		{ humanoid, rHumanoid, rigHumanoid, rHumanoid2 },
		{ true, false, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"IsWalking",
		"Humanoid",
		{ humanoid, rHumanoid, rigHumanoid, rHumanoid2, rHumanoid3, rHumanoid4 },
		{ false, true, false, true, false, true },
		autoRotateOffset.value + 1
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"MoveToPoint",
		"Humanoid",
		{ rHumanoid2 },
		{ Vector3(8282.f, 222.243f, 3.232f)}
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"MoveToPart",
		"Humanoid",
		{ rHumanoid4 },
		{ Roblox::FindFirstChild(workspace, "Position") }
	));

	Helper::DoBasicScan(BasicScanConfig<double>(
		"WalkTimer",
		"Humanoid",
		{ rHumanoid4 },
		{ 8.f },
		0x3f0
	));

	RbxDumper::Offset humanoidState = Helper::DoRTTIScan(RTTIScanConfig(
		"HumanoidState",
		"Humanoid",
		{ humanoid },
		{ "RBX::HUMAN::Running" },
		"int"
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"HumanoidStateID",
		"Humanoid",
		{ Mem::Read<uintptr_t>(humanoid + humanoidState.value) },
		{ 8 }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"FloorMaterial",
		"Humanoid",
		{ humanoid },
		{ 1056 }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Occupant",
		"Seat",
		{ seat },
		{ sittingNpcHumanoid }
	));

	uintptr_t vehicleSeat = Roblox::FindFirstChild(workspace, "VehicleSeat");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MaxSpeed",
		"VehicleSeat",
		{ vehicleSeat },
		{ 47.321f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"SteerFloat",
		"VehicleSeat",
		{ vehicleSeat },
		{ 0.892f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ThrottleFloat",
		"VehicleSeat",
		{ vehicleSeat },
		{ 0.118f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Torque",
		"VehicleSeat",
		{ vehicleSeat },
		{ 24.234f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"TurnSpeed",
		"VehicleSeat",
		{ vehicleSeat },
		{ 2.992f }
	));

	uintptr_t mouseHit = Roblox::GetChildren(Roblox::FindFirstChild(workspace, "MouseHit"))[0];
	vector<string> parts = splitString2(Roblox::GetName(mouseHit), ',');
	Vector3 mouseHitPos = Vector3(stof(parts[0]), stof(parts[1]), stof(parts[2]));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"TargetPoint",
		"Humanoid",
		{ humanoid },
		{ mouseHitPos }
	));

	uintptr_t stats = Roblox::FindFirstChild(Globals::dataModel, "Stats");
	uintptr_t stats2 = Roblox::FindFirstChild(stats, "PerformanceStats");
	uintptr_t pingBase = Roblox::FindFirstChild(stats2, "Ping");

	uintptr_t pingOffset = 0x50;
	while (true) {
		double ping = Mem::Read<double>(pingBase + pingOffset);
		if (isInRange(ping, 30.f, 60.f)) {
			break;
		}

		pingOffset++;
	}
	Globals::offsets.add("Value", "StatsItem", 0xc8, "double");

	uintptr_t backpack = Roblox::FindFirstChild(localPlayer, "Backpack");

	uintptr_t tool = Roblox::FindFirstChild(backpack, "Tool");
	uintptr_t tool2 = Roblox::FindFirstChild(backpack, "Tool2");
	uintptr_t tool3 = Roblox::FindFirstChild(backpack, "Tool3");
	uintptr_t tool4 = Roblox::FindFirstChild(backpack, "Tool4");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Tooltip",
		"Tool",
		{ tool },
		{ "meow" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"TextureId",
		"Tool",
		{ tool },
		{ "rbxassetid://73879578225090" }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"Grip",
		"Tool",
		{ tool },
		{ Vector3(67, 69, 420) }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Enabled",
		"Tool",
		{ tool, tool2, tool3, tool4 },
		{ true, false, true, false }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"CanBeDropped",
		"Tool",
		{ tool, tool2, tool3, tool4 },
		{ false, true, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"ManualActivationOnly",
		"Tool",
		{ tool, tool2, tool3, tool4 },
		{ true, false, true, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"RequiresHandle",
		"Tool",
		{ tool, tool2, tool3, tool4 },
		{ true, true, false, true }
	));

	uintptr_t clothing = Roblox::FindFirstChild(workspace, "Clothing");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"Template",
		"Clothing",
		{ clothing },
		{ "rbxassetid://73879578225090" }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"Color3",
		"Clothing",
		{ clothing },
		{ Color3(17, 199, 255) }
	));

	uintptr_t characterMesh = Roblox::FindFirstChild(workspace, "CharacterMesh");
	uintptr_t characterMesh2 = Roblox::FindFirstChild(workspace, "CharacterMesh2");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"BaseTextureId",
		"CharacterMesh",
		{ characterMesh },
		{ "rbxassetid://3242" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"OverlayTextureId",
		"CharacterMesh",
		{ characterMesh },
		{ "rbxassetid://2732" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"MeshId",
		"CharacterMesh",
		{ characterMesh },
		{ "rbxassetid://5867" }
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"BodyPart",
		"CharacterMesh",
		{ characterMesh, characterMesh2 },
		{ 3, 5 }
	));
}
