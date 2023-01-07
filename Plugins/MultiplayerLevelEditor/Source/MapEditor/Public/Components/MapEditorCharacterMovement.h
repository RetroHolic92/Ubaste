// Copyright 2021, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "MapEditorHandlerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MapEditorCharacterMovement.generated.h"

class FSavedMove_MapEditor;
class FNetworkPredictionData_Client;

UCLASS()
class MAPEDITOR_API UMapEditorCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UMapEditorCharacterMovement(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(EditDefaultsOnly, Category = "MapEditor | Default")
	bool bCenterMouseOnStopMovementMode;

	TWeakObjectPtr<UMapEditorHandlerComponent> MapEditorHandlerComponent;
	
	bool bInMovementMode;
	int32 ScreenCenterX, ScreenCenterY;

	float MouseStartX, MouseStartY;

	virtual void BeginPlay() override;
	
	friend FSavedMove_MapEditor;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	UPROPERTY(EditAnywhere, Category = "MapEditor | PawnMovement")
	float MaxSpeedMultiplier;
	UPROPERTY(EditAnywhere, Category = "MapEditor | PawnMovement")
	float MinSpeedMultiplier;
	
	float RequestedSpeedMultiplier;
	
public:
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void MoveForward(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void MoveRight(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void MoveUp(float Value = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void LookUp(float Value = 1.0f);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void Turn(float Value = 1.0f);
	
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void EnterMovementMode(bool Enter);
	UFUNCTION(BlueprintPure, Category = "MapEditor | PawnMovement")
	bool InMovementMode() const { return bInMovementMode; }
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void IncreaseSpeedMultiplier(float IncreaseAmount = 0.2f);
	UFUNCTION(BlueprintCallable, Category = "MapEditor | PawnMovement")
	void DecreaseSpeedMultiplier(float DecreaseAmount = 0.2f);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetSpeedMultiplier(float SpeedMultiplier);

	UFUNCTION(BlueprintCallable, Category = "MapEditor | Init")
	void Init();
};

class FSavedMove_MapEditor : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;
	virtual void Clear() override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;
	
	float SavedRequestedSpeedMultiplier = 1.0f;
};

class FNetworkPredictionData_Client_MapEditor : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_MapEditor(const UCharacterMovementComponent& ClientMovement):Super(ClientMovement){}
	typedef FNetworkPredictionData_Client_Character Super;
	virtual FSavedMovePtr AllocateNewMove() override;
};