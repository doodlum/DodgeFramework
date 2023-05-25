#include "Events.h"
#include "Settings.h"
#include "Utils.h"

namespace Events
{
	enum Direction : std::uint32_t
	{
		kNeutral = 0,
		kForward = 1,
		kRightForward = 2,
		kRight = 3,
		kRightBackward = 4,
		kBackward = 5,
		kLeftBackward = 6,
		kLeft = 7,
		kLeftForward = 8
	};

	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	auto InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource)
		-> EventResult
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;

		if (Settings::uDodgeKey == kInvalid) {
			return EventResult::kContinue;
		}

		if (!a_event) {
			return EventResult::kContinue;
		}

		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType != EventType::kButton) {
				continue;
			}

			auto button = static_cast<RE::ButtonEvent*>(event);
			if (!button->IsDown()) {
				continue;
			}

			auto key = button->idCode;
			switch (button->device.get()) {
			case DeviceType::kMouse:
				key += kMouseOffset;
				break;
			case DeviceType::kKeyboard:
				key += kKeyboardOffset;
				break;
			case DeviceType::kGamepad:
				key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
				break;
			default:
				continue;
			}

			if (key == Settings::uDodgeKey)
			{
				Dodge();
				break;
			}
		}

		return EventResult::kContinue;
	}

	std::string GetDodgeEvent()
	{
		auto normalizedInputDirection = Vec2Normalize(RE::PlayerControls::GetSingleton()->data.prevMoveVec);
		if (normalizedInputDirection.x == 0.f && normalizedInputDirection.y == 0.f) {
			return "TKDodgeBack";
		}
		RE::NiPoint2 forwardVector(0.f, 1.f);
		float dodgeAngle = GetAngle(normalizedInputDirection, forwardVector);
		if (dodgeAngle >= -2 * PI8 && dodgeAngle < 2 * PI8) {
			return "TKDodgeForward";
		}
		else if (dodgeAngle >= -6 * PI8 && dodgeAngle < -2 * PI8) {
			return "TKDodgeLeft";
		}
		else if (dodgeAngle >= 6 * PI8 || dodgeAngle < -6 * PI8) {
			return "TKDodgeBack";
		}
		else if (dodgeAngle >= 2 * PI8 && dodgeAngle < 6 * PI8) {
			return "TKDodgeRight";
		}

		return "TKDodgeBack";
	}

	void Dodge()
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerControls = RE::PlayerControls::GetSingleton();

		auto ui = RE::UI::GetSingleton();
		auto controlMap = RE::ControlMap::GetSingleton();

		if (ui->GameIsPaused() || !controlMap->IsMovementControlsEnabled() || !controlMap->IsLookingControlsEnabled() || ui->IsMenuOpen("Dialogue Menu")
			|| playerCharacter->AsActorState()->GetSitSleepState() != RE::SIT_SLEEP_STATE::kNormal || playerCharacter->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina) <= Settings::fStaminaCost) {
			return;
		}

		if (!playerCharacter || !playerControls)
		{
			return;
		}

		int iStep = Settings::bUseDefaultDodgeRoll? 0 : 2;

		if (Settings::bUsePerkRestrictions)
		{
			if (Settings::perkDodgeRoll && playerCharacter->HasPerk(Settings::perkDodgeRoll))
			{
				iStep = 0;
			}
			else if (Settings::perkStepDodge && playerCharacter->HasPerk(Settings::perkStepDodge))
			{
				iStep = 2;
			}
			else {
				return;
			}
		}

		playerCharacter->SetGraphVariableInt("iStep", iStep);

		if (Settings::fIFrameDuration > 0) {
			playerCharacter->SetGraphVariableFloat("TKDR_IframeDuration", Settings::fIFrameDuration);	// Set invulnerable frame duration
		}
		playerCharacter->NotifyAnimationGraph(GetDodgeEvent());					// Send TK Dodge Event
	}

	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}

	void SinkEventHandlers()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Added input event sink");
	}

	void ApplyDodgeCost()
	{
		if (auto playerCharacter = RE::PlayerCharacter::GetSingleton()) {
			playerCharacter->AsActorValueOwner()->RestoreActorValue(RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -Settings::fStaminaCost);
		}
	}

	RE::BSEventNotifyControl AnimationEventHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
	{
		std::string stringified = a_event->tag.c_str();

		if (stringified == "TKDR_DodgeStart") {
			ApplyDodgeCost();
		}

		return RE::BSEventNotifyControl::kContinue;
	}

	bool AnimationEventHandler::Register()
	{
		static AnimationEventHandler singleton;
		RE::PlayerCharacter::GetSingleton()->AddAnimationGraphEventSink(&singleton);
		logger::info("Registered {}", typeid(singleton).name());
		return true;
	}
}