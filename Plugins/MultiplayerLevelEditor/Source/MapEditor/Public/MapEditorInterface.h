// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MapEditorDataTypes.h"
#include "MapEditorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMapEditorInterface : public UInterface
{
	GENERATED_BODY()
};

class UMeshComponent;
class UMaterialInterface;

class MAPEDITOR_API IMapEditorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "MapEditor | Interface")
	void OnGrabbed();
	UFUNCTION(BlueprintImplementableEvent, Category = "MapEditor | Interface")
	void OnRelease();
	UFUNCTION(BlueprintImplementableEvent, Category = "MapEditor | Interface")
	void OnDeleted();
	UFUNCTION(BlueprintImplementableEvent, Category = "MapEditor | Interface")
	void OnUndo();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MapEditor | Interface")
	void OnMaterialLoaded(FMapEditorItemMaterial MapEditorItemMaterial);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "MapEditor | Interface")
	void OnScaleChanged(FVector NewScale);
};
