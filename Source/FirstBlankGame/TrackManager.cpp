// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackManager.h"
#include "RunnerTrackTile.h"
#include "RunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle.h"
#include "URunnerGameInstance.h"

// Sets default values
ATrackManager::ATrackManager() {
    /**
	 * Unlike RunnerTrackTile, the TrackManager DOES need Tick.
	 *
	 * The manager needs to continuously update the track's
	 * position while the game is running.
	 */
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrackManager::BeginPlay() {
	// Always call the parent implementation first.
	Super::BeginPlay();

	// Get the character controlled by player 0 (the first/local player)
	RunnerCharacter = Cast<ARunnerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)
	);

	if (!RunnerCharacter) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: Could not find RunnerCharacter.")
		);
		return;
	}


	// Cache the GameInstance so GetLaneOffset() doesn't need to
	// look it up every time an obstacle row is spawned.
	RunnerGameInstance = Cast<URunnerGameInstance>(
		UGameplayStatics::GetGameInstance(this)
	);

	if (!RunnerGameInstance) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: Could not get URunnerGameInstance.")
		);
	}

	/**
	 * Create the initial track.
	 *
	 * For example, if InitialTileCount == 5:
	 *
	 *     Tile 1 → Tile 2 → Tile 3 → Tile 4 → Tile 5
	 */
	SpawnInitialTiles();
}

// Called every frame
void ATrackManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/**
	 * Every frame we move the active track.
	 *
	 * Example:
	 *
	 *     Frame 1:
	 *         Tile X = 1000
	 *
	 *     Frame 2:
	 *         Tile X = 990
	 *
	 *     Frame 3:
	 *         Tile X = 980
	 *
	 * The player doesn't need to move forward.
	 */
	MoveTiles(DeltaTime);

	/**
	 * After moving the tiles, check whether any have gone
	 * too far behind the player.
	 *
	 * Those tiles can safely be destroyed.
	 */
	RemoveOldTiles();

	/**
	 * TODO: Add something like:
	 *
	 *     SpawnNewTiles();
	 *
	 * here.
	 *
	 * We don't need to implement that until the basic
	 * tile spawning/movement system is working.
	 */
}


void ATrackManager::SpawnInitialTiles() {
	/**
     * We can't spawn anything if the TrackTileClass hasn't
     * been assigned.
     *
     * This would normally be assigned through the Unreal Editor.
     */
	if (!TrackTileClass) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: TrackTileClass is not assigned.")
		);
		return;
	}

	/**
	 * Create the requested number of starting tiles.
	 *
	 * SpawnTile() handles figuring out where each tile belongs.
	 */
	for (int32 i = 0; i < InitialTileCount; ++i)
	{
		SpawnTile();
	}
}


void ATrackManager::SpawnTile() {
	// Make sure there's a class to spawn
	if (!TrackTileClass) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: TrackTileClass is not assigned.")
		);
		return;
	}

	/**
	 * Get the current game world.
	 *
	 * SpawnActor() needs a UWorld to know which world the
	 * new Actor belongs to.
	 */
	UWorld* World = AActor::GetWorld();

	if (!World) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("RunnerTrackManager: Could not get World.")
		);
		return;
	}

	/**
	 * Decide where the new tile should be spawned.
	 *
	 * If this is the FIRST tile:
	 *
	 *     SpawnLocation = TrackManager's location
	 *
	 * If tiles already exist:
	 *
	 *     SpawnLocation = EndPoint of the last tile
	 */
	FVector SpawnLocation = AActor::GetActorLocation();

	if (ActiveTiles.Num() > 0) {
		/**
		 * Last() gives us the final tile in our array.
		 *
		 * This should be the tile furthest ahead on the track.
		 */
		ARunnerTrackTile* LastTile = ActiveTiles.Last();

		if (LastTile) {
			/**
			 * Get the last tile's endpoint location.
			 *
			 * The new tile will start exactly there.
			 */
			SpawnLocation = LastTile->GetEndPointLocation();
		}
	}


	/**
     * These parameters control how Unreal creates the Actor.
     *
     * We don't need any special spawn behavior yet,
     * so the default parameters are sufficient.
     */
	FActorSpawnParameters SpawnParameters;


	/**
	 * Actually create an instance of TrackTileClass in the world.
	 *
	 * TrackTileClass tells Unreal WHAT to spawn.
	 *
	 * SpawnLocation tells Unreal WHERE to spawn it.
	 *
	 * FRotator::ZeroRotator means we initially give it
	 * no rotation relative to the world.
	 */
	ARunnerTrackTile* NewTile = World->SpawnActor<ARunnerTrackTile>(
		TrackTileClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParameters
	);


	/**
	 * SpawnActor returns the newly-created Actor.
	 *
	 * It can return nullptr if spawning failed.
	 */
	if (!NewTile) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("RunnerTrackManager: Failed to spawn track tile.")
		);
		return;
	}

	/**
	 * Keep track of the new tile.
	 *
	 * This allows the TrackManager to later:
	 *
	 *     - Move it
	 *     - Check its position
	 *     - Destroy it
	 */
	ActiveTiles.Add(NewTile);

	/**
	 * Now that the tile exists in the world, populate it with
	 * however many obstacle rows fit along its length.
	 */
	SpawnObstaclesForTile(NewTile);

}


void ATrackManager::MoveTiles(float DeltaTime) {
	// const float RunSpeed = 500.0f;
	const float RunSpeed = RunnerGameInstance->GetRunSpeed();

	for (ARunnerTrackTile* Tile : ActiveTiles) {
		// A tile might've been destoryed or 
		// become invalid, so make sure the 
		// pointer is valid before using it.
		if (!Tile) continue;

		// Get the tile's current world position.
		FVector NewLocation = Tile->GetActorLocation();

		/**
		 * Move the tile backward along the X axis.
		 *
		 * DeltaTime is important here.
		 *
		 * Without DeltaTime, the movement would depend on
		 * the game's frame rate.
		 *
		 * With DeltaTime:
		 *
		 *     movement = speed × time
		 *
		 * So a 60 FPS and 120 FPS computer will move the
		 * track at approximately the same real-world speed.
		 */
		NewLocation.X -= RunSpeed * DeltaTime;

		// Apply the new position to the tile.
		Tile->SetActorLocation(NewLocation);
	}
}


void ATrackManager::RemoveOldTiles() {
	if (!RunnerCharacter) {
		UE_LOG(LogTemp, Error, TEXT("TrackManager: RunnerCharacter invalid in RemoveOldTiles"));
	}

	const float PlayerX = RunnerCharacter->GetActorLocation().X;

	/**
	 * Iterate BACKWARDS through the array.
	 *
	 * This is important because we're going to remove elements
	 * from ActiveTiles while iterating through it.
	 *
	 * Example:
	 *
	 *     [Tile 1, Tile 2, Tile 3]
	 *
	 * If we remove Tile 1, the indexes of the remaining elements
	 * change.
	 *
	 * Going backwards avoids skipping elements.
	 */
	for (int32 i = ActiveTiles.Num() - 1; i >= 0; i--) {
		ARunnerTrackTile* Tile = ActiveTiles[i];

		// If the pointer is invalid, remove it from the array
		if (!Tile) {
			ActiveTiles.RemoveAt(i);
			continue;
		}

		// Get the tile's current position
		const float TileX = Tile->GetActorLocation().X;
		
		/**
		 * If the tile has moved far enough behind the player,
		 * it is no longer needed.
		 *
		 * Example:
		 *
		 *     Player X = 0
		 *     DespawnDistance = 1000
		 *
		 *     Anything below X = -1000 can be destroyed.
		 */
		if (TileX < PlayerX - DespawnDistance) {
			// DestroyActor tells Unreal that this Actor
			// should be removed from the world.
			Tile->Destroy();

			// Remove the pointer from ActiveTiles array too.
			ActiveTiles.RemoveAt(i);
		}
	}
}


TArray<ERunnerLane> ATrackManager::GetAllLanes() {
	return {
		ERunnerLane::Left,
		ERunnerLane::Middle,
		ERunnerLane::Right
	};
}


TArray<FObstacleSpawnData> ATrackManager::GenerateObstacleRow() {
	TArray<FObstacleSpawnData> Row;

	// We need a valid RunnerCharacter to determine
	// whether an obstacle can be comfortably jumped.
	if (!RunnerCharacter) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: RunnerCharacter is null when generating obstacle row.")
		);
		return Row;
	}


	// We need at least one obstacle option to choose from.
	if (ObstacleOptions.IsEmpty()) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: ObstacleOptions is empty.")
		);
		return Row;
	}


	// Keep track of which lanes are still available.
	TArray<ERunnerLane> AvailableLanes = GetAllLanes();
	const int32 LaneCount = AvailableLanes.Num();

	if (LaneCount == 0) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: No lanes are available.")
		);
		return Row;
	}

	const int32 MaxObstaclesPerRow = FMath::Min(3, LaneCount);
	const int32 ObstacleCount = FMath::RandRange(1, MaxObstaclesPerRow);
	const float ComfortableJumpHeight = RunnerCharacter->GetComfortableJumpHeight();

	/**
	 * If every lane is going to contain an obstacle,
	 * there must be at least one obstacle that the player
	 * can comfortably jump over.
	 *
	 * Find all obstacle definitions that satisfy that
	 * requirement.
	 */
	TArray<int32> JumpableObstacleIndices;
	for (int32 i = 0; i < ObstacleOptions.Num(); ++i) {
		const FObstacleDefinition& Definition = ObstacleOptions[i];

		if (Definition.Height <= ComfortableJumpHeight) {
			JumpableObstacleIndices.Add(i);
		}
	}

	if (ObstacleCount == LaneCount && JumpableObstacleIndices.IsEmpty()) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: Cannot generate a safe obstacle row. "
				"All lanes would be blocked, but no obstacle is "
				"comfortably jumpable.")
		);
		return {};
	}


	// If every lane is to have an obstacle, make sure at least 1 can be
	// jumped over
	if (ObstacleCount == LaneCount) {
		const int32 JumpableOptionIndex = JumpableObstacleIndices[
			FMath::RandRange(
				0, JumpableObstacleIndices.Num() - 1
			)
		];

		const int32 LaneIndex = FMath::RandRange(0, AvailableLanes.Num());
		const ERunnerLane Lane = AvailableLanes[LaneIndex];

		AvailableLanes.RemoveAt(LaneIndex);

		FObstacleSpawnData SpawnData;
		SpawnData.ObstacleClass = ObstacleOptions[JumpableOptionIndex].ObstacleClass;
		SpawnData.Lane = Lane;

		Row.Add(SpawnData);
	}


	// Assign the rest of the obstacles
	while (Row.Num() < ObstacleCount) {
		const int32 LaneIndex = FMath::RandRange(0, AvailableLanes.Num() - 1);
		const ERunnerLane Lane = AvailableLanes[LaneIndex];

		AvailableLanes.RemoveAt(LaneIndex);

		const int32 ObstacleIndex = FMath::RandRange(0, ObstacleOptions.Num() - 1);
		const FObstacleDefinition& Definition = ObstacleOptions[ObstacleIndex];

		FObstacleSpawnData SpawnData;
		SpawnData.ObstacleClass = Definition.ObstacleClass;
		SpawnData.Lane = Lane;

		Row.Add(SpawnData);
	}

	return Row;
}


float ATrackManager::GetLaneOffset(ERunnerLane Lane) const {
	// Fall back to 0 (Middle) if the GameInstance isn't available
	// for some reason, rather than crashing.
	if (!RunnerGameInstance) {
		return 0.0f;
	}

	const float LaneWidth = RunnerGameInstance->GetLaneWidth();

	switch (Lane) {
		case ERunnerLane::Left:
			return -LaneWidth;

		case ERunnerLane::Right:
			return LaneWidth;

		case ERunnerLane::Middle:
		default:
			return 0.0f;
	}
}


void ATrackManager::SpawnObstacleRow(
	ARunnerTrackTile* Tile, const FVector& RowLocation
) {
	if (!Tile) {
		UE_LOG(LogTemp, Error, TEXT("TrackManager: Tile not valid."));
		return;
	}

	UWorld* World = AActor::GetWorld();

	if (!World) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: Could not get World in SpawnObstacleRow.")
		);
		return;
	}

	const TArray<FObstacleSpawnData> SpawnDataRow = GenerateObstacleRow();

	for (const FObstacleSpawnData& SpawnData : SpawnDataRow) {
		if (!SpawnData.ObstacleClass) continue;

		/**
		 * Offset the row's base location sideways according to
		 * the obstacle's assigned lane.
		 *
		 * NOTE: this assumes lanes are offset along Y and that
		 * the tile isn't rotated relative to the world. If tiles
		 * can curve/rotate, this should instead offset along the
		 * tile's right vector (Tile->GetActorRightVector()).
		 */
		FVector ObstacleLocation = RowLocation;
		ObstacleLocation.Y += GetLaneOffset(SpawnData.Lane);

		FActorSpawnParameters SpawnParameters;

		AObstacle* NewObstacle = World->SpawnActor<AObstacle>(
			SpawnData.ObstacleClass,
			ObstacleLocation,
			FRotator::ZeroRotator,
			SpawnParameters
		);

		if (!NewObstacle) {
			UE_LOG(
				LogTemp,
				Error,
				TEXT("TrackManager: Failed to spawn obstacle.")
			);
		}
	}
	
}


void ATrackManager::SpawnObstaclesForTile(ARunnerTrackTile* Tile) {
	if (!Tile) {
		UE_LOG(LogTemp, Error, TEXT("TrackManager: Tile not valid."));
		return;
	}

	if (ObstacleRowSpacing <= 0.0f) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("TrackManager: ObstacleRowSpacing must be greater than 0.")
		);
		return;
	}

	const float TileLength = Tile->GetTrackLength();

	if (TileLength < ObstacleStartOffset + ObstacleEndOffset) {
		UE_LOG(LogTemp, Error, TEXT("TrackManager: Not enough room for even a single row"));
		return;
	}

	const FVector TileOrigin = Tile->GetActorLocation();
	const FVector TileForward = Tile->GetActorForwardVector();
	const float UsableLength = TileLength - ObstacleEndOffset;

	/**
	 * Walk along the tile from ObstacleStartOffset to
	 * (TileLength - ObstacleEndOffset), placing one row every
	 * ObstacleRowSpacing units.
	 *
	 * Example, with TileLength = 2000, ObstacleStartOffset = 500,
	 * ObstacleEndOffset = 500, ObstacleRowSpacing = 600:
	 *
	 *     Row 1 @ 500
	 *     Row 2 @ 1100
	 *     (next would be 1700, but UsableLength is 1500, so we stop)
	 */
	for (
		float DistanceAlongTile = ObstacleStartOffset;
		DistanceAlongTile <= UsableLength;
		DistanceAlongTile += ObstacleRowSpacing
	) {
		const FVector RowLocation = TileOrigin + (TileForward * DistanceAlongTile);

		SpawnObstacleRow(Tile, RowLocation);
	}
}