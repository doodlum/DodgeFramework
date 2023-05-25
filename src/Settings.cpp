#include "Settings.h"

#include <SimpleIni.h>

void Settings::ReadSettings()
{
	constexpr auto path = L"Data/MCM/Settings/DodgeFramework.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	bUseSprintButton = ini.GetBoolValue("Controls", "bUseSprintButton", true);
	fSprintHoldDuration = (float)ini.GetDoubleValue("Controls", "fSprintHoldDuration", 0.25f);
	uDodgeKey = static_cast<uint32_t>(ini.GetLongValue("Controls", "uDodgeKey"));

	bUseSprintButton = ini.GetBoolValue("Controls", "bUseSprintButton", true);

	bUsePerkRestrictions = ini.GetBoolValue("Gameplay", "bUsePerkRestrictions", true);
	bUseDefaultDodgeRoll = ini.GetBoolValue("Gameplay", "bUseDefaultDodgeRoll", false);

	fStaminaCost = (float)ini.GetDoubleValue("Gameplay", "fStaminaCost", 10.0f);
	fIFrameDuration = (float)ini.GetDoubleValue("Gameplay", "fIFrameDuration", 0.3f);

	sStepDodgePerk = ini.GetValue("Gameplay", "sStepDodgePerk", "");

	auto  dataHandler = RE::TESDataHandler::GetSingleton();
	auto& perkArray = dataHandler->GetFormArray<RE::BGSPerk>();

	perkStepDodge = nullptr;

	for (auto& perk : perkArray)
	{
		if (perk->GetName() == sStepDodgePerk)
		{
			perkStepDodge = perk;
			break;
		}
	}

	sDodgeRollPerk = ini.GetValue("Gameplay", "sDodgeRollPerk", "");

	perkDodgeRoll = nullptr;

	for (auto& perk : perkArray)
	{
		if (perk->GetName() == sDodgeRollPerk)
		{
			perkDodgeRoll = perk;
			break;
		}
	}
}