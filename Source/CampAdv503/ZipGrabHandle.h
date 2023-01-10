// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZipGrabHandle.generated.h"

UCLASS()
class CAMPADV503_API AZipGrabHandle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZipGrabHandle();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USceneComponent* ZipHandleRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ZipHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Carabiner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* HandleCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanGrabHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerGrabbedHandle;

	void GrabHandle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
	UFUNCTION()
	void ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

};
