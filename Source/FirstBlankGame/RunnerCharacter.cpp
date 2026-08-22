// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "URunnerGameInstance.h"

// Sets default values
ARunnerCharacter::ARunnerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Configure the character's jump.
	GetCharacterMovement()->JumpZVelocity = this->JumpZVelocity;
	GetCharacterMovement()->GravityScale = this->GravityScale;

	CurrentLane = ERunnerLane::Middle;
}

// Called when the game starts or when spawned
void ARunnerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Record the character's starting lateral position as the
	// Middle-lane baseline. Left/Right lane targets are computed
	// relative to this in Tick().
	BaseY = AActor::GetActorLocation().Y;

	// Cache the GameInstance so GetLaneOffset() doesn't need to
	// look it up every frame.
	RunnerGameInstance = Cast<URunnerGameInstance>(GetGameInstance());

	if (!RunnerGameInstance) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("RunnerCharacter: Could not get URunnerGameInstance.")
		);
	}
}

// Called every frame
void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/**
	 * Smoothly move the character toward its target lane.
	 *
	 * MoveLeft()/MoveRight() only update CurrentLane — the
	 * actual movement happens here, every frame, so the
	 * character glides between lanes instead of teleporting.
	 */
	const FVector CurrentLocation = GetActorLocation();
	const float TargetY = BaseY + GetLaneOffset(CurrentLane);

	const float NewY = FMath::FInterpTo(
		CurrentLocation.Y,
		TargetY,
		DeltaTime,
		LaneChangeSpeed
	);

	// Only touch Y. X and Z are left alone so we don't fight
	// with CharacterMovementComponent's handling of gravity,
	// jumping, and (eventually) forward motion.
	SetActorLocation(
		FVector(CurrentLocation.X, NewY, CurrentLocation.Z),
		/* bSweep */ true
	);
}

// Called to bind functionality to input
void ARunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARunnerCharacter::MoveLeft() {
	switch (CurrentLane) {
		case ERunnerLane::Left:
			return;
		case ERunnerLane::Right:
			CurrentLane = ERunnerLane::Middle;
			break;
		case ERunnerLane::Middle:
			CurrentLane = ERunnerLane::Left;
			break;
	}

	return;
}

void ARunnerCharacter::MoveRight() {
	switch (CurrentLane) {
	case ERunnerLane::Left:
		CurrentLane = ERunnerLane::Middle;
		break;
	case ERunnerLane::Right:
		return;
	case ERunnerLane::Middle:
		CurrentLane = ERunnerLane::Right;
		break;
	}

	return;
}

void ARunnerCharacter::Jump() {
	Super::Jump();
}

ERunnerLane ARunnerCharacter::GetCurrentLane() const {
	return CurrentLane;
}

float ARunnerCharacter::GetComfortableJumpHeight() const {
	/**
	* Approximate maximum height of the jump is:
	* 
	*	h = (velocity**2) / (2 * gravity)
	*/

	const float Gravity = FMath::Abs(
		GetCharacterMovement()->GetGravityZ()
	);

	const float MaxJumpHeight =
		FMath::Square(JumpZVelocity) / (2.0f * Gravity);

	return MaxJumpHeight * ComfortableJumpHeightRatio;
}



float ARunnerCharacter::GetLaneOffset(ERunnerLane Lane) const {
	// Fall back to 0 (no offset / stay in current lane visually)
	// if the GameInstance isn't available for some reason, rather
	// than crashing.
	if (!RunnerGameInstance) {
		return 0.0f;
	}

	const float LaneWidth = RunnerGameInstance->GetLaneWidth();

	switch (Lane) {
		case ERunnerLane::Left:
			return -LaneWidth;

		case ERunnerLane::Right:
			return LaneWidth;

		case ERunnerLane::Middle:
		default:
			return 0.0f;
	}
}