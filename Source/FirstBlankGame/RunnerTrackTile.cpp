// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerTrackTile.h"

// Sets default values
ARunnerTrackTile::ARunnerTrackTile() {
	/**
	* Track tiles themselves don't need to perform work every frame.
	*
	* The TrackManager is responsible for moving the tiles.
	*
	* Therefore, we can disable Tick for this Actor.
	*/
	PrimaryActorTick.bCanEverTick = false;

	/**
	* Create the root component.
	*
	* Every Actor has a root component. Other components can be
	* attached to it so that they move together with the Actor.
	*/
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	// Tell Unreal that Root is the main/root component of this Actor.
	SetRootComponent(Root);


	/**
	 * Create the visible track mesh.
	 *
	 * CreateDefaultSubobject creates this component when the
	 * TrackTile Actor is created.
	 *
	 * The actual Static Mesh can later be assigned in the
	 * Unreal Editor.
	 */
	TrackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrackMesh"));
	// Attach the mesh to the root.
	//
	// This means that if the tile moves, the mesh moves with it.
	TrackMesh->SetupAttachment(Root);

	/**
	 * Create the endpoint marker.
	 *
	 * This is just a SceneComponent used as a convenient
	 * position marker. It doesn't need a visible mesh.
	 */
	EndPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndPoint"));
	// Attach the endpoint to the root so its position is relative
	// to the track tile.
	EndPoint->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ARunnerTrackTile::BeginPlay() {
	// Always call the parent implementation when overriding
	// an Unreal lifecycle function.
	Super::BeginPlay();
}


FVector ARunnerTrackTile::GetEndPointLocation() const {
	/**
	 * GetComponentLocation() returns the endpoint's location
	 * in WORLD SPACE.
	 *
	 * This is important because the TrackManager needs to know
	 * where in the world to spawn the next tile.
	 */
	return EndPoint->GetComponentLocation();
}


float ARunnerTrackTile::GetTrackLength() const {
	/**
	 * EndPoint's RELATIVE location gives us the tile's length
	 * along its local X (forward) axis.
	 *
	 * Using the relative (rather than world) location means this
	 * works correctly no matter where the tile currently sits or
	 * how it's rotated in the world.
	 */
	return EndPoint->GetRelativeLocation().X;
}
