// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "RieglLMSQ780.h"
#include "AugmentationMachineComponent.generated.h"

UENUM(BlueprintType)
enum class EAugmentationObject : uint8
{
	Sphere						UMETA(DisplayName = "Sphere"),
	Cube						UMETA(DisplayName = "Cube")
};

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

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UFUNCTION(BlueprintCallable)
		void StartScanning(FVector airplaneLocation, FRotator airplaneOrientation, FVector objectLocation, EAugmentationObject object, float minRbnnR);

public:
	UStaticMesh * AirplaneStaticMesh;
	float SpawnDistance = 200.0f;
	AActor* Airplane;
	URieglLMSQ780* ScannerComponent;
	AActor* AugmentedObject;

	// the augmentable objects
	UStaticMesh * SphereStaticMesh;

	float MinRbnnR;
	EAugmentationObject ObjectType;

private:
	void LoadAirplaneStaticMesh();
	void LoadAugmentableMeshes();
	void FindTemplateMesh(TArray<AActor *> &FoundActors);
	void ExtractStaticMeshFromActor(TArray<AActor *> & FoundActors, const int32 &i);
	AActor* SpawnAirplane(FVector Location, FRotator Orientation);
	void SpawnObject(FVector Location, FRotator Orientation, EAugmentationObject type);
	void SetAugmentedObjectAttributes(AActor& object, EAugmentationObject type);
	void GetBoundingBoxSpawnTransform(FTransform &SpawningTransform);
	void SetBoundingBoxAttributes();


};
