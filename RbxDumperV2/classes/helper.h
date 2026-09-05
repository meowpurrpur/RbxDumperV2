#pragma once

template<typename T>
class BasicScanConfig {
public:
	string name;
	string catagory;

	vector<uintptr_t> targetInstances;
	vector<T> expectedValues;

	uintptr_t start = 0x0;
	uintptr_t end = 0x1000;
	uintptr_t step = 0x1;

	bool doubleRead = false;
	float elipson = 0.001f;

	BasicScanConfig(string name, string catagory, vector<uintptr_t> targetInstances, vector<T> expectedValues, uintptr_t start = 0x0, uintptr_t end = 0x1000, uintptr_t step = 0x1, bool doubleRead = false, float elipson = 0.001f) {
		this->name = name;
		this->catagory = catagory;
		this->targetInstances = targetInstances;
		this->expectedValues = expectedValues;
		this->start = start;
		this->end = end;
		this->step = step;
		this->doubleRead = doubleRead;
		this->elipson = elipson;
	}
};

class RTTIScanConfig {
public:
	string name;
	string catagory;
	string type;

	vector<uintptr_t> targetInstances;
	vector<string> expectedValues;

	uintptr_t start = 0x0;
	uintptr_t end = 0x1000;
	uintptr_t step = 0x1;

	RTTIScanConfig(string name, string catagory, vector<uintptr_t> targetInstances, vector<string> expectedValues, string type = "unknown", uintptr_t start = 0x0, uintptr_t end = 0x1000, uintptr_t step = 0x1) {
		this->name = name;
		this->catagory = catagory;
		this->targetInstances = targetInstances;
		this->expectedValues = expectedValues;
		this->start = start;
		this->end = end;
		this->step = step;
		this->type = type;
	}
};

namespace Helper {
	inline string NormalizeTypeName(const string& input)
	{
		string result = input;

		if (result.rfind("struct ", 0) == 0)
			result.erase(0, 7);

		if (result.rfind("class ", 0) == 0)
			result.erase(0, 6);

		size_t pos = result.find("std::basic_string");
		if (pos != string::npos)
			result = "string";

		return result;
	}

	template<typename T>
	inline RbxDumper::Offset DoBasicScan(BasicScanConfig<T> config)
	{
		string typeString = NormalizeTypeName(typeid(T).name());

		for (uintptr_t i = config.start; i < config.end; i += config.step) {
			bool allMatch = true;

			for (size_t j = 0; j < config.targetInstances.size(); j++) {
				uintptr_t addressToCheck = config.targetInstances[j] + i;
				T valueAtAddress;

				if constexpr (is_same_v<T, string>)
				{
					if (config.doubleRead) {
						uintptr_t firstRead = Mem::Read<uintptr_t>(addressToCheck);
						addressToCheck = firstRead;
					}

					valueAtAddress = Mem::ReadString(addressToCheck);
				}
				else {
					valueAtAddress = Mem::Read<T>(addressToCheck);
				}

				if constexpr (is_floating_point_v<T>) {
					T expected = config.expectedValues[j];
					if (isnan(valueAtAddress) || abs(valueAtAddress - expected) > config.elipson) {
						allMatch = false;
						break;
					}
				}
				else {
					if (valueAtAddress != config.expectedValues[j]) {
						allMatch = false;
						break;
					}
				}
			}

			if (allMatch) {
				RbxDumper::Offset offset(config.name, config.catagory, i, typeString);
				Globals::offsets.add(offset);
				return offset;
			}
		}

		Logger::Log(Logger::Level::Warning, "No offset found for " + config.catagory + "::" + config.name);

		RbxDumper::Offset offset(config.name, config.catagory, 0, typeString);
		Globals::offsets.add(offset);
		return offset;
	}

	RbxDumper::Offset DoRTTIScan(RTTIScanConfig config);
	uintptr_t DoPatternScan(string pattern, bool printInstructions = false, uintptr_t offset = 0);
}
