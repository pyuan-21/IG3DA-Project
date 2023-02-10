#pragma once

#include <string>
#include <iostream>
#include <time.h>
#include <cstdarg>

namespace IceRender
{
	class Logger
	{
	public:
		/// <summary>
		/// print message from mupltiple parameters
		/// </summary>
		/// <param name="_fmt">input data-type format. 'd' for int-type, 'c' for char-type, 'f' for float/double-type, 's' for string-type</param>
		/// <param name="_p">no usage inside this function, just need one more parameters to differ PrintMessage()</param>
		/// <param name="">uncertain number parameters</param>
		void PrintMessage(const void* _p, const char* _fmt, ...);
		void PrintMessage(const std::string& _message);
	};
}

#ifndef ICE_PRINT
#define ICE_PRINT
#define Print GLOBAL.logger->PrintMessage //using short-name for Print()
#endif // !ICE_PRINT