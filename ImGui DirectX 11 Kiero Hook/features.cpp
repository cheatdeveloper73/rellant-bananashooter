#include "features.h"

features::aimbot_target_t features::get_aimbot_target()
{

	auto calc_fov = [&](ImVec2 pos)
	{
		static auto screen_center = ImVec2(ImGui::GetIO().DisplaySize.x / 2.f, ImGui::GetIO().DisplaySize.y / 2.f);
		return pos.distance(screen_center);
	};

	std::vector<features::aimbot_target_t> targets{};

	globals::player_sync.lock();

	for (const auto& player : globals::players)
	{

		switch (config::aimbot_hitbox)
		{

		case 0: // head
		{
			ImVec2 head_screen{};
			il2cpp::Vector3 head_pos = il2cpp::bone_position_at_index(player->fields.bones, il2cpp::HEAD);

			if (!il2cpp::world_to_screen(head_pos, head_screen))
				continue;

			float head_fov = calc_fov(head_screen);

			if (head_fov > config::aimbot_fov)
				continue;

			targets.emplace_back<aimbot_target_t>({player, head_fov, head_pos});

			break;
		}
		case 1: // chest
		{
			ImVec2 body_screen{};
			il2cpp::Vector3 body_pos = il2cpp::bone_position_at_index(player->fields.bones, il2cpp::PELVIS);

			if (!il2cpp::world_to_screen(body_pos, body_screen))
				continue;

			float body_fov = calc_fov(body_screen);

			if (!body_fov > config::aimbot_fov)
				continue;

			targets.emplace_back<aimbot_target_t>({ player, body_fov, body_pos });

			break;
		}
		case 2: // nearest
		{
			ImVec2 head_screen{};
			il2cpp::Vector3 head_pos = il2cpp::bone_position_at_index(player->fields.bones, il2cpp::HEAD);

			if (!il2cpp::world_to_screen(head_pos, head_screen))
				continue;

			ImVec2 body_screen{};
			il2cpp::Vector3 body_pos = il2cpp::bone_position_at_index(player->fields.bones, il2cpp::PELVIS);

			if (!il2cpp::world_to_screen(body_pos, body_screen))
				continue;

			float head_fov = calc_fov(head_screen);
			float body_fov = calc_fov(body_screen);

			il2cpp::Vector3 final_pos{};
			float final_fov = 0.f;

			if (head_fov > config::aimbot_fov || body_fov > config::aimbot_fov)
				continue;

			if (head_fov > body_fov)
			{
				final_pos = body_pos;
				final_fov = body_fov;
			}
			else
			{
				final_pos = head_pos;
				final_fov = head_fov;
			}

			targets.emplace_back<aimbot_target_t>({ player, final_fov, final_pos });

			break;
		}

		}

	}

	globals::player_sync.unlock();

	if (targets.size() > 0)
	{

		auto compare_target = [&](aimbot_target_t t1, aimbot_target_t t2)
		{
			return t1.fov < t2.fov;
		};

		std::sort(targets.begin(), targets.end(), compare_target);

		auto target = targets[0];

		return target;


	}
	else
		return aimbot_target_t{ nullptr, 0.f, il2cpp::Vector3{} };

}