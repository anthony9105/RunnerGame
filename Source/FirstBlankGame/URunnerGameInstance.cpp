// Fill out your copyright notice in the Description page of Project Settings.


#include "URunnerGameInstance.h"


void URunnerGameInstance::Init() {
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("Runner Game Instance initialized"));
}


int32 URunnerGameInstance::GetRunSpeed() const {
	return RunSpeed;
}


int32 URunnerGameInstance::GetMinRunSpeed() const {
	return MinRunSpeed;
}


int32 URunnerGameInstance::GetMaxRunSpeed() const {
	return MaxRunSpeed;
}


bool URunnerGameInstance::SetRunSpeed(int32 NewRunSpeed) {
	if (NewRunSpeed > MaxRunSpeed) return false;
	if (NewRunSpeed < MinRunSpeed) return false;

	RunSpeed = NewRunSpeed;
	return true;
}


