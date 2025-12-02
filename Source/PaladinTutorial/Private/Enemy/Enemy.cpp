// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "PaladinCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy() :
	BaseDamage(5.0f),
	Health(100.0f),
	MaxHealth(100.0f)
{
	// Right weapon collision box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), RightWeaponSocketName);
}

void AEnemy::EnterCombat()
{
	CombatStrategy = MakeShared<class AttackStrategy>();
	CombatStrategy->Execute(this);

}

void AEnemy::ExitCombat()
{
	CombatStrategy = MakeShared<class PatrolStrategy>();
	CombatStrategy->Execute(this);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Setup enemy controller
	EnemyAIController = Cast<AEnemyAIController>(GetController());

	// Bind function to overlap for weapon box
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);

	// Setup right weapon collision
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;

	// auto is the same as APaladinCharacter* 
	auto Character = Cast<APaladinCharacter>(OtherActor);

	if (Character)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Hit Player"));
		UGameplayStatics::ApplyDamage(
			Character,
			BaseDamage,
			EnemyAIController,
			this,
			UDamageType::StaticClass()
			);
	}
}

void AEnemy::MeleeAttack()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		if (!AnimInstance->Montage_IsPlaying(AttackMontage))
		{
			// Get number of montage sections
			int32 const SectionCount = AttackMontage->CompositeSections.Num();

			// Get random animation to play
			// Get section index and playtime to use for timer
			FName const SectionName = GetAttackSectionName(SectionCount);
			int32 const SectionIndex = AttackMontage->GetSectionIndex(SectionName);
			float const SectionLength = AttackMontage->GetSectionLength(SectionIndex);

			// Play montage section
			AnimInstance->Montage_Play(AttackMontage);
			AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
			GetWorldTimerManager().SetTimer(TimerAttack, this, &AEnemy::ResetAttack, SectionLength, false);
		}
	}
}

void AEnemy::ResetAttack()
{
	// We can either call melee attack here or in blueprint, we just need to uncomment the above lines
	//MeleeAttack();
	// Reset enemy state
	// random count 1-4
	// if < 2 run strafe code or attack again
}

FName AEnemy::GetAttackSectionName(int32 SectionCount)
{
	const int32 Section = FMath::RandRange(1, SectionCount);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void AEnemy::EnemyPatrol()
{
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	CombatStrategy = MakeShared<class PatrolStrategy>();
	CombatStrategy->Execute(this);
}

void AEnemy::HitInterface_Implementation(FHitResult HitResult)
{
	// impact sound
	// impact niagara effect
	// hit montage
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.0f)
	{
		Health = 0.0f;
		// call blueprint function to play death montage and clean things up
		// Todo: create a flag to either play a montage or ragdoll
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Enemy died!"));
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

