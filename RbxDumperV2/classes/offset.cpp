#include "../globals.h"

unordered_map<string, pair<size_t, string>> typeMap = {
	{"unsigned __int64", {sizeof(uintptr_t), "uintptr_t"}},
	{"Vector3", {sizeof(Vector3), "Vector3"}},
	{"Vector2", {sizeof(Vector2), "Vector2"}},
	{"bool", {sizeof(bool), "bool"}},
	{"BYTE", {sizeof(BYTE), "BYTE"}},
	{"float", {sizeof(float), "float"}},
	{"double", {sizeof(double), "double"}},
    {"int", {sizeof(int), "int"}},
	{"UDim2", {sizeof(UDim2), "UDim2"}},
	{"Matrix3x3", {sizeof(Matrix3x3), "Matrix3x3"}},
	{"ViewMatrix_t", {sizeof(ViewMatrix_t), "ViewMatrix_t"}},
};

namespace RbxDumper
{
	string Offset::ToString() const {
		stringstream stream;
		stream << catagory << "::" << name << " = 0x" << hex << value;
		return stream.str();
	}

	string Offset::ToHeader() const {
		stringstream stream;
		stream << "     inline constexpr uintptr_t " << name << " = 0x" << hex << value << dec << ";";
		return stream.str();
	}

	string Offset::ToCS() const {
		stringstream stream;
		stream << "     public const long " << name << " = 0x" << hex << value << dec << ";";
		return stream.str();
	}

	void Offset::Print() const {
		Logger::Log(Logger::Level::Offset, ToString() + " (" + type + ")");
	}

	void OffsetList::Add(Offset offset, bool silent) {
		if (!silent) {
			offset.Print();
		}
		offsets.push_back(offset);
	}

	void OffsetList::AddSilent(string name, string catagory, uintptr_t value)
	{
		Offset offset(name, catagory, value);
		Add(offset, true);
	}

	void OffsetList::Add(string name, string catagory, uintptr_t value)
	{
		Offset offset(name, catagory, value);
		Add(offset, false);
	}

	void OffsetList::Add(string name, string catagory, uintptr_t value, string type)
	{
		Offset offset(name, catagory, value, type);
		Add(offset, false);
	}

	Offset OffsetList::Get(string name, string catagory) {
		for (const Offset& offset : offsets) {
			if (catagory.empty()) {
				if (offset.name == name)
					return offset;
			}
			else {
				if (offset.name == name && offset.catagory == catagory)
					return offset;
			}
		}

		return Offset("", "", 0);
	}

	void OffsetList::DumpToFile(string path, FileType type, bool hex) const
	{
		Logger::Log(Logger::Level::Info, "Writing offsets to path " + path + "...");

		ofstream out(path);
		time_t now = time(nullptr);
		tm localTime;
		localtime_s(&localTime, &now);

		stringstream timestamp;
		timestamp << setfill('0')
			<< setw(2) << localTime.tm_hour << ":"
			<< setw(2) << localTime.tm_min << " "
			<< setw(2) << localTime.tm_mday << "/"
			<< setw(2) << (localTime.tm_mon + 1) << "/"
			<< (localTime.tm_year + 1900);

		switch (type)
		{
		case HPP: {
			out << "#pragma once" << endl;
			out << "/* =============================================================" << endl;
			out << "/*                       theo's offsets                         " << endl;
			out << "/*                  https://offsets.imtheo.lol                  " << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Dumped With     : RbxDumperV2                               " << endl;
			out << "/*  Roblox Version  : " << Globals::clientVersion << endl;
			out << "/*  Dumper Version  : " << Globals::dumperVersion << endl;
			out << "/*  Dumped At       : " << timestamp.str() << " (GMT)" << endl;
			out << "/*  Total Offsets   : " << offsets.size() << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Join the discord!                                           " << endl;
			out << "/*  https://offsets.imtheo.lol/discord                          " << endl;
			out << "/* =============================================================" << endl;
			out << "*/" << endl << endl;

			out << "#include <cstdint>\n#include <string>" << endl;
			map<string, vector<Offset>> sorted;
			for (const auto& o : offsets) {
				sorted[o.catagory].push_back(o);
			}

			out << "namespace " << namespaceName << " {" << endl;
			out << "    inline std::string ClientVersion = \"" << Globals::clientVersion << "\";" << endl << endl;

			for (const auto& [cat, group] : sorted) {
				out << "    namespace " << cat << " {" << endl;

				vector<Offset> sortedGroup = group;
				sort(sortedGroup.begin(), sortedGroup.end(), [](const Offset& a, const Offset& b) {
					return a.name < b.name;
					});

				for (const auto& o : sortedGroup) {
					out << "    " << o.ToHeader() << endl;
				}

				out << "    }" << endl << endl;
			}

			out << "}" << endl;
			out.close();
			break;
		}
		case STRUCTHPP: {
			out << "#pragma once" << endl;
			out << "/* =============================================================" << endl;
			out << "/*                       theo's offsets                         " << endl;
			out << "/*                  https://offsets.imtheo.lol                  " << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Dumped With     : RbxDumperV2                               " << endl;
			out << "/*  Roblox Version  : " << Globals::clientVersion << endl;
			out << "/*  Dumper Version  : " << Globals::dumperVersion << endl;
			out << "/*  Dumped At       : " << timestamp.str() << " (GMT)" << endl;
			out << "/*  Total Offsets   : " << offsets.size() << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Join the discord!                                           " << endl;
			out << "/*  https://offsets.imtheo.lol/discord                          " << endl;
			out << "/* =============================================================" << endl;
			out << "*/" << endl << endl;

		    out << "#include <cstdint>\n#include <string>" << endl;
		    map<string, vector<Offset>> sorted;
		    for (const auto& o : offsets) {
		        sorted[o.catagory].push_back(o);
		    }

		    out << "namespace Structs {" << endl;
		    out << "    inline std::string ClientVersion = \"" << Globals::clientVersion << "\";" << endl << endl;

			for (const auto& [cat, group] : sorted) {
				out << "    struct " << cat << " {" << endl;

				std::vector<Offset> sortedGroup = group;
				std::sort(sortedGroup.begin(), sortedGroup.end(), [](const Offset& a, const Offset& b) {
					return a.value < b.value;
				});

				size_t currentOffset = 0;
				int padCount = 0;

				for (const auto& o : sortedGroup) {
					if (o.value > currentOffset) {
						size_t padSize = o.value - currentOffset;
						out << "        char pad_" << std::to_string(padCount++) << "[0x" << std::hex << padSize << "];" << endl;
						currentOffset = o.value;
					}

					string typeName = o.type;
					if (typeMap.find(o.type) != typeMap.end()) {
						typeName = typeMap[o.type].second;
						currentOffset += typeMap[o.type].first;
					}

					out << "        " << typeName << " " << o.name << "; // 0x" << std::hex << o.value << endl;
				}

				out << "    }; // sizeof = " << currentOffset << endl << endl;
			}

		    out << "}" << endl;
		    out.close();
		    break;
		}
		case CS: {
			out << "/* =============================================================" << endl;
			out << "/*                       theo's offsets                         " << endl;
			out << "/*                  https://offsets.imtheo.lol                  " << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Dumped With     : RbxDumperV2                               " << endl;
			out << "/*  Roblox Version  : " << Globals::clientVersion << endl;
			out << "/*  Dumper Version  : " << Globals::dumperVersion << endl;
			out << "/*  Dumped At       : " << timestamp.str() << " (GMT)" << endl;
			out << "/*  Total Offsets   : " << offsets.size() << endl;
			out << "/* -------------------------------------------------------------" << endl;
			out << "/*  Join the discord!                                           " << endl;
			out << "/*  https://offsets.imtheo.lol/discord                          " << endl;
			out << "/* =============================================================" << endl;
			out << "*/" << endl << endl;

			map<string, vector<Offset>> sorted;
			for (const auto& o : offsets) {
				sorted[o.catagory].push_back(o);
			}

			out << "namespace " << namespaceName << " {" << endl;
			out << "    public static class Info {" << endl;
			out << "        public static string ClientVersion = \"" << Globals::clientVersion << "\";" << endl;
			out << "    }" << endl << endl;

			for (const auto& [cat, group] : sorted) {
				out << "    public static class " << cat << " {" << endl;

				vector<Offset> sortedGroup = group;
				sort(sortedGroup.begin(), sortedGroup.end(), [](const Offset& a, const Offset& b) {
					return a.name < b.name;
					});

				for (const auto& o : sortedGroup) {
					out << "    " << o.ToCS() << endl;
				}

				out << "    }" << endl << endl;
			}

			out << "}" << endl;
			out.close();

			break;
		}
		case JSON: {
			json offsetsJson;
			for (const auto& offsetEntry : offsets) {
				if (hex) {
					stringstream hexStream;
					hexStream << "0x" << std::hex << offsetEntry.value;
					offsetsJson[offsetEntry.catagory][offsetEntry.name] = hexStream.str();
				}
				else {
					offsetsJson[offsetEntry.catagory][offsetEntry.name] = offsetEntry.value;
				}
			}

			json orderedRoot;
			orderedRoot["Source"] = "https://offsets.imtheo.lol";
			orderedRoot["Roblox Version"] = Globals::clientVersion;
			orderedRoot["Dumper Version"] = Globals::dumperVersion;
			orderedRoot["Dumped With"] = "RbxDumperV2";
			orderedRoot["Dumped At"] = timestamp.str();
			orderedRoot["Discord"] = "https://offsets.imtheo.lol/discord";
			orderedRoot["Total Offsets"] = offsets.size();
			orderedRoot[namespaceName] = offsetsJson;

			out << orderedRoot.dump(4);
			out.close();
			break;
		}
		case TYPESJSON: {
			json offsetsJson;
			for (const auto& offsetEntry : offsets) {
				offsetsJson[offsetEntry.catagory][offsetEntry.name] = offsetEntry.type;
			}

			json orderedRoot;
			orderedRoot["Source"] = "https://offsets.imtheo.lol";
			orderedRoot["Roblox Version"] = Globals::clientVersion;
			orderedRoot["Dumper Version"] = Globals::dumperVersion;
			orderedRoot["Dumped With"] = "RbxDumperV2";
			orderedRoot["Dumped At"] = timestamp.str();
			orderedRoot["Discord"] = "https://offsets.imtheo.lol/discord";
			orderedRoot["Types"] = offsetsJson;

			out << orderedRoot.dump(4);
			out.close();
			break;
		}
		case TXT: {
			for (const auto& o : offsets)
				out << o.ToString() << endl;

			out.close();
			break;
		}
		}

		Logger::Log(Logger::Level::Success, "Offsets written to " + path + " successfully.");
	}
}
