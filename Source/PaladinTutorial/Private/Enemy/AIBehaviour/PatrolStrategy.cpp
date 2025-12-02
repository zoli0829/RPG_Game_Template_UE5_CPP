#include "Enemy/AIBehaviour/PatrolStrategy.h"

PatrolStrategy::PatrolStrategy()
{
}

void PatrolStrategy::Execute(AEnemy* Enemy)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("Inside Execute Patrol Strategy"));

	// Add logic so enemy can patrol
}
