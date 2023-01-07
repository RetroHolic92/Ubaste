// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapEditorDataTypes.h"
#include "MapEditorGizmo.generated.h"

class UStaticMeshComponent;
class UMapEditorHandlerComponent;
class APlayerController;

UCLASS()
class MAPEDITOR_API AMapEditorGizmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMapEditorGizmo();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* Origin;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* ZAxis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* XAxis;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* YAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* Yaw;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* Roll;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* Pitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* ScaleZ;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* ScaleX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	UStaticMeshComponent* ScaleY;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	float MovementSlowdown;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EMoveAxis GetMoveAxis(UPrimitiveComponent* HitComponent);
	ERotationAxis GetRotationAxis(UPrimitiveComponent* HitComponent);
	EScaleAxis GetScaleAxis(UPrimitiveComponent* HitComponent);
	FVector GetMouseWorldPosition();
	FVector2D GetMousePosition();
	//FVector GetMousePosition3D();

	void HandleMovement();
	void HandleRotation();
	void HandleScale();

	EMoveAxis MoveAxis;
	ERotationAxis RotationAxis;
	EScaleAxis ScaleAxis;

	EGizmoType CurrentGizmo;
	
	FVector CurrentMouseWorldPos;
	FVector2D ClickedMousePos;
	FVector ClickedMouseWorldPos;

	TWeakObjectPtr<APlayerController> OwningController;
	UPROPERTY()
	UMapEditorHandlerComponent* HandlerComponent;
	UPROPERTY()
	AActor* CurrentActor;

	bool bGrabbed;

	FTimerHandle TReplicateHandle;
	void Replicate();

	bool IsRightOfActor();

	bool GreaterThanSnapAmount(float Current, float New);
	
public:
	virtual void Tick(float DeltaSeconds) override;

	void SetHandler(UMapEditorHandlerComponent* Handler) { HandlerComponent = Handler; }
	
	void SnapToActor(AActor* Actor);
	void HideGizmo(bool Hide);
	void ClearGizmo();

	void HitGizmo(FHitResult HitResult);
	void ReleaseGizmo();

	void ShowMovement();
	void ShowRotation();
	void ShowScale();

	EGizmoType GetGizmoType() const { return CurrentGizmo; }
};