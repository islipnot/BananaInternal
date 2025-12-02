#include "pch.hpp"
#include "cfg.hpp"
#include "sdk/interfaces.hpp"
#include "core/gui/gui.hpp"
#include "features.hpp"

namespace features::visuals
{
	//
	//// GLOBALS
	//

	static ImDrawList* drawlist = nullptr;

	static const sdk::ClientFields_t* player = nullptr;

	//
	//// HELPERS
	//

	static __forceinline void invert_height(glm::vec3& screen_point)
	{
		screen_point.y = gui::client_area.y - screen_point.y;
	}

	static bool is_off_screen(const glm::vec3& screen_point)
	{
		return screen_point.x > gui::client_area.x || screen_point.y > gui::client_area.y || screen_point.x < 0.0f || screen_point.y < 0.0f || screen_point.z < 0.0f;
	}

	static void get_transform_point(const sdk::transform_t* transform, glm::vec3* dst)
	{
		glm::vec3 world_pos = {};
		sdk::fn::transform_get_position(transform, &world_pos, nullptr);
		sdk::fn::world_to_screen_point(sdk::main::main_cam->cam, &world_pos, 2, dst);
	}

	//
	//// MAIN
	//

	static void draw_box(glm::vec3& torso_point)
	{
		glm::vec3 head_point = {};
		get_transform_point(player->head, &head_point);

		const float difference = head_point.y - torso_point.y;
		const float y_offset   = difference * 1.35f;

		invert_height(torso_point);

		if (cfg::visuals::boxes)
		{
			const float x_offset = difference * gui::box_scale;

			const ImVec2 p1 =
			{
				torso_point.x + x_offset,
				torso_point.y + y_offset
			};

			const ImVec2 p2 =
			{
				torso_point.x - x_offset,
				torso_point.y - y_offset
			};

			drawlist->AddRect(p1, p2, IM_COL32(0, 0, 0, 255),      0.0f, 0, 4.0f);
			drawlist->AddRect(p1, p2, IM_COL32(230, 11, 199, 255), 0.0f, 0, 2.0f);
		}

		if (cfg::visuals::health)
		{
			const ImVec2 p1 =
			{
				torso_point.x + (difference * gui::health_scale),
				torso_point.y + y_offset + 1.0f
			};

			const ImVec2 p2 =
			{
				p1.x + 4.0f,
				(torso_point.y - y_offset) - 1.0f
			};

			const float y_len = y_offset * 2.0f;

			drawlist->AddRectFilled(p1, { p2.x, p2.y + (y_len - (y_len * (static_cast<float>(player->health()) / static_cast<float>(player->max_health())))) }, IM_COL32(0, 255, 0, 255));
			drawlist->AddRect(p1, p2, IM_COL32(0, 0, 0, 255));
		}
	}

	static void draw_skeleton()
	{
		const auto bone_array = player->player_state->fields.bones->fields._items;

		sdk::transform_t* hip    = {}, * spine  = {}, * neck   = {}, * head   = {}, * shoulder_l = {}, * shoulder_r = {}, * arm_tl = {}, * arm_ll = {}, 
			            * arm_tr = {}, * arm_lr = {}, * leg_tl = {}, * leg_ll = {}, * leg_tr     = {}, * leg_lr     = {}, * foot_l = {}, * foot_r = {},
			            * chest  = {}, * hand_l = {}, * hand_r = {};

		for (int i = 0, sz = static_cast<int>(bone_array->max_length); i < sz; ++i)
		{
			const auto transform = bone_array->m_Items[i];
			const auto name = sdk::fn::get_object_name(transform, nullptr);
			
			const uint32_t bone_hash = hash_rt(reinterpret_cast<PCWSTR>(&name->fields._firstChar), name->fields._stringLength);

			switch (bone_hash)
			{
			case hash(L"Hip"):         { hip        = transform; break; }
			case hash(L"Chest"):       { chest      = transform; break; }
			case hash(L"Spine"):       { spine      = transform; break; }
			case hash(L"Neck"):        { neck       = transform; break; }
			case hash(L"Head"):        { head       = transform; break; }
			case hash(L"Shoulder.L"):  { shoulder_l = transform; break; }
			case hash(L"Shoulder.R"):  { shoulder_r = transform; break; }
			case hash(L"Upper Arm.L"): { arm_tl     = transform; break; }
			case hash(L"Lower Arm.L"): { arm_ll     = transform; break; }
			case hash(L"Upper Arm.R"): { arm_tr     = transform; break; }
			case hash(L"Lower Arm.R"): { arm_lr     = transform; break; }
			case hash(L"Upper Leg.L"): { leg_tl     = transform; break; }
			case hash(L"Lower Leg.L"): { leg_ll     = transform; break; }
			case hash(L"Upper Leg.R"): { leg_tr     = transform; break; }
			case hash(L"Lower Leg.R"): { leg_lr     = transform; break; }
			case hash(L"Foot.L"):      { foot_l     = transform; break; }
			case hash(L"Foot.R"):      { foot_r     = transform; break; }
			case hash(L"Hand.L"):      { hand_l     = transform; break; }
			case hash(L"Hand.R"):      { hand_r     = transform; break; }
			}
		}

		auto draw = [](const sdk::transform_t* start, const sdk::transform_t* end)
			{
				if (start && end)
				{
					glm::vec3 start_point, end_point;
					get_transform_point(start, &start_point);
					get_transform_point(end,   &end_point);

					invert_height(start_point);
					invert_height(end_point);

					drawlist->AddLine({ start_point.x, start_point.y }, { end_point.x, end_point.y }, IM_COL32(230, 11, 199, 255));
				}
			};

		draw(hip,        leg_tl);
		draw(hip,        leg_tr);
		draw(hip,        spine);
		draw(spine,      chest);
		draw(chest,      neck);
		draw(neck,       head);
		draw(neck,       shoulder_l);
		draw(neck,       shoulder_r);
		draw(shoulder_l, arm_tl);
		draw(arm_tl,     arm_ll);
		draw(arm_ll,     hand_l);
		draw(shoulder_r, arm_tr);
		draw(arm_tr,     arm_lr);
		draw(arm_lr,     hand_r);
		draw(leg_tl,     leg_ll);
		draw(leg_ll,     foot_l);
		draw(leg_lr,     foot_r);
		draw(leg_tr,     leg_lr);
	}

	static void draw_snapline()
	{
		glm::vec3 screen_point;

		if (features::aimbot::get_target(nullptr, &screen_point, nullptr) && !is_off_screen(screen_point))
		{
			invert_height(screen_point);

			drawlist->AddLine({ screen_point.x, screen_point.y }, { gui::screen_midpoint.x, gui::screen_midpoint.y }, IM_COL32(230, 11, 199, 255));
		}
	}

	static void enable_glow()
	{
		// The outline mode field of the actual outline object and the player state object
		// don't track eachother, meaning you can set this to invincible or team or whatever 
		// you want without it effecting the checks that rely on outline types.

		auto outline = player->player_state->fields.outline;
		sdk::fn::set_behavior_enabled(outline, true, nullptr);
		*reinterpret_cast<glm::vec4*>(&outline->fields.outlineColor.fields) = { 0.90196f, 0.04313f, 0.78039f, 1.0f }; // pink
		outline->fields.outlineMode = 2;
	}

	void run(bool render_esp)
	{
		const std::lock_guard guard(sdk::internal::player_mutex);

		drawlist = ImGui::GetBackgroundDrawList();

		sdk::player_list_t player_list;

		if (render_esp && sdk::main::player_data->get_player_list(&player_list))
		{
			for (int i = 0; i < player_list.count; ++i)
			{
				// Getting player

				player = player_list.entries[i].get_client_fields();
				if (!player || player->is_bad_targ() || (!cfg::visuals::show_team && player->is_teammate()))
				{
					continue;
				}

				glm::vec3 torso_point = {};
				get_transform_point(player->orientation, &torso_point);

				if (is_off_screen(torso_point))
				{
					continue;
				}

				if (cfg::visuals::boxes || cfg::visuals::health)
				{
					draw_box(torso_point);
				}

				if (cfg::visuals::skeleton)
				{							
					draw_skeleton();
				}

				if (cfg::visuals::glow)
				{
					enable_glow();
				}
			}
		}

		if (cfg::aimbot::show_fov)
		{
			drawlist->AddCircle({ gui::screen_midpoint.x, gui::screen_midpoint.y }, gui::fov_radius, IM_COL32(230, 11, 199, 255));
		}

		if (cfg::aimbot::snaplines && !(cfg::aimbot::enabled && !cfg::aimbot::silent_aim))
		{
			draw_snapline();
		}
	}
}