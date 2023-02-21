#include "Events.h"
#include "Hooks.h"
#include "Papyrus.h"
#include "Settings.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::SinkEventHandlers();
		Settings::ReadSettings();
		break;
	}
}

void Load()
{
	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", MessageHandler);
	Hooks::Install();
	Papyrus::Register();
}