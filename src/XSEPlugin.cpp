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
		if (!GetModuleHandle(L"MaxsuIFrame.dll"))
		{
			if (REL::Module::IsAE()) {
				RE::DebugMessageBox("IFrame Generator RE must be installed for DMCO\n Install the \"AE Support\" version");
			}
			else {
				RE::DebugMessageBox("IFrame Generator RE must be installed for DMCO");
			}
		}
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