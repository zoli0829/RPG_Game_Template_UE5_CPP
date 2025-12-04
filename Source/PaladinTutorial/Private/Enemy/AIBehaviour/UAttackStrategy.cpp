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

			EPathFollowingRequestResult::Type MoveResult = EnemyAIController->MoveToActor(PaladinCharacter, Enemy->AcceptanceRadius, true, true, true, nullptr, true);

			if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
			{
				UPathFollowingComponent* PathFollowingComponent = EnemyAIController->GetPathFollowingComponent();
				if (PathFollowingComponent)
				{
					PathFollowingComponent->OnRequestFinished.AddUObject(this, &UAttackStrategy::OnMoveCompleted, Enemy);
				}
			}
		}
	}
}

void UAttackStrategy::OnMoveCompleted(FAIRequestID, const FPathFollowingResult& Result, AEnemy* Enemy)
{
	if (Result.IsSuccess())
	{
		// TODO: Refactor add PaladinCharacter to parameters
		APawn* Pawn = UGameplayStatics::GetPlayerPawn(Enemy->GetWorld(), 0);
		APaladinCharacter* PaladinCharacter = Cast<APaladinCharacter>(Pawn);

		FVector PaladinPos = PaladinCharacter->GetActorLocation();
		FVector EnemyPos = Enemy->GetActorLocation();
		float Distance = FVector::Dist(EnemyPos, PaladinPos);

		// Make sure in range to attack player
		if (Distance <= Enemy->DistanceToAttackPlayer)
		{
			Enemy->MeleeAttack();
		}
	}
}
