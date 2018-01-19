// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPlayerController.h"
#include "BattleTank.h"

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
		//UE_LOG(LogTemp, Warning, TEXT("HitLocation %s: "), *HitLocation.ToString());
		//TODO Tell controlled tank to aim at this point
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
		UE_LOG(LogTemp, Warning, TEXT("Look direction %s: "), *LookDirection.ToString());
	}

	//Line-trace along that direction, and see what we hit.
	return true;
}

bool ATankPlayerController::GetLookLocation(FVector2D ScreenLocation, FVector &LookDirection) const {
	FVector CameraWorldLocation; //to be discarded 
	return DeprojectScreenPositionToWorld(
		ScreenLocation.X, 
		ScreenLocation.Y, 
		CameraWorldLocation, 
		LookDirection);
}