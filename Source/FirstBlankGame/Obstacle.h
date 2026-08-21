// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS(Blueprintable)
class FIRSTBLANKGAME_API AObstacle : public AActor {
	GENERATED_BODY()
	
public:	
	AObstacle();

	/**
	 * Returns the height of the obstacle.
	 *
	 * This is measured from the obstacle's collision/mesh bounds.
	 */
	UFUNCTION(BlueprintPure)
	float GetObstacleHeight() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/**
	 * The visual mesh and collision for the obstacle.
	 *
	 * The actual Static Mesh can be assigned in a Blueprint
	 * child of AObstacle.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Obstacle")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;
};
