// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyProjectile.h"

#include "PaladinCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AEnemyProjectile::AEnemyProjectile() :
ProjectileDamage(10.0f),
ProjectileLife(60.0f)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
	ProjectileMesh->SetupAttachment(SphereComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
}

void AEnemyProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if it hits the player
	if (OtherActor == nullptr) return;

	auto Character = Cast<APaladinCharacter>(OtherActor);

	if (Character)
	{
		UGameplayStatics::ApplyDamage(Character, ProjectileDamage, nullptr, this, UDamageType::StaticClass());
	}
	else
	{
		GetWorldTimerManager().SetTimer(ProjectileTimer, this, &AEnemyProjectile::DestroyProjectile, ProjectileLife);
	}
}

void AEnemyProjectile::DestroyProjectile()
{
	Destroy();
}

// Called when the game starts or when spawned
void AEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Bind function to overlap events for collision boxes
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnProjectileOverlap);
}

// Called every frame
void AEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

