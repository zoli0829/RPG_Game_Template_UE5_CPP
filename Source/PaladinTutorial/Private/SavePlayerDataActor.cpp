// Fill out your copyright notice in the Description page of Project Settings.


#include "SavePlayerDataActor.h"

#include "PaladinCharacter.h"
#include "Chaos/Deformable/ChaosDeformableCollisionsProxy.h"
#include "Components/SphereComponent.h"


// Sets default values
ASavePlayerDataActor::ASavePlayerDataActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create sphere component and attach to root
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->InitSphereRadius(300.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SphereComponent->SetupAttachment(RootComponent);

	// Bind overlap event
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASavePlayerDataActor::OnOverlapBegin);
}

void ASavePlayerDataActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APaladinCharacter* PaladinCharacter = Cast<APaladinCharacter>(OtherActor);

	if (PaladinCharacter)
	{
		PaladinCharacter->SavePlayerData();
	}
}

// Called when the game starts or when spawned
void ASavePlayerDataActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASavePlayerDataActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

