#pragma once

namespace Events
{
	class AnimationEventHandler : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
	{
	public:
		virtual RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource);
		static bool                      Register();
	};

	class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		using EventResult = RE::BSEventNotifyControl;

		static InputEventHandler* GetSingleton();

		virtual EventResult ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

	private:
		enum : uint32_t
		{
			kInvalid = static_cast<uint32_t>(-1),
			kKeyboardOffset = 0,
			kMouseOffset = 256,
			kGamepadOffset = 266
		};

		InputEventHandler() = default;
		InputEventHandler(const InputEventHandler&) = delete;
		InputEventHandler(InputEventHandler&&) = delete;
		virtual ~InputEventHandler() = default;

		InputEventHandler& operator=(const InputEventHandler&) = delete;
		InputEventHandler& operator=(InputEventHandler&&) = delete;

		std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);
	};

	void SinkEventHandlers();

	void Dodge();
}
