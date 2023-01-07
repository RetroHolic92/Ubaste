// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "MapEditorDataTypes.h"
#include "MapEditorGizmo.h"
#include "Components/ActorComponent.h"
#include "MapEditorHandlerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MAPEDITOR_API UMapEditorHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapEditorHandlerComponent();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "MapEditor")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MapEditor")
	FString MapDirectory;
	UPROPERTY(EditDefaultsOnly, Category = "MapEditor")
	TSubclassOf<AMapEditorGizmo> GizmoClass;
	UPROPERTY(EditDefaultsOnly, Category = "MapEditor")
	float ReplicationRate;
	
	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentActor, Category = "MapEditor | Selection")
	AActor* CurrentActor;
	UFUNCTION()
	void OnRep_CurrentActor();
	FTransform CurrentActorTransform;
	
	TWeakObjectPtr<APawn> ReturnPawn;

	UPROPERTY(BlueprintReadOnly, Category = "MapEditor | Edit")
	FMapEditorSnapping SnapAmount;

	TArray<FMapEditorSteps> UndoSteps;

	TWeakObjectPtr<AMapEditorGizmo> Gizmo;
	
	// Called when the game starts
	virtual void BeginPlay() override;

	void SetGizmo();

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_ReplicateTransform(AActor* Actor, FTransform Transform);

	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SpawnActor(TSubclassOf<AActor> ActorClass);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform& Transform);
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_DeleteActor(AActor* Actor);
	
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_UnpossessToReturnPawn();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FHitResult MouseTraceSingle(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine = false);
	FHitResult MouseTraceMulti(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine = false);
	
public:
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Initilization")
	void Init();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Initilization")
	void SetReturnPawn(APawn* Pawn);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Initilization")
	void UnpossessToReturnPawn();
	UFUNCTION(BlueprintPure, Category = "MapEditor | Initilization")
	bool HasValidReturnPawn() const { return ReturnPawn != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "MapEditor | Edit")
	void DuplicateActor(bool bSpawnInPlace);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Edit")
	void DeselectActor();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Edit")
	void DeleteActor();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Edit")
	void Undo();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Edit")
	void SetSnapAmount(FMapEditorSnapping SnappingAmounts);
	UFUNCTION(BlueprintPure, Category = "MapEditor | Edit")
	FMapEditorSnapping GetSnapAmount() const { return SnapAmount; }

	UFUNCTION(BlueprintPure, Category = "MapEditor | Edit")
	float GetReplicationRate() const { return ReplicationRate > 0.001 ? ReplicationRate : 0.1f; }
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Spawn")
	void SpawnActor(TSubclassOf<AActor> ActorClass);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Spawn")
	void SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform SpawnTransform);
	
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Select")
	void SetActor(AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Select")
	FHitResult MouseTrace(float Distance, bool& bHitGizmo, bool bDrawDebugLine = false);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Select")
	void Grab();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Select")
	void Release();

	void ReplicateActor();

	UFUNCTION(BlueprintCallable, Category = "MapEditor | Gizmo")
	void ShowMovement();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Gizmo")
	void ShowRotation();
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Gizmo")
	void ShowScale();
	UFUNCTION(BlueprintPure, Category = "MapEditor | Gizmo")
	EGizmoType GetGizmoType() const { return Gizmo.IsValid() ? Gizmo->GetGizmoType() : EGizmoType::Location; }

	UFUNCTION(BlueprintPure, Category = "MapEditor | Selected Actor")
	FTransform GetActorTransform() const { return CurrentActor ? CurrentActor->GetActorTransform() : FTransform(); }
	UFUNCTION(BlueprintPure, Category = "MapEditor | Selected Actor")
	FString GetActorName() const { return CurrentActor ? CurrentActor->GetName() : FString(); }
	UFUNCTION(BlueprintCallable, Category = "MapEditor | Selected Actor")
	void SetActorTransform(const FTransform NewTransform);

	bool HasAuthority() const { return GetOwner() ? GetOwner()->HasAuthority() : false; }
};