#pragma warning(disable : 4996)

#include <API/Ark/Ark.h>
#include <ArkPermissions.h>
#include <fstream>
#include "json.hpp"
#include <iostream>
#include "Timer.h"
#include "Requests.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")

void PostLoad();

DECLARE_HOOK(AShooterPlayerController_ServerSendChatMessage_Implementation, void, AShooterPlayerController*, FString*, EChatSendMode::Type);
DECLARE_HOOK(AShooterGameMode_PostLogin, void, AShooterGameMode*, APlayerController*);

bool bIsInit = false;
int32 MessageCount = 0;
nlohmann::json config;

void  Hook_AShooterGameMode_PostLogin(AShooterGameMode* _this, APlayerController* NewPlayer)
{
	AShooterGameMode_PostLogin_original(_this, NewPlayer);
	if (!bIsInit)
	{
		bIsInit = true;
		PostLoad();
	}
}

void  Hook_AShooterPlayerController_ServerSendChatMessage_Implementation(AShooterPlayerController* _this, FString* ChatMessage, EChatSendMode::Type SendMode)
{
	if (SendMode == EChatSendMode::GlobalChat)
	{
		if (!ChatMessage->StartsWith("/"))
		{
			FString Map = "";
			if (config.value("CustomMapName", "") == "") ArkApi::GetApiUtils().GetWorld()->GetMapName(&Map);
			else Map = FString(config.value("CustomMapName", ""));
			//ArkApi::GetApiUtils().GetShooterGameMode()->GetServerName(&Map, false);

			int TribeId = _this->TargetingTeamField();
			FTribeData* tribe_data = static_cast<FTribeData*>(FMemory::Malloc(0x200));
			RtlSecureZeroMemory(tribe_data, 0x200);
			ArkApi::GetApiUtils().GetShooterGameMode()->LoadTribeData(TribeId, tribe_data, false, false);
			std::wstring TribeName(*tribe_data->TribeNameField());
			std::wstring Message(**ChatMessage);
			FString Name = "";
			if (_this && _this->GetPlayerCharacter()) Name = _this->GetPlayerCharacter()->PlayerNameField();
			std::wstring PlayerName(*Name);
			std::string URL = config.value("URL", "");
			std::wstring wideStr = ArkApi::Tools::Utf8Decode(config.value("MsgFormat", "No message"));
			FString Data = FString::Format(wideStr.c_str(), PlayerName, TribeName, Message);

			uint64 SteamID = ArkApi::GetApiUtils().GetSteamIdFromController(_this);
			if (Permissions::IsPlayerInGroup(SteamID, "Admins")) Data = "**[SERVER ADMIN]** " + Data;
			nlohmann::json j;
			j["username"] = Map.ToString();
			j["avatar_url"] = "https://cdn.discordapp.com/attachments/824603821132611604/1148194612855443487/IMG_4342.png";
			j["content"] = Data.ToString();
			API::Requests::Get().CreatePostRequest(URL, [](bool Sucess, std::string Result)
				{ /*Log::GetLog()->error(Result.c_str());*/ }, j.dump(), "application/json");
		}
	}
	AShooterPlayerController_ServerSendChatMessage_Implementation_original(_this, ChatMessage, SendMode);
}

void SetMessageCount()
{
	const std::string file_path = config.value("FilePath", "");
	//const std::string file_path = "P:/DiscordMessages.txt";
	//const std::string file_path = "E:/asmdata/Servers/Server1/ShooterGame/Binaries/Win64/ArkApi/Plugins/RioChat/file.txt";
	std::ifstream file{ file_path };
	if (!file.is_open())
		throw std::runtime_error("Can't open file");
	std::string Text;
	TArray<FString> parsed;
	while (std::getline(file, Text))
	{
		parsed.Add(FString(Text));
	}
	file.close();
	MessageCount = parsed.Num();
}

void SendChatMessageToAll(FString SenderName, FString Message)
{
	FChatMessage chat_message = FChatMessage();
	chat_message.SenderId = 0;
	chat_message.SenderName = SenderName;
	chat_message.SenderSteamName = "";
	chat_message.SenderTribeName = "Discord";
	chat_message.Message = Message;
	chat_message.SenderIcon = nullptr;
	auto world = ArkApi::GetApiUtils().GetWorld();
	if (!world) return;

	auto playerControllers = world->PlayerControllerListField();
	for (auto playerController : playerControllers)
	{
		AShooterPlayerController* aShooterPC = static_cast<AShooterPlayerController*>(playerController.Get());
		aShooterPC->ClientChatMessage(chat_message);
	}
}

void CheckNewMessage(float DeltaSeconds)
{
	if (bIsInit && MessageCount != 0)
	{
		const std::string file_path = config.value("FilePath", "");
		//const std::string file_path = "E:/asmdata/Servers/Server1/ShooterGame/Binaries/Win64/ArkApi/Plugins/RioChat/file.txt";
		std::ifstream file{ file_path };
		if (!file.is_open())
			throw std::runtime_error("Can't open file");
		std::string Text;
		TArray<FString> parsed;
		while (std::getline(file, Text))
		{
			std::wstring wText = ArkApi::Tools::Utf8Decode(Text);
			parsed.Add(FString(wText));
		}
		file.close();
		if (parsed.Num() > MessageCount)
		{
			MessageCount = parsed.Num();
			//std::wstring Message(*parsed.Last());
			TArray<FString> MessageStruct;
			parsed.Last().ParseIntoArray(MessageStruct, L" ||||| ", false);
			SendChatMessageToAll(MessageStruct[0], MessageStruct[1]);
		}
		else MessageCount = parsed.Num();
	}
}

void ReadConfig()
{
	const std::string config_path = ArkApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/RioChat/config.json";
	std::ifstream file{ config_path };
	if (!file.is_open())
		throw std::runtime_error("Can't open config.json");

	file >> config;

	file.close();
}

void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/)
{
	FString reply;

	try
	{
		ReadConfig();

	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error(error.what());

		reply = error.what();
		rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
		return;
	}

	reply = "Reloaded config";
	rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

void PostLoad()
{
	try
	{
		ReadConfig();
	}
	catch (const std::exception& error)
	{
		Log::GetLog()->error(error.what());
		throw;
	}
	
	SetMessageCount();
	ArkApi::GetCommands().AddRconCommand("RioChat.Reload", &ReloadConfigRcon);
	ArkApi::GetCommands().AddOnTickCallback("Check", &CheckNewMessage);
	ArkApi::GetHooks().SetHook("AShooterPlayerController.ServerSendChatMessage_Implementation", &Hook_AShooterPlayerController_ServerSendChatMessage_Implementation, &AShooterPlayerController_ServerSendChatMessage_Implementation_original);
	Log::GetLog()->info("Finish PostLoad RioChat plugin!");
}

void Load()
{
	Log::Get().Init("RioChat");
	ArkApi::GetHooks().SetHook("AShooterGameMode.PostLogin", &Hook_AShooterGameMode_PostLogin, &AShooterGameMode_PostLogin_original);
	if (ArkApi::GetApiUtils().GetShooterGameMode())
	{
		PostLoad();
		bIsInit = true;
	}
}

void Unload()
{
	ArkApi::GetCommands().RemoveRconCommand("RioChat.Reload");
	ArkApi::GetCommands().RemoveOnTickCallback("Check");
	ArkApi::GetHooks().DisableHook("AShooterGameMode.PostLogin", &Hook_AShooterGameMode_PostLogin);
	ArkApi::GetHooks().DisableHook("AShooterPlayerController.ServerSendChatMessage_Implementation", &Hook_AShooterPlayerController_ServerSendChatMessage_Implementation);
}

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Load();
		break;
	case DLL_PROCESS_DETACH:
		Unload();
		break;
	}
	return TRUE;
}