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
	 * Plain, empty root component, separate from the mesh.
	 *
	 * This exists so ObstacleMesh can be repositioned/rotated per
	 * obstacle Blueprint variant (e.g. to correct an imported
	 * mesh's off-center pivot or wrong facing direction) via its
	 * own Location/Rotation — a ROOT component's transform IS the
	 * actor's transform and can't be independently offset the same
	 * way a child's can, since TrackManager always spawns obstacles
	 * with a fixed rotation in C++.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Obstacle")
	TObjectPtr<USceneComponent> Root;

	/**
	 * The visual mesh and collision for the obstacle.
	 *
	 * The actual Static Mesh can be assigned in a Blueprint
	 * child of AObstacle.
	 */
	UPROPERTY(VisibleAnywhere, Category = "Obstacle")
	TObjectPtr<UStaticMeshComponent> ObstacleMesh;
};