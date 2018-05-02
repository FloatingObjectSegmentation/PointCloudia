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

private:
	APointCloudActor * PointCloudHostActor;
	TArray<FPointCloudPoint> LoadedPoints;
	UPointCloud* PointCloud;
	float MaxX;
	float MinY;
	float MinZ;

public:	
	UPointCloudRenderingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public: // API

	UFUNCTION(BlueprintCallable)
	void QueryForRegion(FVector& CenterInWorldSpace, FVector& BoundingBox);

	void SavePoints(TArray<FPointCloudPoint> PointsToSave);

protected: // auxiliary
	void SpaceTransformPCToLocal(TArray<FPointCloudPoint> &LoadedPoints);
	UPointCloud * PrepareRenderingSettings(TArray<FPointCloudPoint> &LoadedPoints);
	void SpawnPointCloudHostActor(FTransform const &SpawningTransform);
	void LoadPointsFromFile(TArray<FPointCloudPoint> &LoadedPoints);
	void FindExtremes(TArray<FPointCloudPoint> & LoadedPoints);
};
