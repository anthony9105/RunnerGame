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
	this->bGameOver = NewGameOver;
}