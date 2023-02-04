#pragma once

#include "il2cpp.h"

#include <mutex>
#include <vector>

namespace globals
{

	inline Multiplayer_Client_ClientPlayer_o* local_controller = nullptr;
	 
	inline std::vector<Multiplayer_Client_ClientPlayer_o*> players{};
	inline std::mutex player_sync{};
	inline UnityEngine_Camera_o* camera = nullptr;

}