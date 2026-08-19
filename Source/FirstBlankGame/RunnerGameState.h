// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "RunnerGameState.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTBLANKGAME_API ARunnerGameState : public AGameStateBase
{
	GENERATED_BODY()
	
private:
	int32 CurrentScore;
	int32 CurrentDistance;
	bool bGameOver = false;

public:
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

};
