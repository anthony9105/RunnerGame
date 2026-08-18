// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "URunnerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTBLANKGAME_API URunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	int32 RunSpeed;
	const int32 MaxRunSpeed = 1000;
	const int32 MinRunSpeed = 10;
public:
	virtual void Init() override;

	UFUNCTION(BlueprintPure)
	int32 GetRunSpeed() const;

	UFUNCTION(BlueprintPure)
	int32 GetMaxRunSpeed() const;

	UFUNCTION(BlueprintPure)
	int32 GetMinRunSpeed() const;

	bool SetRunSpeed(int32 NewRunSpeed);
	
};
