// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Components/MeshComponent.h"
#include "MapEditorDataTypes.generated.h"

UENUM(BlueprintType)
enum class EMoveAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	XAxis	UMETA(DisplayName = "XAxis"),
	YAxis	UMETA(DisplayName = "Yxis"),
	ZAxis	UMETA(DisplayName = "ZAxis")
};

UENUM(BlueprintType)
enum class ERotationAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	Yaw		UMETA(DisplayName = "Yaw"),
	Roll	UMETA(DisplayName = "Roll"),
	Pitch	UMETA(DisplayName = "Pitch")
};

UENUM(BlueprintType)
enum class EScaleAxis : uint8
{
	None	UMETA(DisplayName = "None"),
	ScaleX	UMETA(DisplayName = "ScaleX"),
	ScaleY	UMETA(DisplayName = "ScaleY"),
	ScaleZ	UMETA(DisplayName = "ScaleZ")
};

UENUM(BlueprintType)
enum class EGizmoType : uint8
{
	None		UMETA(DisplayName = "None"),
	Location	UMETA(DisplayName = "Location"),
	Rotation	UMETA(DisplayName = "Rotation"),
	Scale		UMETA(DisplayName = "Scale")
};

USTRUCT(BlueprintType)
struct FMapEditorSnapping
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "MapEditor")
	float Location;
	UPROPERTY(BlueprintReadWrite, Category = "MapEditor")
	float Rotation;
	UPROPERTY(BlueprintReadWrite, Category = "MapEditor")
	float Scale;

	FMapEditorSnapping()
	{
		Location = 5.0f;
		Rotation = 5.0f;
		Scale = 0.0f;
	}
	FMapEditorSnapping(float DefaultSnapValue)
	{
		Location = DefaultSnapValue;
		Rotation = DefaultSnapValue;
		Scale = 0.0f;
	}
};

USTRUCT()
struct FMapEditorSteps
{
	GENERATED_BODY()
	AActor* Actor;
	TArray<FTransform> Transforms;
};

USTRUCT(BlueprintType)
struct FMapEditorItemMaterial
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	TArray<UMeshComponent*> MeshComponents;
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FMapEditorItem
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	TSubclassOf<AActor> ActorToSpawn;
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	FTransform ItemTransform;
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	TArray<UMaterialInterface*> Materials;
};

USTRUCT(BlueprintType)
struct FMapEditorItems
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category = "MapEditor")
	TArray<FMapEditorItem> Items;
};