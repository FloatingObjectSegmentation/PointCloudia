// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "PointCloud.h"
#include "../FloatingObject.h"
#include "RieglLMSQ780.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POINTCLOUDSELECTING_API URieglLMSQ780 : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URieglLMSQ780();
	AActor* augmentedObject;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void RecalcRotationMatrix();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// set augmented object
	void SetAugmentedObject(AActor* floatingObject) {
		augmentedObject = floatingObject;
	}

	// external components
	int RerenderInterval = 5;
	int RerenderCounter = 0;


	// hyperparameters
	int StepsPerFrame = 100;
	FVector StartingPoint;
	float SpeedX = 46.3f; // [m/s]
	int SpeedAngular = 54; // [lines/s]
	int Frequency = 400000; // [Hz]
	int PhiMaxDegrees = 30; // [deg]
	float MaxDistanceTraveled = 50.0f; // [m]

									   // storage
	TArray<FVector> Points;
	TArray<float> Intensities;

private:

	// parameters
	float CurrentDistanceTraveled = 0.0f; // [m]
	float PhiMaxRadians;
	float Alpha;
	FMatrix RotationMatrix;
	FMatrix FrameMatrix;
	float dPoint;
	FVector Down;
	FVector Direction;
	FVector DirectionIntermediate;
	FVector CurrentPoint;

};
