#include "Enemy/AIBehaviour/UAttackStrategy.h"
#include "PaladinCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyAIController.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UAttackStrategy::UAttackStrategy() {}

void UAttackStrategy::Execute(AEnemy* Enemy)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0);
	APaladinCharacter* PaladinCharacter = Cast<APaladinCharacter>(Pawn);

	if (PaladinCharacter)
	{
		AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(Enemy->GetController());

		if (EnemyAIController)
		{
			// Set focus on the player
			EnemyAIController->SetFocus(PaladinCharacter);

			// Check player and enemy distance for acceptance range to attack
			if (PlayerEnemyDistance(Enemy, PaladinCharacter) <= Enemy->GetAcceptanceRange())
			{
				Enemy->Attack();
			}

			EPathFollowingRequestResult::Type MoveResult = EnemyAIController->MoveToActor(PaladinCharacter, Enemy->GetAcceptanceRange(), true, true, true, nullptr, true);

			if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
			{
				UPathFollowingComponent* PathFollowingComponent = EnemyAIController->GetPathFollowingComponent();
				if (PathFollowingComponent)
				{
					PathFollowingComponent->OnRequestFinished.AddUObject(this, &UAttackStrategy::OnMoveCompleted, Enemy, PaladinCharacter);
				}
			}
		}
	}
}

void UAttackStrategy::OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy, APaladinCharacter* PaladinCharacter)
{
	if (Result.IsSuccess())
	{
		// Make sure in range to attack player
		if (PlayerEnemyDistance(Enemy, PaladinCharacter) <= Enemy->GetAttackRange())
		{
			Enemy->Attack();
		}
	}
}

float UAttackStrategy::PlayerEnemyDistance(AEnemy* Enemy, APaladinCharacter* PaladinCharacter)
{
	FVector PaladinPos = PaladinCharacter->GetActorLocation();
	FVector EnemyPos = Enemy->GetActorLocation();
	float Distance = FVector::Dist(EnemyPos, PaladinPos);

	return Distance;
}
