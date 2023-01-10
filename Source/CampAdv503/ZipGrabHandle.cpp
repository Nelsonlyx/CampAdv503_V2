// Fill out your copyright notice in the Description page of Project Settings.


#include "ZipGrabHandle.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "VRCharacter.h"


// Sets default values
AZipGrabHandle::AZipGrabHandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ZipHandleRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZipStartPole"));
	SetRootComponent(ZipHandleRoot);
	ZipHandleRoot->SetHiddenInGame(true);

	ZipHandle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZipHandle"));
	ZipHandle->SetupAttachment(ZipHandleRoot);

	Carabiner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Carabiner"));
	Carabiner->SetHiddenInGame(true);
	Carabiner->SetupAttachment(ZipHandleRoot);

	HandleCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("HandleCollision"));
	HandleCollision->SetupAttachment(ZipHandleRoot);
}


// Called when the game starts or when spawned
void AZipGrabHandle::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AZipGrabHandle::ActorBeginOverlap);

	OnActorEndOverlap.AddDynamic(this, &AZipGrabHandle::ActorEndOverlap);
}

// Called every frame
void AZipGrabHandle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZipGrabHandle::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanGrabHandle = true;
}

void AZipGrabHandle::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	bCanGrabHandle = false;
}

void AZipGrabHandle::GrabHandle()
{
	if (!bCanGrabHandle)
	{
		return;
	}
	else
	{
		bPlayerGrabbedHandle = true;
	}
}