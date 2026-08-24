// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameState.h"

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