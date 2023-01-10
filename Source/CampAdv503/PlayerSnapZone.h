// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSnapZone.generated.h"

UCLASS()
class CAMPADV503_API APlayerSnapZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerSnapZone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Components
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UBoxComponent* TriggerZone;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class USceneComponent* SnapZone;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class AActor* TeleportVisualizer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AActor> TeleportVisualizerClass;
	class AVRCharacter* VRChar;
	class USceneComponent* VRCharacterScene;

	// State
	bool bPlayerInRange = false;
	bool bPlayerIsClimbing = false;
	bool bPlayerWasClimbing = false;

	// Config
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float SnapSpeed = 0.4f;

	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool IsPlayerClimbing();
	void StartSnapToZone();
	void FinishSnapToZone();
};
