// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunnerTrackTile.generated.h"

/**
 * Represents one individual section/tile of the endless runner track.
 *
 * A tile is responsible for representing the physical piece of track.
 * It does NOT decide when it should be spawned, moved, or destroyed.
 *
 * Those responsibilities belong to ARunnerTrackManager.
 *
 * Example:
 *
 *     Tile 1          Tile 2          Tile 3
 *   ┌─────────┐     ┌─────────┐     ┌─────────┐
 *   │         │     │         │     │         │
 *   │  Track  │ --> │  Track  │ --> │  Track  │
 *   │         │     │         │     │         │
 *   └─────────┘     └─────────┘     └─────────┘
 *        │
 *        └── EndPoint tells the manager where Tile 2 should start.
 */
UCLASS()
class FIRSTBLANKGAME_API ARunnerTrackTile : public AActor {
	GENERATED_BODY()
	
public:	
	ARunnerTrackTile();

	/**
	 * Returns the world-space location of this tile's endpoint.
	 *
	 * The TrackManager uses this location when spawning the
	 * next track tile.
	 */
	FVector GetEndPointLocation() const;


	/**
	 * Returns the length of this tile along its local forward
	 * (X) axis, measured from the tile's origin to its EndPoint.
	 *
	 * Because this is based on EndPoint's RELATIVE location,
	 * the result is independent of the tile's current world
	 * position or rotation.
	 *
	 * The TrackManager uses this to figure out how many obstacle
	 * rows can fit along the tile.
	 */
	float GetTrackLength() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// The root component of the track tile.
	//
	// Components attached to this component will move with the tile.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TrackMesh;


	// The visible physical surface of the track.
	//
	// The actual static mesh can be assigned to this component
	// in the Unreal Editor.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;


	// A marker that indicates where the next track tile should begin.
	//
	// This is NOT another piece of geometry.
	// It is simply a SceneComponent whose transform gives us a
	// convenient position and rotation.
	//
	// For example:
	//
	//     ┌──────────────────┐
	//     │    Track Tile    │
	//     │                  │
	//     └──────────────────┘
	//                        ●
	//                        ↑
	//                     EndPoint
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EndPoint;
};
