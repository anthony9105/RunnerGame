// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle.h"
#include "RunnerCharacter.h"
#include "RunnerGameState.h"

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
	 * Create a plain, empty root component.
	 *
	 * ObstacleMesh attaches to this rather than being the root
	 * itself — see the comment on Root in the header for why.
	 */
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	AActor::SetRootComponent(Root);

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

	// Attach the mesh to Root, rather than making it the root
	// component, so its Location/Rotation can be freely adjusted
	// per Blueprint variant.
	ObstacleMesh->SetupAttachment(Root);

	/**
	 * Configure collision so we detect the character touching
	 * this obstacle without physically blocking it.
	 *
	 * We use Overlap rather than Block because the track scrolls
	 * obstacles toward a stationary character via direct location
	 * updates (not physics), so a hard physical block would be
	 * unreliable here. Overlap events give us a clean, explicit
	 * "the character touched this" signal instead.
	 */
	ObstacleMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ObstacleMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ObstacleMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ObstacleMesh->SetGenerateOverlapEvents(true);

}

// Called when the game starts or when spawned
void AObstacle::BeginPlay() {
	Super::BeginPlay();

	/**
	 * Subscribe OnMeshBeginOverlap() to ObstacleMesh's overlap
	 * delegate, so Unreal calls it automatically whenever
	 * ObstacleMesh starts overlapping another component.
	 *
	 * OnComponentBeginOverlap is a multicast delegate that every
	 * UPrimitiveComponent has built in — it's a list of functions
	 * Unreal will call when an overlap begins. AddDynamic() adds
	 * our function to that list.
	 *
	 * "Dynamic" means this delegate works via Unreal's reflection
	 * system (rather than being a plain C++ function pointer).
	 * That's also why OnMeshBeginOverlap() has to be marked
	 * UFUNCTION() in the header — AddDynamic() requires it.
	 *
	 * Once bound, whenever ObstacleMesh overlaps something (e.g.
	 * the character's capsule, since we set that collision
	 * response to Overlap above), Unreal calls:
	 *
	 *     OnMeshBeginOverlap(OverlappedComponent, OtherActor, ...)
	 *
	 * filling in all the parameters itself — OverlappedComponent
	 * will be ObstacleMesh, OtherActor will be whatever touched
	 * it, etc. This binding is per-instance: each spawned obstacle
	 * gets its own independent subscription, tied to `this`.
	 */
	ObstacleMesh->OnComponentBeginOverlap.AddDynamic(
		this,
		&AObstacle::OnMeshBeginOverlap
	);
}


void AObstacle::OnMeshBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
) {
	// We only care about the RunnerCharacter touching this obstacle.
	ARunnerCharacter* RunnerCharacter = Cast<ARunnerCharacter>(OtherActor);
	if (!RunnerCharacter) {
		return;
	}

	ARunnerGameState* RunnerGameState = GetWorld()
		? GetWorld()->GetGameState<ARunnerGameState>()
		: nullptr;

	if (!RunnerGameState) {
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Obstacle: Could not get ARunnerGameState.")
		);
		return;
	}

	RunnerGameState->SetGameOver(true);
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