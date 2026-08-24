// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RunnerGameState.generated.h"


// Declares a multicast delegate type with no parameters.
//
// "Multicast" means multiple listeners can bind to it — unlike
// AObstacle's OnComponentBeginOverlap binding (a single AddDynamic
// call on a delegate the engine already defined), here WE are
// defining a brand new delegate type for our own game-specific event.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);

/**
 * 
 */
UCLASS()
class FIRSTBLANKGAME_API ARunnerGameState : public AGameStateBase {
	GENERATED_BODY()
	
private:
	int32 CurrentScore;
	int32 CurrentDistance;
	bool bGameOver = false;

	// Running total of distance traveled, accumulated as a float
	// so fractional per-frame movement isn't lost to int32
	// truncation before it's reflected in CurrentDistance. This
	// is an internal implementation detail — callers just use
	// AddDistance(); nothing outside this class needs to know
	// distance is tracked as a float under the hood.
	float DistanceAccumulator = 0.0f;

public:
	/**
	 * Broadcast once, the moment the game transitions into
	 * game-over (see SetGameOver()).
	 *
	 * BlueprintAssignable means Blueprints can bind to this too —
	 * e.g. a Game Over UMG widget can listen for this event
	 * directly instead of polling GetGameOver() every tick.
	 */
	UPROPERTY(BlueprintAssignable, Category = "GameState")
	FOnGameOver OnGameOver;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentScore() const;

	UFUNCTION(BlueprintPure)
	int32 GetCurrentDistance() const;

	UFUNCTION(BlueprintPure)
	bool GetGameOver() const;

	UFUNCTION(BlueprintCallable)
	bool SetCurrentScore(int32 NewCurrentScore);

	UFUNCTION(BlueprintCallable)
	bool SetCurrentDistance(int32 NewCurrentDistance);

	UFUNCTION(BlueprintCallable)
	void SetGameOver(bool NewGameOver);


	/**
	 * Advances distance (and, for now, score — see the .cpp) by
	 * DeltaDistance units.
	 *
	 * This is the ONLY way distance should be advanced — it owns
	 * the float-accumulation-to-avoid-truncation logic internally,
	 * so callers (e.g. ATrackManager) just report how far the
	 * track moved this frame and don't need to know or duplicate
	 * that detail themselves.
	 */
	UFUNCTION(BlueprintCallable)
	void AddDistance(float DeltaDistance);

};
