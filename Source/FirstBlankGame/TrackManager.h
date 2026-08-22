// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackManager.generated.h"


// Forward declaration of the lane enum (defined in RunnerCharacter.h).
//
// We only need the TYPE here, not its enumerators, so a forward
// declaration avoids a circular include with RunnerCharacter.h
// (which includes TrackManager.h).
enum class ERunnerLane : uint8;


// Forward declaration.
//
// We only need to know that ARunnerTrackTile is an Unreal Actor
// in this header. Its full definition will be included in the .cpp.
class ARunnerTrackTile;
class ARunnerCharacter;
class AObstacle;


/**
 * Describes an obstacle that should be spawned in a row.
 *
 * This represents spawn information, not an actual AObstacle Actor.
 */
USTRUCT(BlueprintType)
struct FObstacleSpawnData {
	GENERATED_BODY()

	// The type of obstacle that should be spawned.
	UPROPERTY()
	TSubclassOf<AObstacle> ObstacleClass;

	// The lane where the obstacle should be placed.
	UPROPERTY()
	ERunnerLane Lane;
};



// This describes an available obstacle type before anything is spawned.
USTRUCT(BlueprintType)
struct FObstacleDefinition {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AObstacle> ObstacleClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Height = 0.0f;
};


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
class FIRSTBLANKGAME_API ATrackManager : public AActor {
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


	UPROPERTY(
		EditDefaultsOnly, BlueprintReadOnly, 
		Category = "Obstacles", meta = (AllowPrivateAccess = "true")
	)
	TArray<FObstacleDefinition> ObstacleOptions;


	// Minimum distance between consecutive obstacle rows.
	UPROPERTY(EditDefaultsOnly, Category = "Obstacles")
	float ObstacleRowSpacing = 600.0f;


	// Distance from the beginning of a tile before
	// the first obstacle row can be spawned.
	UPROPERTY(EditDefaultsOnly, Category = "Obstacles")
	float ObstacleStartOffset = 500.0f;


	// Distance from the end of a tile after which
	// we stop spawning obstacle rows.
	UPROPERTY(EditDefaultsOnly, Category = "Obstacles")
	float ObstacleEndOffset = 500.0f;


	// Distance, in Unreal units, between the center of one lane
	// and the center of the track (Middle lane).
	//
	// Left lane   = -LaneWidth
	// Middle lane =  0
	// Right lane  = +LaneWidth
	UPROPERTY(EditDefaultsOnly, Category = "Obstacles")
	float LaneWidth = 300.0f;
	
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


	/**
	 * Generates a safe configuration of obstacles
	 * for one row of the track.
	 *
	 * The returned data describes what should be spawned
	 * and in which lane.
	 */
	TArray<FObstacleSpawnData> GenerateObstacleRow();


	/**
	 * Spawns one row of obstacles (as described by GenerateObstacleRow)
	 * at RowLocation, offsetting each obstacle sideways according to
	 * its assigned lane.
	 */
	void SpawnObstacleRow(
		ARunnerTrackTile* Tile,
		const FVector& RowLocation
	);


	/**
	 * Walks the length of a newly-spawned tile and spawns as many
	 * obstacle rows as fit, spaced ObstacleRowSpacing apart, starting
	 * ObstacleStartOffset from the tile's beginning and stopping
	 * ObstacleEndOffset before the tile's end.
	 */
	void SpawnObstaclesForTile(ARunnerTrackTile* Tile);


	// Converts a lane enum into a world-space Y offset relative
	// to the track's center line.
	float GetLaneOffset(ERunnerLane Lane) const;


	static TArray<ERunnerLane> GetAllLanes();

};
