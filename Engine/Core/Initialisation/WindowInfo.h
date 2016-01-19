#pragma once
#include <string>

namespace Core
{
	namespace Initialisation
	{
		struct WindowInfo
		{
			std::string name;
			int width, height;
			int position_x, position_y;
			bool isResizable;
			bool useGameMode;
			std::string gameModeString;

			WindowInfo()
			{
				name = "Engine";
				width = 800; height = 600;
				position_x = 300;
				position_y = 300;
				isResizable = true;
				useGameMode = true;
				gameModeString = "1920x1200:32@60";
			}

			WindowInfo(std::string name,
				int start_position_x,
				int start_position_y,
				int width, int height,
				bool is_reshapable,
				bool isUseGameMode,
				std::string gameModeString)
			{

				this->name = name;
				this->position_x = start_position_x;
				this->position_y = start_position_y;

				this->width = width;
				this->height = height;
				this->isResizable = is_reshapable;
				this->useGameMode = isUseGameMode;
				this->gameModeString = gameModeString;
			}

			//copy constructor
			WindowInfo(const WindowInfo& windowInfo)
			{
				Copy(windowInfo);
			}

			void operator=(const WindowInfo& windowInfo)
			{
				Copy(windowInfo);
			}

			void Copy(const WindowInfo& windowInfo)
			{
				name = windowInfo.name;
				position_x = windowInfo.position_x;
				position_y = windowInfo.position_y;

				width = windowInfo.width;
				height = windowInfo.height;
				isResizable = windowInfo.isResizable;
				useGameMode = windowInfo.useGameMode;
				gameModeString = windowInfo.gameModeString;
			}
		};
	}
}