// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerGameState.h"

int32 ARunnerGameState::GetCurrentScore() const {
	return CurrentScore;
}


int32 ARunnerGameState::GetCurrentDistance() const {
	return CurrentDistance;
}


bool ARunnerGameState::GetGameOver() const {
	return bGameOver;
}


