#include "hooks.h"

#include "../globals.h"
#include "../features.h"

bool hooks::hook_function(void* function_pointer, void* callback_function)
{

	if (!function_pointer || !callback_function)
		return false;

	hooked_function fn;
	fn.callback_function = callback_function;
	fn.target_address = function_pointer;

	MH_CreateHook((uintptr_t*)function_pointer, callback_function, (LPVOID*)&fn.original_function);

	hooks.push_back(fn);

	return MH_EnableHook(function_pointer) == MH_OK;

}

void __cdecl hk_client_player_update(Multiplayer_Client_ClientPlayer_o* thisptr)
{

	if (!thisptr->fields._Dead_k__BackingField && !thisptr->fields._IsLocal_k__BackingField)
	{

		bool in_list = false;

		for (const auto& plr : globals::players)
			if (plr == thisptr)
				in_list = true;

		if (!in_list)
		{

			globals::player_sync.lock();
			globals::players.push_back(thisptr);
			globals::player_sync.unlock();

		}

	}
	else if (thisptr->fields._IsLocal_k__BackingField)
		globals::local_controller = thisptr;

	return reinterpret_cast<decltype(&hk_client_player_update)>(hooks::hooks[0].original_function)(thisptr);

}

void __cdecl hk_firearms_update(Firearms_o* thisptr)
{

	globals::camera = thisptr->fields.PlayerCam;

	if (config::enable_infinite_ammo)
	{
		thisptr->fields.currentAmmo->fields.offset = 0;
		thisptr->fields.currentAmmo->fields.value = 30;
	}

	if (config::enable_no_bob)
		thisptr->fields.bobSpeed = 0.f;

	if (config::enable_no_bullet_drop)
		thisptr->fields.useGravity = false;

	if (config::enable_no_spread)
	{
		thisptr->fields.spreadAngle = 0.f;
		thisptr->fields.runSpread = 0.f;
		thisptr->fields.normalSpread = 0.f;
	}

	if (config::enable_no_recoil)
	{
		thisptr->fields.recoilX = 0.f;
		thisptr->fields.recoilY = 0.f;
		thisptr->fields.recoilZ = 0.f;
	}

	if (config::enable_rapid_fire)
		thisptr->fields.fireRate = 30.f;

	if (config::enable_all_guns_use_ballistics && !thisptr->fields.createBullet)
		thisptr->fields.createBullet = true;

	if (config::enable_instant_kill)
		thisptr->fields.damage = 130;

	if (config::enable_instant_reload)
		thisptr->fields.normalReloadTime = 0.f;

	return reinterpret_cast<decltype(&hk_firearms_update)>(hooks::hooks[1].original_function)(thisptr);

}

void __cdecl hk_weapon_manager_update(WeaponManager_o* thisptr)
{

	if (config::enable_infinite_tacticals)
	{
		thisptr->fields.TacticalThrowCount->fields.offset = 0;
		thisptr->fields.TacticalThrowCount->fields.value = 30;
	}

	return reinterpret_cast<decltype(&hk_weapon_manager_update)>(hooks::hooks[2].original_function)(thisptr);

}

void __cdecl hk_bullet_initialize(Bullet_o* thisptr, il2cpp::Vector3 dir, float bulletSpeed, int damage, UnityEngine_LayerMask_o* layerMask, bool local, bool useGravity)
{

	il2cpp::Vector3 real_direction = dir;

	if (config::enable_infinite_penetration && local)
		thisptr->fields.penetrationAmount = 9999.f;

	return reinterpret_cast<decltype(&hk_bullet_initialize)>(hooks::hooks[3].original_function)(thisptr, real_direction, config::enable_instant_bullet ? 9999.f : bulletSpeed, config::enable_instant_kill ? 101 : damage, layerMask, local, config::enable_no_bullet_drop ? false : useGravity);

}

void __cdecl hk_firearms_create_bullet(Firearms_o* thisptr, il2cpp::Vector3 position)
{

	//auto target = features::get_aimbot_target();
	//if (target.player)
	//	return reinterpret_cast<decltype(&hk_firearms_create_bullet)>(hooks::hooks[4].original_function)(thisptr, il2cpp::bone_position_at_index(globals::local_controller->fields.bones, il2cpp::HEAD).direction(target.position));

	//return reinterpret_cast<decltype(&hk_firearms_create_bullet)>(hooks::hooks[4].original_function)(thisptr, position);
	exit(0);

}

void hooks::init()
{

	if (!hook_function(il2cpp::get_method("Multiplayer.Client", "ClientPlayer", "Update")->methodPointer, hk_client_player_update))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "Firearms", "Update")->methodPointer, hk_firearms_update))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "WeaponManager", "Update")->methodPointer, hk_weapon_manager_update))
		exit(0);

	if (!hook_function(il2cpp::get_method("", "Bullet", "Initialization")->virtualMethodPointer, hk_bullet_initialize))
		exit(0);

	if (!hook_function((void*)(il2cpp::gameassembly + 0x3ADF90), hk_firearms_create_bullet))
		exit(0);

	std::cout << "hooked functions\n";

}