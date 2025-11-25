#pragma once

namespace settings
{
	inline bool        show_menu   = true;

	inline ImGuiIO*    imgui_io    = nullptr;

	inline ImGuiStyle* imgui_style = nullptr;
}

namespace cfg
{
	namespace visuals
	{
		inline bool skeleton  = true;

		inline bool boxes     = true;

		inline bool health    = true;

		inline bool glow      = false;

		inline bool show_team = false;
	}

	namespace aimbot
	{
		inline const char* targeting_modes[] =
		{
			"Closest",
			"Lowest health"
		};

		inline const char* bone_names[] =
		{
			"Head",
			"Chest",
		};

		enum targeting_modes_t : int
		{
			AIMBOT_CLOSEST,
			AIMBOT_LOWEST
		};

		enum bone_names_t : int
		{
			AIMBOT_HEAD,
			AIMBOT_CHEST,
		};

		inline bool  enabled    = false;

		inline bool  silent_aim = false;

		inline bool  ignore_fov = true;

		inline bool  vis_check  = false;

		inline float fov        = 35.0f;

		inline bool  show_fov   = false;

		inline bool  snaplines  = true;

		inline int   aim_mode   = AIMBOT_CLOSEST;

		inline int   aim_bone   = AIMBOT_HEAD;
	}

	namespace exploits
	{
		inline bool  magic_bullet = true; // hitboxes in this game are broken when players bend all the way down, headshots are impossible to hit in some cases

		inline bool  no_recoil    = true;
		
		inline bool  no_spread    = true;

		inline bool  noclip       = false; // so powerful its honestly boring, i suggest raging w default cfg

		inline bool  inf_jump     = true;

		inline float jump_mult    = 1.5f;
	}

	namespace misc
	{
		inline bool no_shake       = true;

		inline bool block_voice    = true;

		inline bool no_chat_filter = true;

		inline float aspect_ratio  = 0.0f;
	}
}