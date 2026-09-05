#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <mutex>

namespace Logger {

	enum class Level {
		Debug,
		Info,
		Success,
		Warning,
		Error,
		Offset
	};

	class LoggerInstance {
	private:
		std::ofstream logFile;
		std::mutex mutex;

		std::string GetTimestamp() {
			auto now = std::chrono::system_clock::now();
			std::time_t time = std::chrono::system_clock::to_time_t(now);
			std::tm timeInfo;
#if defined(_WIN32) || defined(_WIN64)
			localtime_s(&timeInfo, &time);
#else
			localtime_r(&time, &timeInfo);
#endif
			std::ostringstream stream;
			stream << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
			return stream.str();
		}

	public:
		LoggerInstance() {
			HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			DWORD mode = 0;
			if (GetConsoleMode(handle, &mode)) {
				mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(handle, mode);
			}

			std::filesystem::create_directory("Logs");

			auto now = std::chrono::system_clock::now();
			std::time_t time = std::chrono::system_clock::to_time_t(now);
			std::tm timeInfo;
#if defined(_WIN32) || defined(_WIN64)
			localtime_s(&timeInfo, &time);
#else
			localtime_r(&time, &timeInfo);
#endif
			std::ostringstream fileNameStream;
			fileNameStream << "Logs/"
				<< std::put_time(&timeInfo, "%Y-%m-%d_%H-%M-%S")
				<< ".log";

			logFile.open(fileNameStream.str(), std::ios::out);
		}

		void Log(Level logLevel, const std::string& message) {
			std::lock_guard<std::mutex> lock(mutex);

			const char* colorCode = "";
			const char* levelName = "";

			switch (logLevel) {
			case Level::Debug:
				colorCode = "\033[36m";
				levelName = "[DEBUG]";
				break;
			case Level::Info:
				colorCode = "\033[34m";
				levelName = "[INFO]";
				break;
			case Level::Success:
				colorCode = "\033[32m";
				levelName = "[SUCCESS]";
				break;
			case Level::Warning:
				colorCode = "\033[33m";
				levelName = "[WARNING]";
				break;
			case Level::Error:
				colorCode = "\033[31m";
				levelName = "[ERROR]";
				break;
			case Level::Offset:
				colorCode = "\033[38;5;151m";
				levelName = "[+]";
				break;
			}

			std::string timestamp = GetTimestamp();
			std::cout << colorCode << levelName << "\033[0m " << message << '\n';
			if (logFile.is_open()) {
				logFile << "[" << timestamp << "] " << levelName << " " << message << '\n';
				logFile.flush();
			}
		}
	};

	inline LoggerInstance instance;

	inline void Log(Level logLevel, const std::string& message) {
		instance.Log(logLevel, message);
	}
}

#endif
