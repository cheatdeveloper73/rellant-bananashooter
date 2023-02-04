#pragma once

#include "config.h"
#include <cstdint>
#include "il2cpp/il2cpp.h"
#include "il2cpp.h"
#include "globals.h"
#include <algorithm>

namespace features
{

	struct aimbot_target_t
	{
		Multiplayer_Client_ClientPlayer_o* player;
		float fov;
		il2cpp::Vector3 position;
	};

	aimbot_target_t get_aimbot_target();

}