// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RunnerPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class FIRSTBLANKGAME_API ARunnerPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARunnerPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	void MoveLeft(const FInputActionValue& value);
	void MoveRight(const FInputActionValue& value);
	void Jump(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> RunnerMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveLeftAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> JumpAction;

};
