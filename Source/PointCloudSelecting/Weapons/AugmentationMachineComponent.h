// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL IMPORTS
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// PROJECT IMPORTS
#include "RieglLMSQ780.h"
#include "../FloatingObject.h"
#include "../FloatingObjectSpawner.h"

// MANDATORY IMPORT
#include "AugmentationMachineComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class POINTCLOUDSELECTING_API UAugmentationMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAugmentationMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FindFloatingObjectSpawnerInWorld();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
		void StartScanning(FVector airplaneLocation, FRotator airplaneOrientation, FVector objectLocation, FVector objectScaleInMeters, EAugmentationObject object, float minRbnnR);

public:
	UStaticMesh * AirplaneStaticMesh;
	float SpawnDistance = 200.0f;
	AActor* Airplane;
	URieglLMSQ780* ScannerComponent;
	AActor* AugmentedObject;

	// the augmentable objects
	AFloatingObjectSpawner* Spawner;

	float MinRbnnR;
	EAugmentationObject ObjectType;

private:
	void LoadAirplaneStaticMesh();
	void FindAirplaneMesh(TArray<AActor *> &FoundActors);
	void ExtractStaticMeshFromActor(TArray<AActor *> & FoundActors, const int32 &i);
	AActor* SpawnAirplane(FVector Location, FRotator Orientation, FVector Scale);
	void SetAirplaneAttributes();


};
