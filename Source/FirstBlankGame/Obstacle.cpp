// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"

// TODO: Eventually this should probably come from
// RunnerCharacter rather than being hardcoded here.
//
// This represents the maximum obstacle height that the
// character is capable of jumping over.
static constexpr float MaxJumpableObstacleHeight = 100.0f;

// Sets default values
AObstacle::AObstacle() {
	// Obstacles don't need to perform work every frame.
	PrimaryActorTick.bCanEverTick = false;

	/**
	 * Create the obstacle's mesh.
	 *
	 * The mesh provides both the visual representation and,
	 * depending on its collision settings, the physical
	 * collision for the obstacle.
	 */
	ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("ObstacleMesh")
	);

	/**
	 * Make the mesh the root component of the Actor.
	 *
	 * This means the Actor's transform directly controls
	 * the position, rotation and scale of the obstacle mesh.
	 */
	AActor::SetRootComponent(ObstacleMesh);

}

// Called when the game starts or when spawned
void AObstacle::BeginPlay() {
	Super::BeginPlay();
}


float AObstacle::GetObstacleHeight() const {
	/**
	 * Bounds.BoxExtent contains half of the bounding box's
	 * dimensions.
	 *
	 * Therefore, multiplying the Z extent by two gives
	 * the full height of the obstacle.
	 */
    const FVector BoxExtent = ObstacleMesh->Bounds.BoxExtent;
    return BoxExtent.Z * 2.0f;
}