// Fill out your copyright notice in the Description page of Project Settings.


#include "PaladinCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PaladinAnimInstance.h"
#include "HitInterface.h"
#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Sight.h"
#include "Sound/SoundCue.h"

// Sets default values
APaladinCharacter::APaladinCharacter() :
	WalkSpeed(300.0f),
	RunSpeed(600.0f),
	BaseDamage(20.0f),
	Health(100.0f),
	MaxHealth(100.0f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create camera boom
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	// Create follow Camera
	FollowCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	FollowCameraComponent->bUsePawnControlRotation = false;

	// Jump settings
	GetCharacterMovement()->JumpZVelocity = 400.0f;
	GetCharacterMovement()->AirControl = 0.1f;

	// Right weapon collision box
	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Box"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("SwordSocket"));

	// Stimulus
	SetupStimulusSource();

	// Motion warping component
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warp Component"));
}

// Called when the game starts or when spawned
void APaladinCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EPlayerState::Ready;

	// Add input mapping content
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get local player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Add input context
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	// Bind function to overlap for weapon box
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &APaladinCharacter::OnRightWeaponOverlap);

	// Setup right weapon collision
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	
}

void APaladinCharacter::MotionWarpAttack(float AttackDistance, FName MotionWarpName)
{
	const FVector Start = GetActorLocation();
	const FVector End = Start + GetActorForwardVector() * AttackDistance;
	FHitResult HitResult;

	// Set up collision query params to ignore the player
	FCollisionQueryParams TraceParams(FName(TEXT("AttackTrace")), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	// Perform  line trace
	ECollisionChannel Ecc_Channel = ECC_Pawn;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Ecc_Channel, TraceParams))
	{
		AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy && MotionWarpingComponent)
		{
			if (HitResult.bBlockingHit && HitResult.GetActor() == Enemy)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit Enemy"));
				MotionWarpingComponent->AddOrUpdateWarpTargetFromLocation(MotionWarpName, HitResult.Location);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Enemy is null or MotionWarpingComponent is null!"));
		}

		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1, 0, 1);
	}
}

void APaladinCharacter::ResetWarpAttack()
{
	MotionWarpingComponent->RemoveAllWarpTargets();
}

void APaladinCharacter::Move(const FInputActionValue& InputValue)
{
	FVector2d InputVector = InputValue.Get<FVector2d>();

	if (IsValid(Controller))
	{
		// Get forward direction
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Rotation matrix
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis((EAxis::Y));

		// Add movement input
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}

void APaladinCharacter::Look(const FInputActionValue& InputValue)
{
	FVector2d InputVector = InputValue.Get<FVector2d>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
	}
}

void APaladinCharacter::Jump()
{
	// Call parent class jump function
	Super::Jump();

	if (GetCharacterMovement()->IsMovingOnGround())
	{
		// Get current forward velocity
		FVector ForwardVelocity = GetVelocity();
		ForwardVelocity.Z = 0;

		// Define jump vertical velocity
		float JumpVerticalVelocity = GetCharacterMovement()->JumpZVelocity;

		// Combine current forward velocity with jump vertical velocity
		FVector JumpVelocity = ForwardVelocity + FVector(0, 0, JumpVerticalVelocity);

		// Launch character with the combined velocity
		LaunchCharacter(JumpVelocity, true, true);
	}
}

void APaladinCharacter::Running()
{
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void APaladinCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APaladinCharacter::BasicAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack1"), 1.25f);
}

void APaladinCharacter::HeavyAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack2"), 1.25f);
}

void APaladinCharacter::SpinAttack()
{
	AnimMontagePlay(AttackMontage, FName("Attack3"), 1.25f);
}

void APaladinCharacter::JumpAttack()
{
	MotionWarpAttack(1000, "Attack4");
	AnimMontagePlay(AttackMontage, FName("Attack4"), 1.25f);
}

void APaladinCharacter::DodgeBack()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Dodged back"));
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeBack"));
}

void APaladinCharacter::DodgeLeft()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Dodged left"));
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeLeft"));
}

void APaladinCharacter::DodgeRight()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Dodged right"));
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeRight"));
}

void APaladinCharacter::DodgeForward()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Dodged forward"));
	CurrentState = EPlayerState::BlockDodge;
	AnimMontagePlay(DodgeMontage, FName("DodgeForward"));
}

void APaladinCharacter::StartBlocking()
{
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		// If you want to stop the player from moving when blocking uncomment the DisableMovement() and the StopBlocking() from below below
		//GetCharacterMovement()->DisableMovement();

		CurrentState = EPlayerState::BlockDodge;
		AnimInstance->SetIsBlocking(true);
	}
}

void APaladinCharacter::StopBlocking()
{
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		//GetCharacterMovement()->SetMovementMode(MOVE_Walking); // uncomment if you want to stop the player from moving

		CurrentState = EPlayerState::Ready;
		AnimInstance->SetIsBlocking(false);
	}
}

void APaladinCharacter::AnimMontagePlay(UAnimMontage* MontageToPlay, FName SectionName, float PlayRate)
{
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance && MontageToPlay)
	{
		// Check if montage is playing
		if (!AnimInstance->Montage_IsPlaying(MontageToPlay))
		{
			PlayAnimMontage(MontageToPlay, PlayRate, SectionName);
		}

		if (MontageToPlay == AttackMontage && SectionName == "Attack4")
		{
			FTimerHandle WarpTimer;
			GetWorldTimerManager().SetTimer(WarpTimer, this, &APaladinCharacter::ResetWarpAttack, 2.0f, false);
		}
	}
}

void APaladinCharacter::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(SweepResult.GetActor()) && SweepResult.GetActor() != this)
	{
		IHitInterface* HitInterface = Cast<IHitInterface>(SweepResult.GetActor());

		if (HitInterface)
		{
			HitInterface->HitInterface_Implementation(SweepResult);
		}
		
		// Apply damage to enemy
		UGameplayStatics::ApplyDamage(
		SweepResult.GetActor(),
		BaseDamage,
		GetController(),
		this,
		UDamageType::StaticClass()
		);
	}
}

bool APaladinCharacter::PlayerFacingActor(AActor* FacingActor)
{
	// Getting the dot product 
	FVector PlayerDirection = GetActorForwardVector();
	FVector ActorDirection = (FacingActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	// if the float is + then the actors are facing each other
	float DotProduct = FVector::DotProduct(PlayerDirection, ActorDirection);
	
	if (DotProduct > 0)
	{
		return true;
	}

	return false;
}

void APaladinCharacter::SetupStimulusSource()
{
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus Source"));
	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimuliSource->RegisterWithPerceptionSystem();
	}
}

// Called every frame
void APaladinCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APaladinCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement actions
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Move);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Jump);
		Input->BindAction(RunAction, ETriggerEvent::Triggered, this, &APaladinCharacter::Running);
		Input->BindAction(RunAction, ETriggerEvent::Completed, this, &APaladinCharacter::StopRunning);

		// Blocking actions
		Input->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APaladinCharacter::StartBlocking);
		Input->BindAction(BlockAction, ETriggerEvent::Completed, this, &APaladinCharacter::StopBlocking);

		// Attack actions
		Input->BindAction(BasicAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::BasicAttack);
		Input->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::HeavyAttack); // or Triggered, whichever feels better
		Input->BindAction(SpinAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::SpinAttack);
		Input->BindAction(JumpAttackAction, ETriggerEvent::Completed, this, &APaladinCharacter::JumpAttack);

		// Dodge actions
		Input->BindAction(DodgeBackAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeBack);
		Input->BindAction(DodgeLeftAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeLeft);
		Input->BindAction(DodgeRightAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeRight);
		Input->BindAction(DodgeForwardAction, ETriggerEvent::Triggered, this, &APaladinCharacter::DodgeForward);
	}
}

void APaladinCharacter::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void APaladinCharacter::DeactivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float APaladinCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	UPaladinAnimInstance* AnimInstance = Cast<UPaladinAnimInstance>(GetMesh()->GetAnimInstance());
	// If the player is not blocking
	if (CurrentState != EPlayerState::BlockDodge)
	{
		if (Health - DamageAmount <= 0)
		{
			Health = 0.0f;
			
			// Play impact niagara effect
			const FVector SpawnLocation = GetMesh()->GetBoneLocation(BloodImpactBoneLocation, EBoneSpaces::WorldSpace);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodParticles, SpawnLocation, GetActorRotation());
			
			// Play hit SFX
			if (BodyImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, BodyImpactSound, GetActorLocation());
			}

			// TODO: Play death sound
			
			DeathOfPlayer();
		}
		else
		{
			// TODO: Play hit animation
			Health -= DamageAmount;

			// Play impact niagara effect
			const FVector SpawnLocation = GetMesh()->GetBoneLocation(BloodImpactBoneLocation, EBoneSpaces::WorldSpace);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodParticles, SpawnLocation, GetActorRotation());
			
			// Play hit SFX
			if (BodyImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, BodyImpactSound, GetActorLocation());
			}
		}
	}
	// Is blocking == true
	else
	{
		// Check if player is facing the enemy -> Run dot product logic
		if (PlayerFacingActor(DamageCauser))
		{
			// Play BlockSFX
			if (ShieldImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ShieldImpactSound, GetActorLocation());
			}

			// Play impact niagara effect
			const FVector SpawnLocation = GetMesh()->GetBoneLocation(ShieldImpactBoneLocation, EBoneSpaces::WorldSpace);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShieldBlockingParticles, SpawnLocation, GetActorRotation());
			
			// TODO: Play block impact animation maybe
		}
		else
		{
			Health -= DamageAmount;
			
			// Play HitSFX
			if (BodyImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, BodyImpactSound, GetActorLocation());
			}

			// Play impact niagara effect
			const FVector SpawnLocation = GetMesh()->GetBoneLocation(BloodImpactBoneLocation, EBoneSpaces::WorldSpace);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodParticles, SpawnLocation, GetActorRotation());
			
			// TODO: Play hit animation
		}
	}

	
	return DamageAmount;
}

