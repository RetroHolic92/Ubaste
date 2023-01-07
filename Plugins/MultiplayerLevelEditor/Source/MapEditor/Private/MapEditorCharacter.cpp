// Copyright 2021, Dakota Dawe, All rights reserved


#include "MapEditorCharacter.h"
#include "Components/MapEditorCharacterMovement.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AMapEditorCharacter::AMapEditorCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMapEditorCharacterMovement>(CharacterMovementComponentName))
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	GetMesh()->SetupAttachment(CameraComponent);
}

// Called when the game starts or when spawned
void AMapEditorCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void AMapEditorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}