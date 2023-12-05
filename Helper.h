#include <API/Ark/Ark.h>
#include <ArkPermissions.h>

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")

FString GetBlueprintShort(UObjectBase* object)
{
	if (object != nullptr && object->ClassField() != nullptr)
	{
		FString path_name;
		object->ClassField()->GetDefaultObject(true)->GetFullName(&path_name, nullptr);

		if (int find_index = 0; path_name.FindChar(' ', find_index))
		{
			path_name = path_name.Mid(find_index + 1,
				path_name.Len() - (find_index + (path_name.EndsWith(
					"_C", ESearchCase::
					CaseSensitive)
					? 3
					: 1)));
			path_name = path_name.Replace(L"Default__", L"", ESearchCase::CaseSensitive);
			return path_name;
		}
	}

	return FString("");
}

void GetCryo(APrimalDinoCharacter* dino, AShooterPlayerController* player, UClass* Class)
{

	FCustomItemData customItemData;
	FARKDinoData DinoData;
	dino->GetDinoData(&DinoData);

		//
	// Custom Data Name
	//
	customItemData.CustomDataName = FName("Dino", EFindName::FNAME_Add);

	//
	// Custom Data Floats
	//
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Health]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Stamina]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Torpidity]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Oxygen]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Food]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Water]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Temperature]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Weight]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::MeleeDamageMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::SpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::TemperatureFortitude]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::CraftingSpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Health]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Stamina]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Torpidity]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Oxygen]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Food]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Water]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Temperature]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Weight]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::MeleeDamageMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::SpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::TemperatureFortitude]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::CraftingSpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->bIsFemale()());

	//
	// Custom Data Doubles
	//
	auto t1 = ArkApi::GetApiUtils().GetShooterGameMode()->GetWorld()->TimeSecondsField();
	customItemData.CustomDataDoubles.Doubles.Add(t1);
	customItemData.CustomDataDoubles.Doubles.Add(dino->BabyNextCuddleTimeField() - t1);
	customItemData.CustomDataDoubles.Doubles.Add(dino->NextAllowedMatingTimeField());

	const float d1 = static_cast<float>(dino->RandomMutationsMaleField());
	const double d2 = static_cast<double>(d1);
	customItemData.CustomDataDoubles.Doubles.Add(d2);

	const float d3 = static_cast<float>(dino->RandomMutationsFemaleField());
	const double d4 = static_cast<double>(d3);
	customItemData.CustomDataDoubles.Doubles.Add(d4);

	auto stat = dino->MyCharacterStatusComponentField();
	if (stat)
	{
		const double d5 = static_cast<double>(stat->DinoImprintingQualityField());
		customItemData.CustomDataDoubles.Doubles.Add(d5);
	}

	//
	// Custom Data Strings
	//
	FString sNeutered{ "" };
	FString sGender{ "Male" };

	if (dino->bIsFemale()())
		sGender = "FEMALE";

	if (dino->bNeutered()())
		sNeutered = "NEUTERED";

	FString color_indices{};
	dino->GetColorSetInidcesAsString(&color_indices);
	customItemData.CustomDataStrings.Add(DinoData.DinoNameInMap);
	customItemData.CustomDataStrings.Add(DinoData.DinoName);
	customItemData.CustomDataStrings.Add(color_indices);
	customItemData.CustomDataStrings.Add(sNeutered);
	customItemData.CustomDataStrings.Add(sGender);

	//
	// Custom Data Classes
	//
	customItemData.CustomDataClasses.Add(DinoData.DinoClass);

	//
	// Custom Data Bytes
	//
	FCustomItemByteArray dinoBytes, saddlebytes;
	dinoBytes.Bytes = DinoData.DinoData;
	customItemData.CustomDataBytes.ByteArrays.Add(dinoBytes);

	UPrimalInventoryComponent* DinoInventory = dino->MyInventoryComponentField();
	UPrimalItem* saddleItem = DinoInventory->GetEquippedItemOfType(EPrimalEquipmentType::DinoSaddle);

	if (saddleItem)
	{
		saddleItem->GetItemBytes(&saddlebytes.Bytes);
		customItemData.CustomDataBytes.ByteArrays.Add(saddlebytes);
	}
	UPrimalItem* item = UPrimalItem::AddNewItem(Class, nullptr, false, false, 0, true, 1, false, 0, false, nullptr, 0, false, false);
	item->SetCustomItemData(&customItemData);
	item->UpdatedItem(true);

	if (player->GetPlayerInventoryComponent())
	{
		player->GetPlayerInventoryComponent()->AddItemObjectEx(item, false, false, true, true, false, false, static_cast<AShooterCharacter*>(player->GetPlayerInventoryComponent()->GetOwner()), false, nullptr, false);
	}
	dino->Destroy(true, false);
}

void GetSoul(APrimalDinoCharacter* dino, AShooterPlayerController* player, UClass* Class)
{
	FCustomItemData customItemData;
	FARKDinoData dinoData;
	dino->GetDinoData(&dinoData);

	customItemData.CustomDataName = FName("Dino", EFindName::FNAME_Add);

	customItemData.CustomDataStrings.Add(dinoData.DinoNameInMap);
	customItemData.CustomDataStrings.Add(dinoData.DinoName);
	customItemData.CustomDataStrings.Add(L"0");
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bNeutered()())));
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bIsFemale()())));
	customItemData.CustomDataStrings.Add(L"");
	customItemData.CustomDataStrings.Add(L"");
	customItemData.CustomDataStrings.Add(L"SDOTU");
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bUsesGender()())));
	customItemData.CustomDataStrings.Add(L"0");
	customItemData.CustomDataStrings.Add(GetBlueprintShort(dino));
	customItemData.CustomDataStrings.Add(L"");
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bPreventMating()())));
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bUseBabyGestation()())));
	customItemData.CustomDataStrings.Add(FString(std::to_string(dino->bDebugBaby()())));
	customItemData.CustomDataStrings.Add("0"); 
	customItemData.CustomDataStrings.Add(dino->DescriptiveNameField());

	customItemData.CustomDataDoubles.Doubles.Add(ArkApi::GetApiUtils().GetWorld()->TimeSecondsField() * -1);
	customItemData.CustomDataDoubles.Doubles.Add(0);

	FCustomItemByteArray dinoBytes;
	dinoBytes.Bytes = dinoData.DinoData;
	customItemData.CustomDataBytes.ByteArrays.Add(dinoBytes); 

	UPrimalInventoryComponent* DinoInventory = dino->MyInventoryComponentField();
	UPrimalItem* saddleItem = DinoInventory->GetEquippedItemOfType(EPrimalEquipmentType::DinoSaddle);

	if (saddleItem)
	{
		FCustomItemByteArray saddlebytes, emptyBytes;
		saddleItem->GetItemBytes(&saddlebytes.Bytes);
		customItemData.CustomDataBytes.ByteArrays.Add(emptyBytes); 
		customItemData.CustomDataBytes.ByteArrays.Add(saddlebytes); 
	}

	UPrimalItem* item = UPrimalItem::AddNewItem(Class, nullptr, false, false, 0, false, 0, false, 0, false, nullptr, 0, false, false);
	item->SetCustomItemData(&customItemData);
	item->UpdatedItem(true);

	if (player->GetPlayerInventoryComponent())
	{
		player->GetPlayerInventoryComponent()->AddItemObjectEx(item, false, false, true, true, false, false, static_cast<AShooterCharacter*>(player->GetPlayerInventoryComponent()->GetOwner()), false, nullptr, false);
	}

	dino->Destroy(true, false);
}

void DestroyCryo(AShooterPlayerController* player, const int count)
{
	for (int i = 0; i < count; i++)
	{
		for (UPrimalItem* item : player->GetPlayerInventoryComponent()->InventoryItemsField())
		{
			FString name;
			FString empty = "Empty Cryopod";
			item->GetItemName(&name, false, false, player);
			if (name == empty)
			{
				if (!item->bIsBlueprint()())
				{
					item->OwnerInventoryField()->RemoveItem(&item->ItemIDField(), false, false, true, false);
					break;
				}
			}
		}
	}
}

void DestroySoul(AShooterPlayerController* player, const int count)
{
	for (int i = 0; i < count; i++)
	{
		for (UPrimalItem* item : player->GetPlayerInventoryComponent()->InventoryItemsField())
		{
			FString name;
			FString empty = "Soul Trap (DS)";
			FString EmptyStack = "Soul Traps (DS)";
			item->GetItemName(&name, false, false, player);
			if (name == empty)
			{
				if (!item->bIsBlueprint()())
				{
					item->OwnerInventoryField()->RemoveItem(&item->ItemIDField(), false, false, true, false);
					break;
				}
			}
			if (name == EmptyStack)
			{
				if (!item->bIsBlueprint()())
				{
					item->OwnerInventoryField()->RemoveItem(&item->ItemIDField(), false, false, true, false);
					break;
				}
			}
		}
	}
}

void GetCryoForTransfer(APrimalDinoCharacter* dino, AShooterPlayerController* player, UClass* Class)
{

	FCustomItemData customItemData;
	FARKDinoData DinoData;
	dino->GetDinoData(&DinoData);
	//
// Custom Data Name
//
	customItemData.CustomDataName = FName("Dino", EFindName::FNAME_Add);

	// one time use settings
	customItemData.CustomDataNames.Add(FName("MissionTemporary", EFindName::FNAME_Add));
	customItemData.CustomDataNames.Add(FName("None", EFindName::FNAME_Find));

	//
	// Custom Data Floats
	//
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Health]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Stamina]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Torpidity]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Oxygen]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Food]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Water]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Temperature]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::Weight]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::MeleeDamageMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::SpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::TemperatureFortitude]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->CurrentStatusValuesField()()[EPrimalCharacterStatusValue::CraftingSpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Health]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Stamina]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Torpidity]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Oxygen]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Food]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Water]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Temperature]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::Weight]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::MeleeDamageMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::SpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::TemperatureFortitude]);
	customItemData.CustomDataFloats.Add(dino->MyCharacterStatusComponentField()->MaxStatusValuesField()()[EPrimalCharacterStatusValue::CraftingSpeedMultiplier]);
	customItemData.CustomDataFloats.Add(dino->bIsFemale()());

	//
	// Custom Data Doubles
	//
	auto t1 = ArkApi::GetApiUtils().GetShooterGameMode()->GetWorld()->TimeSecondsField();
	customItemData.CustomDataDoubles.Doubles.Add(t1);
	customItemData.CustomDataDoubles.Doubles.Add(dino->BabyNextCuddleTimeField() - t1);
	customItemData.CustomDataDoubles.Doubles.Add(dino->NextAllowedMatingTimeField());

	const float d1 = static_cast<float>(dino->RandomMutationsMaleField());
	const double d2 = static_cast<double>(d1);
	customItemData.CustomDataDoubles.Doubles.Add(d2);

	const float d3 = static_cast<float>(dino->RandomMutationsFemaleField());
	const double d4 = static_cast<double>(d3);
	customItemData.CustomDataDoubles.Doubles.Add(d4);

	auto stat = dino->MyCharacterStatusComponentField();
	if (stat)
	{
		Log::GetLog()->info("DEBUG INFO: Stat is valid");
		const double d5 = static_cast<double>(stat->DinoImprintingQualityField());
		customItemData.CustomDataDoubles.Doubles.Add(d5);
	}

	//
	// Custom Data Strings
	//
	FString sNeutered{ "" };
	FString sGender{ "Male" };

	if (dino->bIsFemale()())
		sGender = "FEMALE";

	if (dino->bNeutered()())
		sNeutered = "NEUTERED";

	FString color_indices{};
	dino->GetColorSetInidcesAsString(&color_indices);
	customItemData.CustomDataStrings.Add(DinoData.DinoNameInMap);
	customItemData.CustomDataStrings.Add(DinoData.DinoName);
	customItemData.CustomDataStrings.Add(color_indices);
	customItemData.CustomDataStrings.Add(sNeutered);
	customItemData.CustomDataStrings.Add(sGender);

	//
	// Custom Data Classes
	//
	customItemData.CustomDataClasses.Add(DinoData.DinoClass);

	//
	// Custom Data Bytes
	//
	FCustomItemByteArray dinoBytes, saddlebytes;
	dinoBytes.Bytes = DinoData.DinoData;
	customItemData.CustomDataBytes.ByteArrays.Add(dinoBytes);

	UPrimalInventoryComponent* DinoInventory = dino->MyInventoryComponentField();
	UPrimalItem* saddleItem = DinoInventory->GetEquippedItemOfType(EPrimalEquipmentType::Hat);

	if (saddleItem)
	{
		saddleItem->GetItemBytes(&saddlebytes.Bytes);
		customItemData.CustomDataBytes.ByteArrays.Add(saddlebytes);
	}

	UPrimalItem* item = UPrimalItem::AddNewItem(Class, nullptr, false, false, 0, true, 1, false, 0, false, nullptr, 0, false, false);
	item->SetCustomItemData(&customItemData);
	item->UpdatedItem(true);
	if (player->GetPlayerInventoryComponent())
	{
		player->GetPlayerInventoryComponent()->AddItemObjectEx(item, false, false, true, true, false, false, static_cast<AShooterCharacter*>(player->GetPlayerInventoryComponent()->GetOwner()), false, nullptr, false);
	}
	APrimalStructureItemContainer* cache;
	dino->MyInventoryComponentField()->DropInventoryDeposit(ArkApi::GetApiUtils().GetWorld()->GetTimeSeconds() + 300, false, true, nullptr, nullptr, &cache, nullptr, FString(""), FString(""), -1, 300, false, -1, false, nullptr, false);
	dino->Destroy(true, false);
}
