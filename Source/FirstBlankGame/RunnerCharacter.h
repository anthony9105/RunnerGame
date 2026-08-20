// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RunnerCharacter.generated.h"

UENUM(Blueprintable)
enum class ERunnerLane : uint8 {
	Left,
	Middle,
	Right
};

UCLASS(Blueprintable)
class FIRSTBLANKGAME_API ARunnerCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	ERunnerLane CurrentLane;
public:
	// Sets default values for this character's properties
	ARunnerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable)
	void MoveLeft();

	UFUNCTION(BlueprintCallable)
	void MoveRight();

	virtual void Jump() override;

	UFUNCTION(BlueprintPure)
	ERunnerLane GetCurrentLane() const;

};
