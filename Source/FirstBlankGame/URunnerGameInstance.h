// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "URunnerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTBLANKGAME_API URunnerGameInstance : public UGameInstance {
	GENERATED_BODY()

private:
	int32 RunSpeed;
	const int32 MaxRunSpeed = 1000;
	const int32 MinRunSpeed = 10;

	// Distance, in Unreal units, between the Middle lane and the
	// Left/Right lanes.
	//
	// This is the SINGLE source of truth for lane spacing — both
	// ARunnerCharacter (to know where to stand) and ATrackManager
	// (to know where to place obstacles) read this value rather
	// than each keeping their own copy, so they can never drift
	// out of sync with each other.
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Track",
		meta = (AllowPrivateAccess = "true")
	)
	float LaneWidth = 300.0f;
public:
	virtual void Init() override;

	UFUNCTION(BlueprintPure)
	int32 GetRunSpeed() const;

	UFUNCTION(BlueprintPure)
	int32 GetMaxRunSpeed() const;

	UFUNCTION(BlueprintPure)
	int32 GetMinRunSpeed() const;

	bool SetRunSpeed(int32 NewRunSpeed);

	UFUNCTION(BlueprintPure)
	float GetLaneWidth() const;
	
};
