// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	
}

// Called every frame
void ARunnerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

	// TODO: apply the movement

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

	// TODO: apply the movement

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