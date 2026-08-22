// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

class URunnerGameInstance;

UENUM(BlueprintType)
enum class ERunnerLane : uint8 {
	Left,
	Middle,
	Right
};

UCLASS(Blueprintable)
class FIRSTBLANKGAME_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	ERunnerLane CurrentLane;

	// The character's lateral (Y) position corresponding to the
	// Middle lane, captured once in BeginPlay.
	//
	// Left/Right lane target positions are computed relative to
	// this, so lane-switching works correctly no matter where
	// the character is initially placed in the level.
	float BaseY = 0.0f;


	// Cached reference to the GameInstance, which is the single
	// source of truth for LaneWidth (shared with ATrackManager
	// so obstacle placement always matches where lanes actually
	// are).
	UPROPERTY()
	TObjectPtr<URunnerGameInstance> RunnerGameInstance;


	// How quickly the character interpolates toward its target
	// lane position. Higher = snappier lane changes.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement",
		meta = (AllowPrivateAccess = "true")
	)
	float LaneChangeSpeed = 10.0f;


	// Controls how strongly the character is launched upward when jumping.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement",
		meta = (AllowPrivateAccess = "true")
	)
	float JumpZVelocity = 600.0f;


	// Controls how strongly gravity affects the character while airborne.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement",
		meta = (AllowPrivateAccess = "true")
	)
	float GravityScale = 1.0f;

	/**
	 * Percentage of the theoretical maximum jump height
	 * that we consider comfortably jumpable.
	 *
	 * For example, 0.8 means the player only needs to reach
	 * 80% of their maximum jump height.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Movement",
		meta = (AllowPrivateAccess = "true")
	)
	float ComfortableJumpHeightRatio = 0.8f;


	// Converts a lane enum into a Y offset relative to BaseY.
	//
	// Mirrors ATrackManager::GetLaneOffset() — see the LaneWidth
	// comment above for why these need to be kept in sync.
	float GetLaneOffset(ERunnerLane Lane) const;
public:
	// Sets default values for this character's properties
	ARunnerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
	void MoveLeft();

	UFUNCTION(BlueprintCallable)
	void MoveRight();

	virtual void Jump() override;

	UFUNCTION(BlueprintPure)
	ERunnerLane GetCurrentLane() const;

	UFUNCTION(BlueprintPure)
	float GetComfortableJumpHeight() const;

};
