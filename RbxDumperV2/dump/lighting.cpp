#include "../globals.h"

inline auto isValidChannel = [](float c) {
	return c > 0.0f && c < 1.0f &&
		(c > 0.001f && c < 0.999f);
};

void Dump::Lighting() {
	uintptr_t lighting = Roblox::FindFirstChild(Globals::dataModel, "Lighting");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ClockTime",
		"Lighting",
		{ lighting },
		{ -0.00897f },
		0x0,
		0x1000,
		0x1,
		false,
		0.0001f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Brightness",
		"Lighting",
		{ lighting },
		{ 2.54f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"EnvironmentDiffuseScale",
		"Lighting",
		{ lighting },
		{ 0.872f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"EnvironmentSpecularScale",
		"Lighting",
		{ lighting },
		{ 0.233f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"FogStart",
		"Lighting",
		{ lighting },
		{ 722.1f },
		0x0,
		0x1000,
		0x1,
		false,
		0.1f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"FogEnd",
		"Lighting",
		{ lighting },
		{ 100000.f },
		0x0,
		0x1000,
		0x1,
		false,
		0.1f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"FogColor",
		"Lighting",
		{ lighting },
		{ Color3{ 0.752941f, 0.752941f, 0.752941f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"Ambient",
		"Lighting",
		{ lighting },
		{ Color3{ 0.541f, 0.541f, 0.541f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"OutdoorAmbient",
		"Lighting",
		{ lighting },
		{ Color3{ 0.502f, 0.502f, 0.502f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"ColorShift_Top",
		"Lighting",
		{ lighting },
		{ Color3{ 0.121569f, 0.705882f, 0.207843f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"ColorShift_Bottom",
		"Lighting",
		{ lighting },
		{ Color3{ 0.070588f, 0.062745f, 0.027451f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"ExposureCompensation",
		"Lighting",
		{ lighting },
		{ 2.13f },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"GeographicLatitude",
		"Lighting",
		{ lighting },
		{ 115.231f },
		0x0,
		0x1000,
		0x1,
		false,
		1.f
	));

	for (int i = 0; i < 0x1000; i++) {
		Color3 col = Mem::Read<Color3>(lighting + i);
		if (!isValidChannel(col.r)) continue;
		if (!isValidChannel(col.g)) continue;
		if (!isValidChannel(col.b)) continue;

		if (abs(col.r - 0.75f) <= 0.01f &&
			abs(col.g - 0.75f) <= 0.01f &&
			abs(col.b - 0.75f) <= 0.01f && i >= 0x120) {
			Globals::offsets.add("LightColor", "Lighting", i, "Color3");
		}
	}

	uintptr_t gradientTop = 0;
	for (int i = 0; i < 0x1000; i++) {
		Vector3 val = Mem::Read<Vector3>(lighting + i);

		static bool foundLight = false;
		if (abs(val.x - 0.0151139f) <= 0.01f &&
			abs(val.y - -0.026178f) <= 0.01f &&
			abs(val.z - 0.999543f) <= 0.01f && !foundLight) {
			foundLight = true;
			Globals::offsets.add("LightDirection", "Lighting", i, "Vector3");
		}

		static bool foundGrad1 = false;
		static bool foundGrad2 = false;
		if (val.x == 1 && val.y == 1 && val.z == 1 && !foundGrad1) {
			gradientTop = i;
			foundGrad1 = true;
			Globals::offsets.add("GradientTop", "Lighting", i, "Color3");
		}
		else if (val.x == 1 && val.y == 1 && val.z == 1 && !foundGrad2) {
			foundGrad2 = true;
			Globals::offsets.add("GradientBottom", "Lighting", i, "Color3");
		}
	}

	Helper::DoBasicScan(BasicScanConfig<bool>(
		"GlobalShadows",
		"Lighting",
		{ lighting },
		{ true },
		gradientTop - sizeof(Vector3) - 0x2
	));

	uintptr_t moonPosOffset = Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"MoonPosition",
		"Lighting",
		{ lighting },
		{ Vector3(-0.0151139f, 0.026178f, 0.999543f) }
	)).value;

	uintptr_t sunPosOffset = Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"SunPosition",
		"Lighting",
		{ lighting },
		{ Vector3(0.0151139f, -0.026178f, 0.999543f) },
		moonPosOffset - sizeof(Vector3) - 0x2
	)).value;

	Globals::offsets.add("Source", "Lighting", sunPosOffset - 0x4, "int");

	Helper::DoBasicScan(BasicScanConfig<uintptr_t>(
		"Sky",
		"Lighting",
		{ lighting },
		{ Roblox::FindFirstChild(lighting, "Sky") }
	));

	uintptr_t workspace = Roblox::FindFirstChild(Globals::dataModel, "Workspace");
	uintptr_t sky = Roblox::FindFirstChild(workspace, "Sky");

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxBk",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_bk.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxDn",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_dn.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxFt",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_ft.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxLf",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_lf.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxRt",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_rt.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SkyboxUp",
		"Sky",
		{ sky },
		{ "rbxasset://textures/sky/sky512_up.tex" }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"SunAngularSize",
		"Sky",
		{ sky },
		{ 21.f },
		0x0,
		0x1000,
		0x1,
		false,
		1.f
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"MoonAngularSize",
		"Sky",
		{ sky },
		{ 11.f },
		0x0,
		0x1000,
		0x1,
		false,
		1.f
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"SunTextureId",
		"Sky",
		{ sky },
		{ "rbxasset://sky/sun.jpg" }
	));

	Helper::DoBasicScan(BasicScanConfig<string>(
		"MoonTextureId",
		"Sky",
		{ sky },
		{ "rbxasset://sky/moon.jpg" }
	));

	Helper::DoBasicScan(BasicScanConfig<Vector3>(
		"SkyboxOrientation",
		"Sky",
		{ sky },
		{ Vector3{ 123.f, 22.f, 83.f } },
		0x0,
		0x1000,
		0x1,
		false,
		0.01f
	));

	Helper::DoBasicScan(BasicScanConfig<int>(
		"StarCount",
		"Sky",
		{ sky },
		{ 3000 },
		0x0,
		0x1000,
		0x1,
		false,
		1.f
	));

	uintptr_t atmosphere = Roblox::FindFirstChild(lighting, "Atmosphere");
	Helper::DoBasicScan(BasicScanConfig<float>(
		"Density",
		"Atmosphere",
		{ atmosphere },
		{ 0.315f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Offset",
		"Atmosphere",
		{ atmosphere },
		{ 0.243f }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"Color",
		"Atmosphere",
		{ atmosphere },
		{ Color3{ 0.541f, 0.541f, 0.541f } }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"Decay",
		"Atmosphere",
		{ atmosphere },
		{ Color3(18, 16, 7) }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Glare",
		"Atmosphere",
		{ atmosphere },
		{ 0.125f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Haze",
		"Atmosphere",
		{ atmosphere },
		{ 0.372f }
	));

	uintptr_t bloom = Roblox::FindFirstChild(lighting, "Bloom");
	Helper::DoBasicScan(BasicScanConfig<float>(
		"Intensity",
		"BloomEffect",
		{ bloom },
		{ 0.652f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Size",
		"BloomEffect",
		{ bloom },
		{ 5.123f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Threshold",
		"BloomEffect",
		{ bloom },
		{ 4.231f }
	));

	uintptr_t bloom2 = Roblox::FindFirstChild(lighting, "Bloom2");
	uintptr_t bloom3 = Roblox::FindFirstChild(lighting, "Bloom3");
	uintptr_t bloom4 = Roblox::FindFirstChild(lighting, "Bloom4");

	uintptr_t enabledOffset = Helper::DoBasicScan(BasicScanConfig<bool>(
		"Enabled",
		"BloomEffect",
		{ bloom, bloom2, bloom3, bloom4 },
		{ true, false, false, false }
	)).value;

	uintptr_t depthOfField = Roblox::FindFirstChild(lighting, "DepthOfField");
	Helper::DoBasicScan(BasicScanConfig<float>(
		"FocusDistance",
		"DepthOfFieldEffect",
		{ depthOfField },
		{ 93.82f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"FarIntensity",
		"DepthOfFieldEffect",
		{ depthOfField },
		{ 0.259f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"NearIntensity",
		"DepthOfFieldEffect",
		{ depthOfField },
		{ 0.173f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"InFocusRadius",
		"DepthOfFieldEffect",
		{ depthOfField },
		{ 9.875f }
	));

	Globals::offsets.add("Enabled", "DepthOfFieldEffect", enabledOffset, "bool");

	uintptr_t sunRays = Roblox::FindFirstChild(lighting, "SunRays");
	Helper::DoBasicScan(BasicScanConfig<float>(
		"Intensity",
		"SunRaysEffect",
		{ sunRays },
		{ 0.296f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Spread",
		"SunRaysEffect",
		{ sunRays },
		{ 0.827f }
	));

	Globals::offsets.add("Enabled", "SunRaysEffect", enabledOffset, "bool");

	uintptr_t colorCorrection = Roblox::FindFirstChild(lighting, "ColorCorrection");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Brightness",
		"ColorCorrectionEffect",
		{ colorCorrection },
		{ 0.124f }
	));

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Contrast",
		"ColorCorrectionEffect",
		{ colorCorrection },
		{ 0.132f }
	));

	Helper::DoBasicScan(BasicScanConfig<Color3>(
		"TintColor",
		"ColorCorrectionEffect",
		{ colorCorrection },
		{ Color3(126, 139, 82) }
	));

	Globals::offsets.add("Enabled", "ColorCorrectionEffect", enabledOffset, "bool");

	uintptr_t colorGrading = Roblox::FindFirstChild(lighting, "ColorGrading");
	uintptr_t colorGrading2 = Roblox::FindFirstChild(lighting, "ColorGrading2");
	uintptr_t colorGrading3 = Roblox::FindFirstChild(lighting, "ColorGrading3");
	uintptr_t colorGrading4 = Roblox::FindFirstChild(lighting, "ColorGrading4");

	Helper::DoBasicScan(BasicScanConfig<int>(
		"TonemapperPreset",
		"ColorGradingEffect",
		{ colorGrading, colorGrading2, colorGrading3, colorGrading4 },
		{ 0, 0, 1, 1 }
	));

	Globals::offsets.add("Enabled", "ColorGradingEffect", enabledOffset, "bool");

	uintptr_t blurEffect = Roblox::FindFirstChild(lighting, "Blur");

	Helper::DoBasicScan(BasicScanConfig<float>(
		"Size",
		"BlurEffect",
		{ blurEffect },
		{ 32.213f }
	));

	Globals::offsets.add("Enabled", "BlurEffect", enabledOffset, "bool");
}
