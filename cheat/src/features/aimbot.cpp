#include "pch.hpp"
#include "cfg.hpp"
#include "core/gui/gui.hpp"
#include "sdk/interfaces.hpp"
#include "features.hpp"

enum bone_types_t : int
{
	bone_jaw,
	bone_head,
	bone_neck,
	bone_shoulder_l,
	bone_shoulder_r,
	bone_upper_arm_l,
	bone_upper_arm_r,
	bone_lower_arm_l,
	bone_lower_arm_r,
	bone_chest,
	bone_spine,
	bone_hip,
	bone_upper_leg_l,
	bone_upper_leg_r,
	bone_lower_leg_l,
	bone_lower_leg_r,
	bone_foot_l,
	bone_foot_r,
};

struct bone_field_t
{
	UINT head        : 1;
	UINT neck        : 1;
	UINT shoulder_l  : 1;
	UINT shoulder_r  : 1;
	UINT chest       : 1;
};

struct side_info_t
{
	float x_dist; // X-Axis distance
	float z_dist; // Z-Axis distance
	float y_dist; // Y-Axis distance
	float h_dist; // distance on horizontal plane ( sqrt(x_dist^2 + z_dist^2) )
};

namespace features::aimbot
{
	//
	//// MATH
	//

	static void get_angle_info(const glm::vec3& origin, const glm::vec3& target, const side_info_t& sides, angle_info_t* buffer)
	{
		constexpr float rad_mult = static_cast<float>(180.0 / M_PI);

		float yaw = -((atan2f(sides.z_dist, sides.x_dist) * rad_mult) - 90.0f);
		if (yaw < 0.0f) yaw += 360.0f;

		buffer->pitch = -(atan2f(sides.y_dist, sides.h_dist) * rad_mult);
		buffer->yaw   = yaw;
	}

	static void get_side_lengths(const glm::vec3& origin, const glm::vec3& target, side_info_t* buffer)
	{
		const float x_dist = target.x - origin.x;
		const float z_dist = target.z - origin.z;

		buffer->h_dist = sqrtf((x_dist * x_dist) + (z_dist * z_dist));
		buffer->x_dist = x_dist;
		buffer->z_dist = z_dist;
		buffer->y_dist = target.y - origin.y;
	}

	//
	//// HELPERS
	//

	static bool check_fov(const glm::vec3& target_pos, glm::vec3* point_buffer)
	{
		if (!cfg::aimbot::ignore_fov || point_buffer)
		{
			// Getting screen point

			glm::vec3 screen_point = {};
			sdk::fn::world_to_screen_point(sdk::main::main_cam->cam, &target_pos, 2, &screen_point);

			// Checking if player is within aimbot FOV

			if (!cfg::aimbot::ignore_fov)
			{
				if (screen_point.x > gui::client_area.x || screen_point.y > gui::client_area.y || screen_point.x < 0.0f || screen_point.y < 0.0f || screen_point.z < 0.0f)
				{
					return false;
				}

				const float distance = sqrtf(powf(gui::screen_midpoint.x - screen_point.x, 2.0f) + powf(gui::screen_midpoint.y - screen_point.y, 2.0f));

				if (distance > gui::fov_distance)
				{
					return false;
				}
			}

			// Storing screen point

			if (point_buffer)
			{
				*point_buffer = screen_point;
			}
		}

		return true;
	}

	static bool check_health(const sdk::ClientFields_t* player, int& lowest_hp)
	{
		if (cfg::aimbot::aim_mode == cfg::aimbot::AIMBOT_LOWEST)
		{
			const int health = player->health();

			if (!lowest_hp)
			{
				lowest_hp = health;
			}
			else if (health > lowest_hp)
			{
				return false;
			}
		}

		return true;
	}

	static bool check_distance(float distance, float& closest)
	{
		if (cfg::aimbot::aim_mode == cfg::aimbot::AIMBOT_CLOSEST)
		{
			if (distance < closest || !closest)
			{
				closest = distance;
			}
			else
			{
				return false;
			}
		}

		return true;
	}

	static bool get_target_bone(const sdk::ClientFields_t* player, glm::vec3* pos_buffer, sdk::transform_t** transform_buffer)
	{
		auto get_bone = [player, pos_buffer, transform_buffer](uint32_t bone_hash)
			{
				return player->get_bone(bone_hash, pos_buffer, transform_buffer);
			};

		bone_types_t current_bone;
		bone_field_t checked = {};

		// Locating preferred bone

		if (cfg::aimbot::aim_bone == cfg::aimbot::AIMBOT_HEAD)
		{
			if (!get_bone(hash(L"Jaw")))
				return false;

			current_bone = bone_jaw;
		}
		else
		{
			if (!get_bone(hash(L"Chest")))
				return false;

			current_bone = bone_chest;
		}

		// Performing visibility checks

		if (cfg::aimbot::vis_check)
		{
			glm::vec3 cam_pos;
			sdk::main::main_cam->get_pos(&cam_pos);

			const auto target_rb = player->player_state->fields.rb;
			
			while (true)
			{
				const glm::vec3 new_pos = *pos_buffer - cam_pos;

				glm::vec3 cam_forward;
				sdk::fn::get_normalized_vec3(&cam_forward, &new_pos, nullptr);

				sdk::raycast_hit_t hit_info;
				if (sdk::fn::physics_raycast(&cam_pos, &cam_forward, &hit_info, 1000.0f, sdk::main::prefab_mgr->whatIsHittable.fields.m_Mask, 1, nullptr))
				{
					if (sdk::fn::get_raycast_rb(&hit_info, nullptr) == target_rb)
					{
						return true;
					}
				}
				else
				{
					return false;
				}

				switch (current_bone)
				{
				case bone_jaw:
				{
					if (!checked.head)
					{
						if (!get_bone(hash(L"Head")))
							return false;

						current_bone = bone_head;
					}
					else if (!checked.shoulder_l)
					{
						if (!get_bone(hash(L"Shoulder.L")))
							return false;

						current_bone = bone_shoulder_l;
					}
					else
					{
						if (!get_bone(hash(L"Shoulder.R")))
							return false;

						current_bone = bone_shoulder_r;
					}
					
					break;
				}
				case bone_head:
				{
					if (!checked.neck)
					{
						if (!get_bone(hash(L"Neck")))
							return false;

						current_bone = bone_neck;
					}
					else
					{
						if (!get_bone(hash(L"Jaw")))
							return false;

						current_bone = bone_jaw;
					}

					checked.head = true;
					break;
				}
				case bone_neck:
				{
					if (!checked.chest)
					{
						if (!get_bone(hash(L"Chest")))
							return false;

						current_bone = bone_chest;
					}
					else
					{
						if (!get_bone(hash(L"Head")))
							return false;

						current_bone = bone_head;
					}

					checked.neck = true;
					break;
				}
				case bone_shoulder_l:
				{
					if (!get_bone(hash(L"Upper Arm.L")))
						return false;

					current_bone = bone_upper_arm_l;
					checked.shoulder_l = true;
					break;
				}
				case bone_shoulder_r:
				{
					if (!get_bone(hash(L"Upper Arm.R")))
						return false;

					current_bone = bone_upper_arm_r;
					checked.shoulder_r = true;
					break;
				}
				case bone_upper_arm_l:
				{
					if (!get_bone(hash(L"Lower Arm.L")))
						return false;

					current_bone = bone_lower_arm_l;
					break;
				}
				case bone_upper_arm_r:
				{
					if (!get_bone(hash(L"Lower Arm.R")))
						return false;

					current_bone = bone_lower_arm_r;
					break;
				}
				case bone_lower_arm_l:
				{
					if (!checked.shoulder_r)
					{
						if (!get_bone(hash(L"Shoulder.R")))
							return false;

						current_bone = bone_shoulder_r;
					}
					else
					{
						if (!get_bone(hash(L"Hip")))
							return false;

						current_bone = bone_hip;
					}

					break;
				}
				case bone_lower_arm_r:
				{
					if (!get_bone(hash(L"Hip")))
						return false;

					current_bone = bone_hip;
					break;
				}
				case bone_chest:
				{
					// Check order: neck, spine

					if (!checked.neck)
					{
						if (!get_bone(hash(L"Neck")))
							return false;

						current_bone = bone_neck;
					}
					else
					{
						if (!get_bone(hash(L"Spine")))
							return false;

						current_bone = bone_spine;
					}

					checked.chest = true;
					break;
				}
				case bone_spine:
				{
					if (!get_bone(hash(L"Hip")))
						return false;

					current_bone  = bone_hip;
					break;
				}
				case bone_hip:
				{
					if (!get_bone(hash(L"Upper Leg.L")))
						return false;

					current_bone = bone_upper_leg_l;
					break;
				}
				case bone_upper_leg_l:
				{
					if (!get_bone(hash(L"Lower Leg.L")))
						return false;

					current_bone = bone_lower_leg_l;
					break;
				}
				case bone_upper_leg_r:
				{
					if (!get_bone(hash(L"Lower Leg.R")))
						return false;

					current_bone = bone_lower_leg_r;
					break;
				}
				case bone_lower_leg_l:
				{
					if (!get_bone(hash(L"Foot.L")))
						return false;

					current_bone = bone_foot_l;
					break;
				}
				case bone_lower_leg_r:
				{
					if (!get_bone(hash(L"Foot.R")))
						return false;

					current_bone = bone_foot_r;
					break;
				}
				case bone_foot_l:
				{
					if (!get_bone(hash(L"Upper Leg.R")))
						return false;

					current_bone = bone_upper_leg_r;
					break;
				}

				default: return false;
				}
			}

			return false;
		}

		return true;
	}

	//
	//// MAIN
	//

	sdk::ClientFields_t* get_target(angle_info_t* angle_buffer, glm::vec3* point_buffer, sdk::transform_t** transform_buffer)
	{
		sdk::player_list_t player_list;
		if (!sdk::main::player_data->get_player_list(&player_list))
		{
			return nullptr;
		}

		glm::vec3 local_pos;
		sdk::main::main_cam->get_pos(&local_pos);

		sdk::ClientFields_t* final_target = nullptr;

		float closest = 0.0f;
		int lowest_hp = 0;

		for (int i = 0; i < player_list.count; ++i)
		{
			const auto player = player_list.entries[i].get_client_fields();
			if (!player || player->is_bad_targ() || player->is_invincible() || player->is_teammate())
			{
				continue;
			}

			glm::vec3 target_pos;

			if (!get_target_bone(player, &target_pos, transform_buffer))
				continue;

			// Checking FOV/health

			int temp_hp = lowest_hp;

			if (!check_fov(target_pos, point_buffer) || !check_health(player, temp_hp))
				continue;

			// Calculating side lengths and checking distance

			side_info_t sides;
			get_side_lengths(local_pos, target_pos, &sides);

			const float distance = sqrtf((sides.h_dist * sides.h_dist) + (sides.y_dist * sides.y_dist));
			float temp_dst = closest;

			if (!check_distance(distance, temp_dst))
				continue;

			if (angle_buffer)
			{
				get_angle_info(local_pos, target_pos, sides, angle_buffer);
			}

			lowest_hp    = temp_hp;
			closest      = temp_dst;
			final_target = player;
		}

		return final_target;
	}

	void run()
	{
		const std::lock_guard guard(sdk::internal::player_mutex);

		angle_info_t angles;
		sdk::transform_t* transform;
		
		if (get_target(&angles, nullptr, &transform))
		{
			sdk::main::player_movement->set_view_angles(angles);
			gui::aim_targ = transform;
		}
	}
}