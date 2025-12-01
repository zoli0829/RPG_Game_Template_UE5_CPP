// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PaladinCharacter.h"

AEnemyAIController::AEnemyAIController()
{
	SetupPerceptionSystem();
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// Add a small delay to ensure the pawn is fully initialized to avoid issues with spawned enemies during runtime
	FTimerHandle TimerPawnInit;
	GetWorld()->GetTimerManager().SetTimer(TimerPawnInit, this, &AEnemyAIController::SetupControlledPawn, 0.1f, false);
}

void AEnemyAIController::SetupControlledPawn()
{
	AEnemy* Enemy = Cast<AEnemy>(GetPawn());
	if (Enemy != nullptr)
	{
		ControlledEnemy = Enemy;
	}
	else
	{
		// If we get this error then the Timer need more time, so just increase it from 0.1f
		UE_LOG(LogTemp, Error, TEXT("AEnemyAIController::SetupControlledPawn: No controlled pawn found!!!"));
	}
}

void AEnemyAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
		SetPerceptionComponent(*PerceptionComponent);

		SightConfig->SightRadius = 1500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius * 2; // might be  too much
		SightConfig->PeripheralVisionAngleDegrees = 120.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 1550.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void AEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const PaladinCharacter = Cast<APaladinCharacter>(Actor))
	{
		if (Stimulus.IsActive())
		{
			if (ControlledEnemy != nullptr)
			{
				//ControlledEnemy->EnterCombat();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, TEXT("Paladin detected."));
			}
		}
		else
		{
			if (ControlledEnemy != nullptr)
			{
				//ControlledEnemy->ExitCombat();
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Exit combat."));
			}
		}
	}
}
