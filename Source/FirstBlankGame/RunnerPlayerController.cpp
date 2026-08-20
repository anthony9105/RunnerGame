// Fill out your copyright notice in the Description page of Project Settings.


#include "RunnerPlayerController.h"
#include "RunnerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

namespace {
    constexpr const TCHAR* GetPawnErrorMsg =
        TEXT("RunnerPlayerController: couldn't get the pawn "
            "currently controlled by this controller and cast to "
            "RunnerCharacter");
}


ARunnerPlayerController::ARunnerPlayerController() {
}


/**
 * Called when the PlayerController begins playing.
 *
 * Use this to activate our Input Mapping Context.
 *
 * The Mapping Context contains the actual keyboard/controller
 * bindings that map physical inputs to Input Actions.
 *
 * Example:
 *     A / Left Arrow → IA_MoveLeft
 *     D / Right Arrow → IA_MoveRight
 *     Space → IA_Jump
 */
void ARunnerPlayerController::BeginPlay() {
    // call the parent implementation first
    Super::BeginPlay();

    // Get the LocalPlayer associated with this controller.
    //
    // A LocalPlayer represents a player on this machine.
    // For a normal single-player game, there will typically
    // only be one.
    ULocalPlayer* LocalPlayer = GetLocalPlayer();
    if (!LocalPlayer) {
        UE_LOG(LogTemp, Error, TEXT("RunnerPlayerController: LocalPlayer is null."));
        return;
    }

    // Get the Enhanced Input subsystem for this player.
    //
    // The subsystem is responsible for managing which
    // Input Mapping Contexts are currently active.
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem) {
        UE_LOG(LogTemp, Error, TEXT("RunnerPlayerController: InputSubsystem is null."));
        return;
    }

    // Make sure we actually have a Mapping Context assigned.
    if (!RunnerMappingContext) {
        UE_LOG(LogTemp, Error, TEXT("RunnerPlayerController: RunnerMappingContext is null/not assigned."));
        return;
    }

    // Activate our runner's input mappings.
    //
    // The second argument (0) is the priority.
    // Higher priority mapping contexts can override
    // lower priority ones when necessary.
    InputSubsystem->AddMappingContext(RunnerMappingContext, 0);
}


/**
 * Called by Unreal to set up the input component for this
 * PlayerController.
 *
 * Here we connect our Input Actions to our C++ functions.
 *
 * For example:
 *
 *     IA_MoveLeft
 *          ↓
 *     MoveLeft()
 *          ↓
 *     RunnerCharacter->MoveLeft()
 */
void ARunnerPlayerController::SetupInputComponent() {
    // let APlayerController perform its normal setup first
    Super::SetupInputComponent();

    // InputComponent is the component that receives input.
    //
    // EnhancedInputComponent is the Enhanced Input version
    // that allows us to bind UInputAction assets.
    UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(AActor::InputComponent);

    // If the cast failed, we don't have an Enhanced Input Component,
    // so we can't bind our Enhanced Input Actions.
    if (!EnhancedInputComponent) {
        UE_LOG(
            LogTemp, 
            Error, 
            TEXT("RunnerPlayerController: cast"
                " failed, don't have an enhanced input component so the"
                " enhanced input actions can't be binded")
        );
        return;
    }

    // When IA_MoveLeft is triggered, call our MoveLeft() function.
    EnhancedInputComponent->BindAction(
        MoveLeftAction,
        ETriggerEvent::Triggered,
        this,
        &ARunnerPlayerController::MoveLeft
    );

    // When IA_MoveRight is triggered, call our MoveRight() function.
    EnhancedInputComponent->BindAction(
        MoveRightAction,
        ETriggerEvent::Triggered,
        this,
        &ARunnerPlayerController::MoveRight
    );

   // When IA_Jump starts, call our Jump() function.
   //
   // "Started" is useful here because we want one jump when
   // the player presses the button, rather than continuously
   // triggering while the button is held.
    EnhancedInputComponent->BindAction(
        JumpAction,
        ETriggerEvent::Started,
        this,
        &ARunnerPlayerController::Jump
    );
}



/**
 * Called when the player activates the Move Left Input Action.
 *
 * The PlayerController doesn't actually perform the movement.
 * Instead, it tells the currently controlled RunnerCharacter
 * to move left.
 */
void ARunnerPlayerController::MoveLeft(const FInputActionValue& Value) {
    // GetPawn() returns the Pawn currently controlled by
    // this PlayerController.
    //
    // Cast it to our specific RunnerCharacter type so we can
    // call RunnerCharacter-specific functions.
    ARunnerCharacter* RunnerCharacter = Cast<ARunnerCharacter>(AController::GetPawn());

    if (!RunnerCharacter) {
        UE_LOG(LogTemp, Error, TEXT("%s"), GetPawnErrorMsg);
        return;
    }

    // Tell the character to move one lane to the left.
    RunnerCharacter->MoveLeft();
}


/**
 * Called when the player activates the Move Right Input Action.
 */
void ARunnerPlayerController::MoveRight(const FInputActionValue& Value) {
    ARunnerCharacter* RunnerCharacter = Cast<ARunnerCharacter>(AController::GetPawn());

    if (!RunnerCharacter) {
        UE_LOG(LogTemp, Error, TEXT("%s"), GetPawnErrorMsg);
        return;
    }

    // Tell the character to move one lane to the right.
    RunnerCharacter->MoveRight();
}


/**
 * Called when the player activates the Jump Input Action.
 */
void ARunnerPlayerController::Jump(const FInputActionValue& Value) {
    // Get the character controlled by this PlayerController.
    ARunnerCharacter* RunnerCharacter = Cast<ARunnerCharacter>(GetPawn());
    if (!RunnerCharacter) {
        UE_LOG(LogTemp, Error, TEXT("%s"), GetPawnErrorMsg);
        return;
    }

    // ARunnerCharacter inherits Jump() from ACharacter.
    //
    // We don't need to implement the actual jumping logic
    // here. ACharacter already handles it.
    RunnerCharacter->Jump();
}