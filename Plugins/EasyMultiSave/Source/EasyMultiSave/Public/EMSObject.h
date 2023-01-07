//Easy Multi Save - Copyright (C) 2022 by Michael Hegemann.  

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EMSData.h"
#include "EMSCustomSaveGame.h"
#include "EMSInfoSaveGame.h"
#include "EMSPluginSettings.h"
#include "EMSAsyncLoadGame.h"
#include "EMSAsyncSaveGame.h"
#include "GameFramework/SaveGame.h"
#include "EMSObject.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogEasyMultiSave, Log, All);

const int PlayerIndex = 0;

UCLASS(config=Game, configdonotcheckdefaults)
class EASYMULTISAVE_API UEMSObject : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

/** Variables */

public:

	UPROPERTY(config)
	FString CurrentSaveGameName;

	UPROPERTY(config)
	FString CurrentSaveUserName;

	UPROPERTY(Transient)
	UEMSInfoSaveGame* CachedSlotInfoSave;

	UPROPERTY(Transient)
	TMap<FString, UEMSCustomSaveGame*> CachedCustomSaves;

	UPROPERTY(Transient)
	TArray<AActor*> ActorList;

	UPROPERTY(Transient)
	TArray<FLevelArchive> LevelArchiveList;	

	UPROPERTY(Transient)
	FMultiLevelStreamingData MultiLevelStreamData;

	UPROPERTY(Transient)
	FPlayerStackArchive PlayerStackData;

	UPROPERTY(Transient)
	TArray<FActorSaveData> SavedActors;

	UPROPERTY(Transient)
	TArray<FLevelScriptSaveData> SavedScripts;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameMode;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameState;

	UPROPERTY(Transient)
	FControllerSaveData SavedController;

	UPROPERTY(Transient)
	FPawnSaveData SavedPawn;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedPlayerState;

/** Blueprint Library function accessors */
	
public:

	UEMSCustomSaveGame* GetCustomSave(const TSubclassOf<UEMSCustomSaveGame>& SaveGameClass);
	bool SaveCustom(UEMSCustomSaveGame* SaveGame);
	void DeleteCustomSave(UEMSCustomSaveGame* SaveGame);

	void SetCurrentSaveGameName(const FString & SaveGameName);
	void SetCurrentSaveUserName(const FString& UserName);

	TArray<FString> GetSortedSaveSlots() const;
	TArray<FString> GetAllSaveUsers() const;

	UTexture2D* ImportSaveThumbnail(const FString& SaveGameName);
	void ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName);

	void DeleteAllSaveDataForSlot(const FString& SaveGameName);
	void DeleteAllSaveDataForUser(const FString& UserName);

/** Other public Functions  */

public:

	virtual class UWorld* GetWorld() const override;

	static UEMSObject* Get(UObject* WorldContextObject);

	FString GetCurrentSaveGameName() const;
	void SaveSlotInfoObject();
	UEMSInfoSaveGame* GetSlotInfoObject(FString SaveGameName = FString());

	bool DoesSaveGameExist(const FString& SaveGameName) const;

	void PrepareLoadAndSaveActors(const uint32& Flags, const bool& bFullReload = false);
	void LogFinishLoadingLevel();

	void SavePlayerActors();
	void LoadPlayerActors(UEMSAsyncLoadGame* LoadTask);

	void SaveLevelActors();
	void LoadLevelActors(UEMSAsyncLoadGame* LoadTask);

	void StartLoadLevelActors(UEMSAsyncLoadGame* LoadTask);
	void LoadAllLevelActors(UEMSAsyncLoadGame* LoadTask);

	bool SpawnOrUpdateLevelActor(const FActorSaveData& ActorArray);
	EUpdateActorResult UpdateLevelActor(const FActorSaveData& ActorArray);
	void SpawnLevelActor(const FActorSaveData& ActorArray);
	void ProcessLevelActor(AActor* Actor, const FActorSaveData& ActorArray);

	bool TryLoadPlayerFile();
	bool TryLoadLevelFile();

	APlayerController* GetPlayerController() const;
	APawn* GetPlayerPawn() const;

	FTimerManager& GetTimerManager() const;

	bool IsAsyncSaveOrLoadTaskActive(const ESaveGameMode& Mode = ESaveGameMode::MODE_All, const EAsyncCheckType& CheckType = EAsyncCheckType::CT_Both, const bool& bLogAndReturnError = true) const;

	bool HasValidGameMode() const;
	bool HasValidPlayer() const;

/** Internal Functions  */

protected:

	bool VerifyOrCreateDirectory(const FString& NewDir);

	bool SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath);
	bool LoadBinaryArchive(const EDataLoadType& LoadType, const FString& FullSavePath, UObject* Object = nullptr);
	bool UnpackBinaryArchive(const EDataLoadType& LoadType, FMemoryReader FromBinary, UObject* Object = nullptr);
	bool UnpackLevel(const FLevelArchive& LevelArchive);
	bool UnpackPlayer(const FPlayerArchive& PlayerArchive);
	
	void DirectSetPlayerPosition(const FPlayerPositionArchive& PosArchive);
	
	USaveGame* CreateSaveObject(const TSubclassOf<USaveGame>& SaveGameClass);
	bool SaveObject(const FString& FullSavePath, USaveGame* SaveGameObject);
	USaveGame* LoadObject(const FString& FullSavePath, TSubclassOf<USaveGame> SaveGameClass);

	void SaveActorToBinary(AActor* Actor, FGameObjectSaveData& OutData);
	void LoadActorFromBinary(AActor* Actor, const FGameObjectSaveData& InData);

	void SerializeToBinary(UObject* Object, TArray<uint8>& OutData);
	void SerializeFromBinary(UObject* Object, const TArray<uint8>& InData);

	void SerializeActorStructProperties(AActor* Actor);
	void SerializeStructProperties(UObject* Object);
	void SerializeScriptStruct(UStruct* ScriptStruct);
	void SerializeArrayStruct(FArrayProperty* ArrayProp);
	void SerializeMap(FMapProperty* MapProp);

	void SaveActorComponents(AActor* Actor, TArray<FComponentSaveData>& OutComponents);
	void LoadActorComponents(AActor* Actor, const TArray<FComponentSaveData>& InComponents);

	bool HasSaveInterface(const AActor* Actor) const;
	bool IsValidActor(const AActor* Actor) const;
	bool IsValidForSaving(const AActor* Actor) const;
	bool IsValidForLoading(const AActor* Actor) const;

	bool SaveAsLevelActor(const APawn* Pawn) const;

	bool CheckForExistingActor(const FActorSaveData& ActorArray);

	EActorType GetActorType(const AActor* Actor) const;
    bool IsMovable(const USceneComponent* SceneComp) const;

	TArray<uint8> BytesFromString(const FString& String) const;
	FString StringFromBytes(const TArray<uint8>& Bytes) const;

	FName GetLevelName() const;

	TArray<FString> GetSaveSlotsDesktop() const;
	TArray<FString> GetSaveSlotsConsole() const;

	template <class TSaveGame>
	TSaveGame* GetDesiredSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName, TSaveGame*& SaveGameObject);

/** Settings Helpers  */

public:

	FORCEINLINE bool IsNormalMultiLevelSave() const
	{
		return UEMSPluginSettings::Get()->MultiLevelSaving != EMultiLevelSaveMethod::ML_Disabled
			&& UEMSPluginSettings::Get()->MultiLevelSaving != EMultiLevelSaveMethod::ML_Stream;
	}

	FORCEINLINE bool IsSlowMultiLevelSave() const
	{
		return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Slow;
	}

	FORCEINLINE bool IsStreamMultiLevelSave() const
	{
		return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Stream;
	}

	FORCEINLINE bool IsConsoleFileSystem() const
	{
		return UEMSPluginSettings::Get()->FileSaveMethod == EFileSaveMethod::FM_Console;
	}

/** File Access and Path Names  */

public:

	FORCEINLINE void ClearSavedLevelActors()
	{
		SavedActors.Empty();
		SavedScripts.Empty();
	}

	FORCEINLINE void ClearCachedSlot()
	{
		CachedSlotInfoSave = nullptr;

		LevelArchiveList.Empty();
		MultiLevelStreamData = FMultiLevelStreamingData();
		PlayerStackData = FPlayerStackArchive();
	}

	FORCEINLINE void ClearCachedCustomSaves()
	{
		CachedCustomSaves.Empty();
	}

	FORCEINLINE FString SaveUserDir() const
	{
		return FPaths::ProjectSavedDir() + TEXT("SaveGames/Users/");
	}

	FORCEINLINE FString UserSubDir() const
	{
		//Takes into account the already defined path from ISaveGame
		return TEXT("Users/") + CurrentSaveUserName + Slash;
	}

	FORCEINLINE FString BaseSaveDir() const
	{
		if (!CurrentSaveUserName.IsEmpty())
		{
			return SaveUserDir() + CurrentSaveUserName + Slash;
		}

		return FPaths::ProjectSavedDir() + TEXT("SaveGames/");
	}

	FORCEINLINE FString AllSaveFiles() const
	{
		return BaseSaveDir() + TEXT("*.sav");
	}

	FORCEINLINE FString AllThumbnailFiles() const
	{
		return BaseSaveDir() + TEXT("*.png");
	}

	FORCEINLINE FString GetFolderOrFile() const
	{
		//Console uses file names and not folders, "/" will automatically create a new folder.
		const bool bFile = IsConsoleFileSystem();
		const FString FolderOrFile = bFile ? Underscore : Slash;

		return FolderOrFile;
	}

	FORCEINLINE FString FullSaveDir(const FString& DataType, FString SaveGameName = FString()) const
	{
		if (SaveGameName.IsEmpty())
		{
			SaveGameName = GetCurrentSaveGameName();
		}

		const FString FullName = SaveGameName + GetFolderOrFile() + DataType;

		if (!CurrentSaveUserName.IsEmpty())
		{
			return UserSubDir() + FullName;
		}

		return FullName;
	}

	FORCEINLINE FString CustomSaveFile(const FString& CustomSaveName, const bool bUseSlot = false) const
	{
		//Bound to a save slot, use default dir.
		if (bUseSlot)
		{
			return FullSaveDir(CustomSaveName);
		}

		//Not bound to slot, so we just save in the base folder. with user if desired.
		if (!CurrentSaveUserName.IsEmpty())
		{
			return UserSubDir() + CustomSaveName;
		}

		return CustomSaveName;
	}

	FORCEINLINE FString SlotInfoSaveFile(const FString& SaveGameName = FString()) const
	{
		return FullSaveDir(SlotSuffix, SaveGameName);
	}

	FORCEINLINE FString ActorSaveFile(const FString& SaveGameName = FString()) const
	{
		return FullSaveDir(ActorSuffix, SaveGameName);
	}

	FORCEINLINE FString PlayerSaveFile(const FString& SaveGameName = FString())  const
	{
		return FullSaveDir(PlayerSuffix, SaveGameName);
	}

	FORCEINLINE FString ThumbnailSaveFile(const FString& SaveGameName) const
	{
		const FString ThumbnailPath = BaseSaveDir() + SaveGameName + GetFolderOrFile();
		return ThumbnailPath + TEXT("thumb.png");
	}

	FORCEINLINE FString SlotFilePath(const FString& SaveGameName = FString()) const
	{
		//This is only used for sorting.
		return BaseSaveDir() + SlotInfoSaveFile(SaveGameName) + SaveType;
	}

	FORCEINLINE FName LevelScriptSaveName(const AActor* Actor) const
	{
		return FName(*Actor->GetLevel()->GetOuter()->GetName());
	}

	FORCEINLINE bool IsPlacedActor(const AActor* Actor) const
	{
		return Actor->IsNetStartupActor();
	}

	FORCEINLINE FString GetFullActorName(const AActor* Actor) const
	{
		const FString ActorName = Actor->GetName();

		//This is only valid for placed Actors. Runtime Actors are always in the persistent.
		//Can't use GetActorType here, since it would crash Multi-Thread loading.
		if (IsPlacedActor(Actor))
		{
			const FString LevelName = Actor->GetLevel()->GetOuter()->GetName();
			const bool bAlreadyHas = ActorName.Contains(LevelName);

			if (bAlreadyHas)
			{
				return ActorName;
			}
			else
			{
				return LevelName + Underscore + ActorName;
			}
		}
		
		return ActorName;
	}

	FORCEINLINE FString ValidateSaveName(const FString& SaveGameName) const
	{
		FString CurrentSave = SaveGameName;
		CurrentSave = CurrentSave.Replace(TEXT(" "), *Underscore);
		CurrentSave = CurrentSave.Replace(TEXT("."), *Underscore);

		return FPaths::MakeValidFileName(*CurrentSave);
	}



};
