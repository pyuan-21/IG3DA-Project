#pragma once
#include <string>

namespace IceRender
{
	class Console
	{
	public:
		std::string GetCommandFromConsole() const;
		std::string GetPreviousCommand() const;

		void HandleCommand(const std::string& _command);
		void PrintHelp();

	private:
		std::string preCommand; // previous command
	};
}
