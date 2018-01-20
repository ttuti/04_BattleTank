// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "BattleTank.h"
#include "Engine/World.h"

ATank* ATankPlayerController::GetControlledTank() const {
	return Cast<ATank>(GetPawn());
}

void ATankPlayerController::BeginPlay() {
	Super::BeginPlay();
	auto ControlledTank = GetControlledTank();
	if (!ControlledTank) {
		UE_LOG(LogTemp, Error, TEXT("Player controller not possessing a tank"));
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Tank possessed: %s"), *(ControlledTank->GetName()));
	}
}

void ATankPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	AimTowardsCrosshair();	
}

void ATankPlayerController::AimTowardsCrosshair() {
	if (!GetControlledTank()) { return; }

	FVector HitLocation; //OUT parameter, doesn't need initialisation
	if (GetSightRayHitLocation(HitLocation)) { //Has side-effect: its going to line trace
		UE_LOG(LogTemp, Warning, TEXT("Hit location %s: "), *HitLocation.ToString());
		GetControlledTank()->AimAt(HitLocation);
	}
	
}

//Get world location if linetrace through the crosshair
//true if it hits landscape
bool ATankPlayerController::GetSightRayHitLocation(FVector &HitLocation) const {
	//Find the crosshair position
	int32 ViewportSizeX, ViewpointSizeY;
	GetViewportSize(ViewportSizeX, ViewpointSizeY);
	auto ScreenLocation = FVector2D(CrossHairXLocation * ViewportSizeX, CrossHairYLocation * ViewpointSizeY);
	
	//De-project the screen position of the crosshair to a world direction
	FVector LookDirection;
	if (GetLookLocation(ScreenLocation, LookDirection)) {
		//Line-trace along that direction, and see what we hit.
		GetLookVectorHitLocation(LookDirection, HitLocation);
		return true;
	}
	return false;
}

bool ATankPlayerController::GetLookLocation(FVector2D ScreenLocation, FVector &LookDirection) const {
	FVector CameraWorldLocation; //to be discarded 
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X, 
		ScreenLocation.Y, 
		CameraWorldLocation, 
		LookDirection);
}

bool ATankPlayerController::GetLookVectorHitLocation(FVector LookDirection, FVector &HitLocation) const {
	FHitResult HitResult;
	auto StartLocation = PlayerCameraManager->GetCameraLocation();
	auto EndLocation = StartLocation + (LookDirection * LineTraceRange);

	if (GetWorld()->LineTraceSingleByChannel(
			OUT HitResult,
			StartLocation,
			EndLocation,
			ECollisionChannel::ECC_Visibility)
		){ //line trace succeeds
		HitLocation=HitResult.Location;
		return true;
	}
	HitLocation = FVector(0.f);
	return false;
}

