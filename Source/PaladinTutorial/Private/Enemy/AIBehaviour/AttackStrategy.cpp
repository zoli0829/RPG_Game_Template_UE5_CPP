#include "Enemy/AIBehaviour/AttackStrategy.h"

AttackStrategy::AttackStrategy()
{
}

void AttackStrategy::Execute(AEnemy* Enemy)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString("Inside Execute Attack Strategy"));

	// Move to player and attack
}
