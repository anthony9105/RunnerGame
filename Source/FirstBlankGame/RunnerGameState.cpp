// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameState.h"
#include "URunnerGameInstance.h"

/**
* GETTERS
*/
int32 ARunnerGameState::GetCurrentScore() const {
	return CurrentScore;
}

int32 ARunnerGameState::GetCurrentDistance() const {
	return CurrentDistance;
}

bool ARunnerGameState::GetGameOver() const {
	return bGameOver;
}


/**
* SETTERS
*/
bool ARunnerGameState::SetCurrentScore(int32 NewCurrentScore) {
	if (NewCurrentScore < 0) return false;

	this->CurrentScore = NewCurrentScore;
	return true;
}

bool ARunnerGameState::SetCurrentDistance(int32 NewCurrentDistance) {
	if (NewCurrentDistance < 0) return false;

	this->CurrentDistance = NewCurrentDistance;
	return true;
}


void ARunnerGameState::SetGameOver(bool NewGameOver) {
	// Remember whether we were already in game-over, so we only
	// broadcast on the actual transition — not every time this
	// gets called with true again (e.g. if the character somehow
	// overlaps two obstacles in the same frame).
	const bool bWasGameOver = this->bGameOver;

	this->bGameOver = NewGameOver;

	if (NewGameOver && !bWasGameOver) {
		URunnerGameInstance* RunnerGameInstance = Cast<URunnerGameInstance>(GetGameInstance());

		if (RunnerGameInstance) {
			RunnerGameInstance->UpdateHighScore(CurrentScore);
		}
		else {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("RunnerGameState: Could not get URunnerGameInstance to update high score.")
			);
		}

		//// The * operator converts the resulting FString into the TCHAR* format required by %s
		//FString Message = FString::Printf(TEXT("%s"), NewGameOver ? TEXT("True") : TEXT("False"));

		//UE_LOG(LogTemp, Warning, TEXT("Broadcasting on game over.  New game over: %s"), *Message);

		OnGameOver.Broadcast();
	}
}


void ARunnerGameState::AddDistance(float DeltaDistance) {
	// Ignore zero/negative deltas rather than letting distance
	// accidentally tick backward.
	if (DeltaDistance <= 0.0f) {
		return;
	}

	DistanceAccumulator += DeltaDistance;

	const int32 NewDistance = FMath::FloorToInt(DistanceAccumulator);
	SetCurrentDistance(NewDistance);

	/**
	 * No coin/collectible system exists yet, so score is simply
	 * treated as equal to distance for now — this keeps the Game
	 * Over screen's score field meaningful instead of sitting at
	 * 0 forever.
	 *
	 * TODO: once a real scoring mechanic (coins, combos, etc.)
	 * exists, replace this with actual point accumulation instead
	 * of mirroring distance.
	 */
	SetCurrentScore(NewDistance);
}