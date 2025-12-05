// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "PaladinCharacter.h"
#include "Enemy/EnemyAIController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemy::AEnemy() :
	BaseDamage(5.0f),
	Health(100.0f),
	MaxHealth(100.0f),
	AttackRange(300.0f),
	AcceptanceRange(150.0f),
	StrafeChance(0.3f),
	StrafeDelayMax(5.0f)
{
	// Right weapon collision box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), RightWeaponSocketName);
}

void AEnemy::EnterCombat()
{
	CurrentState = EAIState::Attack;
	// TODO: switch soundtrack
}

void AEnemy::ExitCombat()
{
	bIsWaiting = false;
	CurrentState = EAIState::Patrol;
	if (EnemyAIController != nullptr)
	{
		EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyAIController is null in ExitCombat()"));
	}
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

	// Can enemy patrol
	CurrentState = EAIState::Patrol;

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

			// Call reset melee attack 
			FTimerHandle TimerResetAttack;
			GetWorldTimerManager().SetTimer(TimerResetAttack, this, &AEnemy::ResetMeleeAttack, SectionLength, false);
		}
	}
}

void AEnemy::ResetMeleeAttack()
{
	float RandomChance = FMath:: FRand();
	if (RandomChance <= GetStrafeChance()) 
	{
		CurrentState = EAIState::Strafe;
	}
}

void AEnemy::ResetAttack()
{
	// this might be redundant
}

FName AEnemy::GetAttackSectionName(int32 SectionCount)
{
	const int32 Section = FMath::RandRange(1, SectionCount);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

void AEnemy::EnemyPatrol()
{
	if (PatrolStrategy.IsValid())
	{
		PatrolStrategy->Execute(this);
	}
	else
	{
		PatrolStrategy = NewObject<UPatrolStrategy>();
		PatrolStrategy->Execute(this);
	}
	
	bIsWaiting = false;
}

void AEnemy::EnemyAttack()
{
	if (!AttackStrategy.IsValid())
	{
		AttackStrategy = NewObject<UAttackStrategy>(this);
	}

	AttackStrategy->Execute(this);

	bIsWaiting = false;
}

void AEnemy::EnemyStrafe()
{
	bIsWaiting = false;
	CurrentState = EAIState::Attack;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	switch (CurrentState)
	{
	case EAIState::Attack:
		if (!bIsWaiting)
		{
			bIsWaiting = true;
			float AttackDelay = FMath::RandRange(0.75f, 2.0f);
			FTimerHandle AttackDelayTimer;
			GetWorldTimerManager().SetTimer(AttackDelayTimer, this, &AEnemy::EnemyAttack, AttackDelay, false);
		}
		break;
	case EAIState::Strafe:
		if (StrafeStrategy->HasReachedDestination(this) && !bIsWaiting)
		{
			bIsWaiting = true;
			if (StrafeStrategy.IsValid())
			{
				StrafeStrategy->Execute(this);
			}
			else
			{
				StrafeStrategy = NewObject<UStrafeStrategy>();
				StrafeStrategy->Execute(this);
			}
			
			float StrafeDelay = FMath::RandRange(2.0f, GetStrafeDelayMax());
			FTimerHandle StrafeDelayTimer;
			GetWorldTimerManager().SetTimer(StrafeDelayTimer, this, &AEnemy::EnemyStrafe, StrafeDelay, false);
			break;
		}
	case EAIState::Patrol:
		if (PatrolStrategy->HasReachedDestination(this) && !bIsWaiting)
		{
			bIsWaiting = true;
			float PatrolDelay = FMath::RandRange(1.0f, 5.0f);
			GetWorldTimerManager().SetTimer(PatrolDelayTimer, this, &AEnemy::EnemyPatrol, PatrolDelay, false);
		}
	}
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
		EnemyAIController->UnPossess();
		SetActorEnableCollision(false);
		EnemyDeath();
		// Todo: create a flag to either play a montage or ragdoll
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

