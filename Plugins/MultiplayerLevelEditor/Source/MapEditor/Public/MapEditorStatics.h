// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapEditorDataTypes.h"
#include "MapEditorStatics.generated.h"

/**
 * 
 */
UCLASS()
class MAPEDITOR_API UMapEditorStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FString EncodeString(const FString& StringToEncode);
	static FString DecodeString(const FString& StringToEncode);
	
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Serialization")
	static FString SerializeLevel(AActor* WorldActor, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Serialization")
	static FMapEditorItems DeSerializeLevel(const FString& JsonString, bool& Success);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Map")
	static void SpawnMapItems(AActor* WorldActor, FMapEditorItems MapItems);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Map")
	static void SpawnMapItemsFromJson(AActor* WorldActor, const FString& JsonString);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Map")
	static void ClearMap(AActor* WorldActor);

	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static bool LoadMapFromFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& Extension, FString& OutString, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static bool SaveMapToFile(AActor* WorldActor, const FString& MapDirectory, const FString& MapName, const FString& StringToSave, FString& FullMapName);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static bool DoesMapExist(AActor* WorldActor, const FString& MapDirectory, const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static FString GetRealMapName(const FString& MapName);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static TArray<FString> GetMapList(AActor* WorldActor, const FString& MapDirectory, bool bCutLevelname = true, bool bShowAllMaps = false);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | FileHandling")
	static FString RemoveExtension(const FString& String);
	
	static void StripInvalidMaps(const FString& WorldName, TArray<FString>& MapList);

	UFUNCTION(BlueprintCallable, Category = "MapEditor | Material")
	static void SetMaterials(const FMapEditorItemMaterial& MapEditorItemMaterial);
};
