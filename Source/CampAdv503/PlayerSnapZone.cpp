// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSnapZone.h"
#include "Components/SceneComponent.h"
#include "HandController.h"
#include "VRCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"


// Sets default values
APlayerSnapZone::APlayerSnapZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SnapZone = CreateDefaultSubobject<USceneComponent>(TEXT("SnapZone"));
	SetRootComponent(SnapZone);

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));
	TriggerZone->SetupAttachment(SnapZone);
}

// Called when the game starts or when spawned
void APlayerSnapZone::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &APlayerSnapZone::ActorBeginOverlap);

	OnActorEndOverlap.AddDynamic(this, &APlayerSnapZone::ActorEndOverlap);

	TeleportVisualizer = GetWorld()->SpawnActor<AActor>(TeleportVisualizerClass);
	if (TeleportVisualizer != nullptr)
	{
		TeleportVisualizer->AttachToComponent(SnapZone, FAttachmentTransformRules::KeepRelativeTransform);
		TeleportVisualizer->SetOwner(this);
		TeleportVisualizer->SetActorHiddenInGame(true);
	}
}

// Called every frame
void APlayerSnapZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlayerInRange)
	{
		bPlayerIsClimbing = IsPlayerClimbing();

		if (bPlayerIsClimbing)
		{
			bPlayerWasClimbing = true;
		}
		else if (bPlayerWasClimbing)
		{
			StartSnapToZone();
		}
	}

}

void APlayerSnapZone::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	SetActorTickEnabled(true);
	bPlayerInRange = true;
	TeleportVisualizer->SetActorHiddenInGame(false);
}

void APlayerSnapZone::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	SetActorTickEnabled(false);
	bPlayerInRange = false;
	bPlayerIsClimbing = false;
	bPlayerWasClimbing = false;
	TeleportVisualizer->SetActorHiddenInGame(true);
}

bool APlayerSnapZone::IsPlayerClimbing()
{
	VRChar = Cast<AVRCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (VRChar != nullptr)
	{
		return VRChar->bIsClimbing;
	}
	return false;
}

void APlayerSnapZone::StartSnapToZone()
{
	VRChar = Cast<AVRCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (VRChar != nullptr)
	{
		FTimerHandle FadeTimerHandle;
		VRChar->StartFade(0, 1);
		GetWorldTimerManager().SetTimer(FadeTimerHandle, this, &APlayerSnapZone::FinishSnapToZone, VRChar->TeleportFadeDuration, false);
	}
}

void APlayerSnapZone::FinishSnapToZone()
{
	VRChar = Cast<AVRCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (VRChar != nullptr)
	{
		VRChar->TeleportTo(SnapZone->GetComponentLocation(), VRChar->GetActorRotation());
		VRChar->StartFade(1, 0);
	}
}

