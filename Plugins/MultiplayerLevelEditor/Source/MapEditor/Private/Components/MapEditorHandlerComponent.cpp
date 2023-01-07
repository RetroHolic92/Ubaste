// Copyright 2021, Dakota Dawe, All rights reserved


#include "Components/MapEditorHandlerComponent.h"
#include "MapEditorGizmo.h"
#include "MapEditorInterface.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

UMapEditorHandlerComponent::UMapEditorHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	MapDirectory = FString("Maps");

	ReplicationRate = 1.0f;
}


void UMapEditorHandlerComponent::OnRep_CurrentActor()
{
	SetActor(CurrentActor);
}

void UMapEditorHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (APawn* OwningPawn = GetOwner<APawn>())
	{
		if (OwningPawn->IsLocallyControlled() && GizmoClass)
		{
			Gizmo = GetWorld()->SpawnActor<AMapEditorGizmo>(GizmoClass);
			Gizmo->SetHandler(this);
		}
	}
}

void UMapEditorHandlerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UMapEditorHandlerComponent, CurrentActor, COND_OwnerOnly);
}

FHitResult UMapEditorHandlerComponent::MouseTraceSingle(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector End = WorldLocation + WorldDirection * Distance;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			FHitResult HitResult;
			bool bBlockingHit = false;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, End, CollisionChannel, Params))
			{
				bBlockingHit = true;
			}

			if (bDrawDebugLine)
			{
				DrawDebugLine(GetWorld(), WorldLocation, End, FColor::Red, false, 5.0f, 0, 3.0f);
			}

			if (bBlockingHit)
			{
				return HitResult;
			}
		}
	}
	return FHitResult();
}

FHitResult UMapEditorHandlerComponent::MouseTraceMulti(const float Distance, bool& bHitGizmo, const ECollisionChannel CollisionChannel, const bool bDrawDebugLine)
{
	FHitResult HitResult;
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector End = WorldLocation + WorldDirection * Distance;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(GetOwner());
			TArray<FHitResult> HitResults;
			if (GetWorld()->LineTraceMultiByChannel(HitResults, WorldLocation, End, CollisionChannel, Params))
			{
				if (Gizmo.IsValid())
				{
					for (FHitResult& Result : HitResults)
					{
						if (Result.GetActor() == Gizmo)
						{
							bHitGizmo = true;
							HitResult = Result;
						}
					}
				}
			}
			else
			{
				if (HitResults.Num())
				{
					return HitResults[0];
				}
			}

			if (bDrawDebugLine)
			{
				DrawDebugLine(GetWorld(), WorldLocation, End, FColor::Red, false, 5.0f, 0, 3.0f);
			}
		}
	}
	return HitResult;
}

void UMapEditorHandlerComponent::Init()
{
	if (APawn* OwningPawn = GetOwner<APawn>())
	{
		if (OwningPawn->IsLocallyControlled() && GizmoClass)
		{
			Gizmo = GetWorld()->SpawnActor<AMapEditorGizmo>(GizmoClass);
			Gizmo->SetHandler(this);
		}
	}
}

void UMapEditorHandlerComponent::SetReturnPawn(APawn* Pawn)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		ReturnPawn = Pawn;
	}
}

bool UMapEditorHandlerComponent::Server_UnpossessToReturnPawn_Validate()
{
	return true;
}

void UMapEditorHandlerComponent::Server_UnpossessToReturnPawn_Implementation()
{
	UnpossessToReturnPawn();
}

void UMapEditorHandlerComponent::UnpossessToReturnPawn()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (GetOwner() && ReturnPawn.IsValid())
		{
			GetOwner<APawn>()->GetController()->Possess(ReturnPawn.Get());
			GetOwner()->Destroy();
		}
	}
	else
	{
		Server_UnpossessToReturnPawn();
	}
	if (Gizmo.IsValid())
	{
		Gizmo->HideGizmo(true);
		Gizmo->ClearGizmo();
	}
}

void UMapEditorHandlerComponent::SetGizmo()
{
	if (Gizmo.IsValid())
	{
		
	}
}

bool UMapEditorHandlerComponent::Server_ReplicateTransform_Validate(AActor* Actor, FTransform Transform)
{
	return true;
}

void UMapEditorHandlerComponent::Server_ReplicateTransform_Implementation(AActor* Actor, FTransform Transform)
{
	if (Actor)
	{
		Actor->SetActorTransform(Transform);
	}
}

void UMapEditorHandlerComponent::SetActor(AActor* Actor)
{
	if (Actor)
	{
		if (!Actor->IsRootComponentMovable())
		{
			return;
		}
		if (Actor->Implements<UMapEditorInterface>())
		{
			IMapEditorInterface::Execute_OnGrabbed(Actor);
		}
	}
	
	CurrentActor = Actor;

	if (CurrentActor)
	{
		FMapEditorSteps UndoStep;
		UndoStep.Actor = CurrentActor;
		UndoSteps.Add(UndoStep);
	}
	
	if (Gizmo.IsValid())
	{
		if (CurrentActor)
		{
			Gizmo->SnapToActor(CurrentActor);
			Gizmo->HideGizmo(false);
		}
		else
		{
			Gizmo->HideGizmo(true);
			Gizmo->ClearGizmo();
		}
	}
}

FHitResult UMapEditorHandlerComponent::MouseTrace(float Distance, bool& bHitGizmo, bool bDrawDebugLine)
{
	return MouseTraceMulti(Distance, bHitGizmo, TraceCollisionChannel, bDrawDebugLine);
}

void UMapEditorHandlerComponent::Grab()
{
	bool bHitGizmo = false;
	FHitResult HitResult = MouseTrace(100000.0f, bHitGizmo, false);

	if (bHitGizmo && Gizmo.IsValid())
	{
		Gizmo->HitGizmo(HitResult);
	}
	else
	{
		SetActor(HitResult.GetActor());
	}
}

void UMapEditorHandlerComponent::Release()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ReleaseGizmo();
		if (CurrentActor)
		{
			if (CurrentActor->Implements<UMapEditorInterface>())
			{
				IMapEditorInterface::Execute_OnRelease(CurrentActor);
			}
			int32 Index = UndoSteps.Num() - 1;
			if (Index >= 0)
			{
				UndoSteps[Index].Transforms.Add(CurrentActor->GetActorTransform());
			}
		}
	}
}

void UMapEditorHandlerComponent::ReplicateActor()
{
	if (CurrentActor)
	{
		if (!CurrentActor->GetActorTransform().Equals(CurrentActorTransform))
		{
			CurrentActorTransform = CurrentActor->GetActorTransform();
			if (!HasAuthority())
			{
				Server_ReplicateTransform(CurrentActor, CurrentActor->GetActorTransform());
			}
			else
			{
				if (CurrentActor->Implements<UMapEditorInterface>())
				{
					IMapEditorInterface::Execute_OnScaleChanged(CurrentActor, CurrentActor->GetActorTransform().GetScale3D());
				}
			}
		}
	}
}

void UMapEditorHandlerComponent::ShowMovement()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowMovement();
	}
}

void UMapEditorHandlerComponent::ShowRotation()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowRotation();
	}
}

void UMapEditorHandlerComponent::ShowScale()
{
	if (Gizmo.IsValid())
	{
		Gizmo->ShowScale();
	}
}

void UMapEditorHandlerComponent::SetActorTransform(const FTransform NewTransform)
{
	if (CurrentActor)
	{
		CurrentActor->SetActorTransform(NewTransform);
		if (Gizmo.IsValid())
		{
			Gizmo->SnapToActor(CurrentActor);
		}
		ReplicateActor();
	}
}

void UMapEditorHandlerComponent::DuplicateActor(bool bSpawnInPlace)
{
	if (IsValid(CurrentActor))
	{
		bool bHitGizmo;
		const FHitResult HitResult = MouseTraceSingle(10000.0f, bHitGizmo, ECC_Visibility, false);

		if (!HitResult.Location.Equals(FVector::ZeroVector))
		{
			if (bSpawnInPlace)
			{
				SpawnActorAtTransform(CurrentActor->GetClass(), CurrentActor->GetTransform());
			}
			else
			{
				SpawnActorAtTransform(CurrentActor->GetClass(), FTransform(HitResult.Location));
			}
		}
	}
}

void UMapEditorHandlerComponent::DeselectActor()
{
	CurrentActor = nullptr;
}

void UMapEditorHandlerComponent::DeleteActor()
{
	if (CurrentActor)
	{
		if (CurrentActor->Implements<UMapEditorInterface>())
		{
			IMapEditorInterface::Execute_OnGrabbed(CurrentActor);
		}
		if (HasAuthority())
		{
			CurrentActor->Destroy();
		}
		else
		{
			Server_DeleteActor(CurrentActor);
		}
		if (Gizmo.IsValid())
		{
			Gizmo->ClearGizmo();
			Gizmo->HideGizmo(true);
		}
		CurrentActor = nullptr;
	}
}

void UMapEditorHandlerComponent::Undo()
{
	int32 Index = UndoSteps.Num() - 1;
	if (Index >= 0)
	{
		FMapEditorSteps UndoStep = UndoSteps[Index];
		if (UndoStep.Actor && UndoStep.Transforms.Num() - 1)
		{
			const int32 TransformIndex = UndoStep.Transforms.Num() - 1;
			if (TransformIndex - 1 >= 0)
			{
				UndoStep.Actor->SetActorTransform(UndoStep.Transforms[TransformIndex - 1]);
				UndoStep.Transforms.RemoveAt(TransformIndex);
				UndoSteps[Index] = UndoStep;
				if (Gizmo.IsValid())
				{
					CurrentActor = UndoStep.Actor;
					if (CurrentActor->Implements<UMapEditorInterface>())
					{
						IMapEditorInterface::Execute_OnUndo(CurrentActor);
					}
					Gizmo->SnapToActor(CurrentActor);
					ReplicateActor();
				}
			}
			else
			{
				UndoSteps.RemoveAt(Index);
				Undo();
			}
		}
		else
		{
			UndoSteps.RemoveAt(Index);
			Undo();
		}
	}
}

void UMapEditorHandlerComponent::SetSnapAmount(FMapEditorSnapping SnappingAmounts)
{
	SnapAmount.Location = SnappingAmounts.Location >= 0 ? SnappingAmounts.Location : 0.0f;
	SnapAmount.Rotation = SnappingAmounts.Rotation >= 0 ? SnappingAmounts.Rotation : 0.0f;
	SnapAmount.Scale = SnappingAmounts.Scale >= 0 ? SnappingAmounts.Scale : 0.0f;
}

bool UMapEditorHandlerComponent::Server_SpawnActor_Validate(TSubclassOf<AActor> ActorClass)
{
	return true;
}

void UMapEditorHandlerComponent::Server_SpawnActor_Implementation(TSubclassOf<AActor> ActorClass)
{
	SpawnActor(ActorClass);
}

void UMapEditorHandlerComponent::SpawnActor(TSubclassOf<AActor> ActorClass)
{
	if (ActorClass)
	{
		if (HasAuthority())
		{
			if (GetOwner())
			{
				FTransform Transform = GetOwner()->GetActorTransform();
				FVector SpawnLocation = Transform.GetLocation() + Transform.Rotator().Vector() * 500.0f;
				Transform.SetLocation(SpawnLocation);

				FVector End = SpawnLocation;
				End.Z -= 10000.0f;
				FHitResult HitResult;
				if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, End, ECollisionChannel::ECC_Visibility))
				{
					SpawnLocation = HitResult.Location;
				}

				Transform.SetRotation(FRotator::ZeroRotator.Quaternion());
				
				CurrentActor = GetWorld()->SpawnActor<AActor>(ActorClass, Transform);
				OnRep_CurrentActor();
			}
		}
		else
		{
			Server_SpawnActor(ActorClass);
		}
	}
}

bool UMapEditorHandlerComponent::Server_SpawnActorAtTransform_Validate(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	return true;
}

void UMapEditorHandlerComponent::Server_SpawnActorAtTransform_Implementation(TSubclassOf<AActor> ActorClass, const FTransform& Transform)
{
	SpawnActorAtTransform(ActorClass, Transform);
}

void UMapEditorHandlerComponent::SpawnActorAtTransform(TSubclassOf<AActor> ActorClass, const FTransform SpawnTransform)
{
	if (ActorClass)
	{
		if (HasAuthority())
		{
			if (GetOwner())
			{
				CurrentActor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform);
				OnRep_CurrentActor();
			}
		}
		else
		{
			Server_SpawnActorAtTransform(ActorClass, SpawnTransform);
		}
	}
}

bool UMapEditorHandlerComponent::Server_DeleteActor_Validate(AActor* Actor)
{
	return true;
}

void UMapEditorHandlerComponent::Server_DeleteActor_Implementation(AActor* Actor)
{
	if (Actor)
	{
		Actor->Destroy();
	}
}