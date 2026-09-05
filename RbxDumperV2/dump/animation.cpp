#include "../globals.h"

void Dump::Animation() {
	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t animation = Roblox::FindFirstChild(workspace, "Animation");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"AnimationId",
		"Misc",
		{ animation },
		{ "rbxassetid://121903298942078" }
	));

	uintptr_t valueOffset = Globals::offsets.get("Value", "Misc").value;

	uintptr_t animationTrackHolder = Roblox::FindFirstChild(workspace, "AnimationTrackHolder");
	uintptr_t animationTrack = Mem::Read<uintptr_t>(animationTrackHolder + valueOffset);

	uintptr_t animationTrackHolder2 = Roblox::FindFirstChild(workspace, "AnimationTrackHolder2");
	uintptr_t animationTrack2 = Mem::Read<uintptr_t>(animationTrackHolder2 + valueOffset);

	uintptr_t animationTrackHolder3 = Roblox::FindFirstChild(workspace, "AnimationTrackHolder3");
	uintptr_t animationTrack3 = Mem::Read<uintptr_t>(animationTrackHolder3 + valueOffset);

	uintptr_t animationTrackHolder4 = Roblox::FindFirstChild(workspace, "AnimationTrackHolder4");
	uintptr_t animationTrack4 = Mem::Read<uintptr_t>(animationTrackHolder4 + valueOffset);

	uintptr_t rig = Roblox::FindFirstChild(workspace, "Rig");
	uintptr_t humanoid = Roblox::FindFirstChild(rig, "Humanoid");
	uintptr_t animator = Roblox::FindFirstChild(humanoid, "Animator");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Animation",
		"AnimationTrack",
		{ animationTrack },
		{ animation }
	));

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Animator",
		"AnimationTrack",
		{ animationTrack },
		{ animator }
	));

	uintptr_t speedOff = Helper::DoBasicScan(BasicScanConfig<float>(
		"Speed",
		"AnimationTrack",
		{ animationTrack },
		{ 6.218f }
	)).value;
	Globals::offsets.add("TimePosition", "AnimationTrack", speedOff + 4, "float");

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"Looped",
		"AnimationTrack",
		{ animationTrack, animationTrack2, animationTrack3, animationTrack4 },
		{ true, false, false, true }
	));

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"IsPlaying",
		"AnimationTrack",
		{ animationTrack, animationTrack2, animationTrack3, animationTrack4 },
		{ true, false, false, true },
		0x200
	));

	for (int i = 0x600; i < 0x1000; i++) {
		uintptr_t head = Mem::Read<uintptr_t>(animator + i);
		uintptr_t node = Mem::Read<uintptr_t>(head);

		size_t guard = 0;
		bool found = false;

		while (node && node != head && guard++ < 4096) {
			uintptr_t track = Mem::Read<uintptr_t>(node + 0x10);
			if (Mem::IsValid(track) && Roblox::GetName(track) == "Animation") {
				found = true;
				Globals::offsets.add("ActiveAnimations", "Animator", i, "unsigned __int64");
				break;
			}

			node = Mem::Read<uintptr_t>(node);
		}

		if (found) break;
	}
}
