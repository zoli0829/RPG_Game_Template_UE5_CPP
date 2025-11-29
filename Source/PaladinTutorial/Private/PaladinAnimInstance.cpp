// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinAnimInstance.h"

#include "PaladinCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Acts like tick for animations and get speed
void UPaladinAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (PaladinCharacter == nullptr)
	{
		PaladinCharacter = Cast<APaladinCharacter>(TryGetPawnOwner());
	}

	if (PaladinCharacter)
	{
		// Get speed of character from velocity
		FVector Velocity = PaladinCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = PaladinCharacter->GetCharacterMovement()->IsFalling();

		// Get offset yaw for player in blend spaces
		FRotator const AimRotation = PaladinCharacter->GetBaseAimRotation();
		FRotator const MovementRotation = UKismetMathLibrary::MakeRotFromX(PaladinCharacter->GetVelocity());

		Direction = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	}
}

// Acts like begin play to create reference to the character
void UPaladinAnimInstance::NativeInitializeAnimation()
{
	PaladinCharacter = Cast<APaladinCharacter>(TryGetPawnOwner());
}
