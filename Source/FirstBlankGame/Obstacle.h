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

	/**
	 * Called when something begins overlapping ObstacleMesh.
	 *
	 * If the overlapping actor is the RunnerCharacter, this
	 * triggers game over.
	 *
	 * Because this is overlap-based rather than a physical block,
	 * jumping over the obstacle naturally avoids triggering it —
	 * as long as ObstacleMesh's collision matches its visual
	 * height, a character whose capsule is above the mesh when
	 * passing through won't overlap it at all.
	 */
	UFUNCTION()
	void OnMeshBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

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
