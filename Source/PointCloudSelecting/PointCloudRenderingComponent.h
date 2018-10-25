// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "Rpc.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PointCloud.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Guid.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "Runtime/Core/Public/Misc/StringFormatArg.h"
#include "PointCloudHelper.h"
#include "PointCloudActor.h"
#include "Engine/World.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "PointCloudRenderingComponent.generated.h"


UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFilterModeEnum : uint8
{
	FilterFloor 		UMETA(DisplayName = "NonFloor"),
	FilterNonFloating   UMETA(DisplayName = "Floating"),
	None  				UMETA(DisplayName = "Scaling")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFloatingSegmentColorMode : uint8
{
	Uniform				UMETA(DisplayName = "Uniform"),
	Mixed				UMETA(DisplayName = "Mixed"),
	None 				UMETA(DisplayName = "None"),
	Class				UMETA(DisplayName = "Class")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API UPointCloudRenderingComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	#pragma region [configuration]
	bool UseFancyFeatures = true;
	EFilterModeEnum FilterMode = EFilterModeEnum::FilterNonFloating;
	EFloatingSegmentColorMode FloatingSegmentColorMode = EFloatingSegmentColorMode::Mixed;
	FString PointCloudFile = TEXT("C:\\Users\\km\\Desktop\\MAG\\FloatingObjectFilter\\data\\459_100.txt");
	FString PointCloudClassFile = TEXT("C:\\Users\\km\\Desktop\\MAG\\FloatingObjectFilter\\data\\459_100class.txt");
	FString FloatingObjectFile = TEXT("C:\\Users\\km\\Desktop\\MAG\\FloatingObjectFilter\\data\\result459_100.pcd");
	FString ClassColorsFile = TEXT("C:\\Users\\km\\Desktop\\MAG\\FloatingObjectFilter\\data\\colormap.txt");
	#pragma endregion

	#pragma region [locals]
	APointCloudActor * PointCloudHostActor;
	TArray<FPointCloudPoint> FilteredPoints;
	TArray<FPointCloudPoint> LoadedPoints;
	UPointCloud* PointCloud;
	float MaxX;
	float MinY;
	float MinZ;

	// rbnn
	TArray<TArray<FString>> RbnnResults;
	double preferredFloatingObjectRadius = 1;
	int32 currentRbnnIndex;

	// class data
	TArray<int32> Classifications;
	TMap<int32, FVector> ClassToColorMap;
	#pragma endregion

public:	
	UPointCloudRenderingComponent();

protected:
	virtual void BeginPlay() override;

	

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public: // API

	UFUNCTION(BlueprintCallable)
	FString ProcessSelectedPoints(FVector& CenterInWorldSpace, FVector& BoundingBox);

	UFUNCTION(BlueprintCallable)
	void ChangeColorMode(EFloatingSegmentColorMode mode);

	UFUNCTION(BlueprintCallable)
	void ChangeFilterMode(EFilterModeEnum mode);

	UFUNCTION(BlueprintCallable)
	void ChangeRbnnIndex();


protected: // auxiliary

	void LoadAndPreparePoints();

	void RerenderPointCloud();

	void ColorPoints(TArray<FPointCloudPoint>& Points);
	void ColorPointsByClass(TArray<FPointCloudPoint>& Points);
	void ColorPointsUniform(TArray<FPointCloudPoint> & Points);
	void ColorPointsMixed(TArray<FPointCloudPoint> & Points);
	
	void LoadRbnnResults();
	void LoadClassifications();
	void LoadDesiredClassColors();

	void FilterPoints(TArray<FPointCloudPoint> & LoadedPoints);

	void FilterNonFloatingObjectPoints();

	void FilterFloorPoints();

	void SpaceTransformPCToLocal(TArray<FPointCloudPoint> &LoadedPoints);
	UPointCloud * PrepareRenderingSettings(TArray<FPointCloudPoint> &Points, FString pointCloudName, FString settingsName);
	void SpawnPointCloudHostActor(FTransform const &SpawningTransform);
	void GetPointCloudPoints(TArray<FPointCloudPoint> &LoadedPoints);
	TArray<FPointCloudPoint> LoadPointCloudFromFileTXT(FString filename, FVector2D RgbRange = FVector2D(0.0f, 256.0f * 256.0f - 1.0f));
	void FindExtremes(TArray<FPointCloudPoint> & LoadedPoints);
	void MarkSubsetWithinLoadedPoints(TArray<int32> &QueryResultIndices);
	TArray<FPointCloudPoint> GetPointSubset(TArray<int32> &QueryResultIndices);
	void FindSelectionIndices(FVector & CenterInWorldSpace, FVector & BoundingBox, TArray<int32> &QueryResultIndices);
	FString SelectedPointsToPointCloudTxtFormatString(TArray<FPointCloudPoint> PointsToSave);
};
