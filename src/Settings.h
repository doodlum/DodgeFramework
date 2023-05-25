#pragma once

struct Settings
{
	static void ReadSettings();

	// Controls
	static inline bool bUseSprintButton = true;
	static inline float fSprintHoldDuration = 0.25f;
	static inline std::uint32_t uDodgeKey = static_cast<std::uint32_t>(-1);

	// Gameplay
	static inline bool bUsePerkRestrictions = true;
	static inline bool bUseDefaultDodgeRoll = false;
	static inline float fStaminaCost = 10.0f;
	static inline float fIFrameDuration = 0.3f;

	static inline std::string sStepDodgePerk = "";
	static inline RE::BGSPerk* perkStepDodge = nullptr;

	static inline std::string sDodgeRollPerk = "";
	static inline RE::BGSPerk* perkDodgeRoll = nullptr;
};
