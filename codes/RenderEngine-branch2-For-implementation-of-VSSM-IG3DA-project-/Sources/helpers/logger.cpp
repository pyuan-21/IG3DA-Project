#include "logger.hpp"

using namespace IceRender;

void Logger::PrintMessage(const void* _p, const char* _fmt, ...)
{
	// _fmt is to define the data-type of args
	std::string str;
	va_list args;
	va_start(args, _fmt);

	while (*_fmt != '\0') {
		if (*_fmt == 'd') {
			int i = va_arg(args, int);
			str.append(std::to_string(i));
		}
		else if (*_fmt == 'c') {
			// note automatic conversion to integral type
			int c = va_arg(args, int);
			str.push_back(c);
		}
		else if (*_fmt == 'f') {
			double d = va_arg(args, double);
			str.append(std::to_string(d));
		}
		else if (*_fmt == 's') {
			std::string s = va_arg(args, char*);
			str.append(s);
		}
		else if (*_fmt == 'b') {
			bool b = va_arg(args, bool);
			str.append(b ? "true" : "false");
		}
		++_fmt;
	}
	va_end(args);

	PrintMessage(str);
}

void Logger::PrintMessage(const std::string& _message)
{
	// TODO: add a deubg/log type to differentiate message
	// By using #define to enable/disable debug log
	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	std::string timeStr = std::string(asctime(timeInfo));
	timeStr = timeStr.substr(0, timeStr.size() - 1);
	std::cout << "[IceRender][" + timeStr + "] " + _message << std::endl;
};