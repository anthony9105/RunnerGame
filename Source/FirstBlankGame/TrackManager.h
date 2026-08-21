// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackManager.generated.h"

// Forward declaration.
//
// We only need to know that ARunnerTrackTile is an Unreal Actor
// in this header. Its full definition will be included in the .cpp.
class ARunnerTrackTile;
class ARunnerCharacter;

/**
 * Manages the endless runner's track.
 *
 * The player character does NOT move forward.
 *
 * Instead, this manager moves the track tiles toward the player.
 *
 * The manager is responsible for:
 *
 *     1. Creating the initial track tiles.
 *     2. Moving the active tiles toward the player.
 *     3. Removing tiles that have gone behind the player.
 *     4. Creating new tiles ahead of the player.
 *
 * Conceptually:
 *
 *                         Player
 *                           │
 *                           ▼
 *
 *                    ┌─────────────┐
 *                    │    Tile 3   │
 *                    └─────────────┘
 *                    ┌─────────────┐
 *                    │    Tile 2   │
 *                    └─────────────┘
 *                    ┌─────────────┐
 *                    │    Tile 1   │
 *                    └─────────────┘
 *
 * The tiles continuously move toward the player.
 *
 * When a tile gets too far behind:
 *
 *                    Tile 1
 *                       ↓
 *                    DESTROY
 *
 * When the player gets close to the end of the active track:
 *
 *                    Player
 *                       │
 *                       ▼
 *                    Tile 3
 *                       │
 *                       ▼
 *                    Spawn Tile 4
 */
UCLASS()
class FIRSTBLANKGAME_API ATrackManager : public AActor
{
	GENERATED_BODY()
	
public:
	ATrackManager();

	// Called every frame.
	//
	// The manager uses Tick to continuously :
	//
	//     - Move the track
	//     - Remove old tiles
	//     - Spawn new tiles
	//
	// DeltaTime is the amount of time that has passed since
	// the previous frame.
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the TrackManager begins playing.
	virtual void BeginPlay() override;

private:
	// The Blueprint/C++ class that should be used when creating
	// new track tiles.
	//
	// TSubclassOf means this stores a CLASS, rather than an
	// individual track tile object.
	//
	// For example, the editor could assign:
	//
	//     BP_RunnerTrackTile
	//
	// The manager can then spawn instances of that class.
	//
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARunnerTrackTile> TrackTileClass;


    // All track tiles that currently exist in the world.
    //
    // Example:
    //
    //     ActiveTiles
    //         ├── Tile 1
    //         ├── Tile 2
    //         ├── Tile 3
    //         └── Tile 4
    //
    // When a tile is destroyed, it should also be removed
    // from this array.
	UPROPERTY()
	TArray<TObjectPtr<ARunnerTrackTile>> ActiveTiles;


	// Number of tiles that should exist when the game starts.
	//
	// For example, if this is 5:
	//
	//     Tile 1 → Tile 2 → Tile 3 → Tile 4 → Tile 5
	//
	// are created at the beginning of the run.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Track")
	int32 InitialTileCount = 5;


	// How far ahead of the player we want the track to extend.
	//
	// Once the active track gets too close to the player,
	// the manager will create another tile.
	//
	// This prevents the player from reaching the end of the
	// generated track.
	//
	UPROPERTY(EditDefaultsOnly, Category = "Track")
	float SpawnDistance = 3000.0f;


	// How far behind the player a tile can travel before
	// the manager destroys it.
	//
	// Destroying old tiles prevents the game from continuously
	// accumulating Actors in memory during an endless run.
	UPROPERTY(EditDefaultsOnly, Category = "Track")
	float DespawnDistance = 1000.0f;


	// The player character whose position we use to determine
	// when track tiles should be spawned or destroyed.
	UPROPERTY()
	TObjectPtr<ARunnerCharacter> RunnerCharacter;

	
    // Creates the initial set of track tiles when the game starts.
	void SpawnInitialTiles();


	// Creates one new track tile.
	//
	// If there are already tiles:
	//
	//     New tile location = EndPoint of the last tile
	//
	// This allows tiles of different lengths to connect
	// without hardcoding their dimensions.
	void SpawnTile();


	// Moves every active track tile toward the player.
	//
	// The player stays approximately in the same forward position.
	// The track creates the illusion that the player is running.
	void MoveTiles(float DeltaTime);


	// Checks whether any tiles have moved far enough behind
	// the player to be removed.
	void RemoveOldTiles();

};
