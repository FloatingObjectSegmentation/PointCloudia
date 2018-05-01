// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointCloud.h"
#include "PointCloudHelper.h"
#include "PointCloudActor.h"
#include "Engine/World.h"
#include "PointCloudRenderingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API UPointCloudRenderingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPointCloudRenderingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPointCloud* PreparePointCloud(TArray<FPointCloudPoint> &LoadedPoints);

	void SpawnPointCloudHostActor(FTransform const &SpawningTransform);

	void LoadPointCloudPointsFromFile(TArray<FPointCloudPoint> &LoadedPoints);

	void NormalizePointLocations(TArray<FPointCloudPoint> &LoadedPoints);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	APointCloudActor * PointCloudHostActor;
};
