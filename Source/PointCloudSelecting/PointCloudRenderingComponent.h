// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// CPP IMPORTS
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "Rpc.h"

// UNREAL IMPORTS
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Runtime/Core/Public/Misc/Guid.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "Runtime/Core/Public/Misc/StringFormatArg.h"
#include "Engine/World.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/Misc/Paths.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

// EXTERNAL IMPORTS
#include "PointCloudHelper.h"
#include "PointCloudActor.h"
#include "PointCloud.h"

// PROJECT IMPORTS
#include "Weapons/AugmentationMachineComponent.h"
#include "FloatingObject.h"

#include "PointCloudRenderingComponent.generated.h"



UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EFilterModeEnum : uint8
{
	FilterFloor 		UMETA(DisplayName = "NonFloor"),
	FilterNonFloating   UMETA(DisplayName = "Floating"),
	None  				UMETA(DisplayName = "Scaling")
};

UENUM(BlueprintType)
enum class EFloatingSegmentColorMode : uint8
{
	Uniform				UMETA(DisplayName = "Uniform"),
	Mixed				UMETA(DisplayName = "Mixed"),
	None 				UMETA(DisplayName = "None"),
	Class				UMETA(DisplayName = "Class"),
	Intensity			UMETA(DisplayName = "Intensity")
};

UENUM(BlueprintType)
enum class EFloatingObjectLabel : uint8
{
	Floating					UMETA(DisplayName = "Floating"),
	LikelyFloating				UMETA(DisplayName = "LikelyFloating"),
	MaybeFloating 				UMETA(DisplayName = "MaybeFloating"),
	NotFloating					UMETA(DisplayName = "NotFloating")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EAugmentationMode : uint8
{
	NoAugmentation 					UMETA(DisplayName = "NoAugmentation"),
	Augmentation    				UMETA(DisplayName = "Augmentation"),
	RenderAugmentationOnly  	    UMETA(DisplayName = "RenderAugmentationOnly")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API UPointCloudRenderingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EFilterModeEnum FilterMode = EFilterModeEnum::None;
	EFloatingSegmentColorMode FloatingSegmentColorMode = EFloatingSegmentColorMode::None;
	EAugmentationMode AugmentationMode = EAugmentationMode::Augmentation;

private:

	#pragma region [configuration]
	int time = 0;
	bool UseFancyFeatures = true;
	bool AugmentationInProgress = false;

	FString PointCloudFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\lidar\\449_121.txt");
	FString PointCloudClassFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\lidar\\449_121class.txt");
	FString PointCloudIntensityFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\lidar\\449_121intensity.txt");
	FString FloatingObjectFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\lidar\\rbnnresult449_121.pcd");
	FString ClassColorsFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\point_cloudia\\colormap.txt");
	FString AugmentablesFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\augmentation\\augmentation_result.txt");
	FString AugmentedFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\augmentation\\augmented.txt");

	/*FString PointCloudFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\tests\\dmr_augs_merge_test\\result.txt");
	FString PointCloudClassFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\tests\\dmr_augs_merge_test\\resultclass.txt");
	FString PointCloudIntensityFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\tests\\dmr_augs_merge_test\\resultintensity.txt");
	FString FloatingObjectFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\tests\\dmr_augs_merge_test\\resultrbnn.txt");

	FString ClassColorsFile = TEXT("C:\\Users\\km\\Desktop\\MAG\\FloatingObjectFilter\\data\\colormap.txt");
	FString AugmentablesFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\augmentation\\augmentation_resultend.txt");
	FString AugmentedFile = TEXT("E:\\workspaces\\LIDAR_WORKSPACE\\augmentation\\augmented.txt");*/
	#pragma endregion

	#pragma region [locals]
	APointCloudActor * PointCloudHostActor;
	TArray<FPointCloudPoint> FilteredPoints;
	TArray<FPointCloudPoint> LoadedPoints;
	UPointCloud* PointCloud;
	float MaxX;
	float MinY;
	float MinZ;

	////// rbnn
	TArray<TArray<FString>> RbnnResults;
	double preferredFloatingObjectRadius = 1;
	int32 currentRbnnIndex;

	// rbnn viewing of clusters
	int32 currentViewedClusterIndex = 0;
	TMap<int32, TArray<int32>> CurrentClusterToClusterPointIndicesMap;
	TArray<int32> RbnnClusterIndices;
	AActor *Marker;
	FString MarkerMaterialPath = TEXT("/Game/Materials/ParameterizedTranslucentGold");
	UMaterialInstanceDynamic* MarkerMaterialInstance;
	int32 MarkerHealth = 300;

	// class data
	TArray<int32> Classifications;
	TMap<int32, FVector> ClassToColorMap;

	// intensity data
	TArray<float> Intensities;


	////// augmentation
	FString AugmentationFinalResultString = TEXT("");

	// augmentables data
	TArray<TArray<FString>> Augmentables;

	TQueue<TArray<FString>> AugmentablesQueue;

	FTransform AugmentationStartingTransform;

	#pragma endregion

public:	
	UPointCloudRenderingComponent();

protected:
	virtual void BeginPlay() override;

	

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FString StoreAugmentedSamples();

public: // API

	UFUNCTION(BlueprintCallable)
	FString ProcessSelectedPoints(FVector& CenterInWorldSpace, FVector& BoundingBox);

	UFUNCTION(BlueprintCallable)
	void ChangeColorMode(EFloatingSegmentColorMode mode);

	UFUNCTION(BlueprintCallable)
	void ChangeFilterMode(EFilterModeEnum mode);

	UFUNCTION(BlueprintCallable)
	void ChangeRbnnIndex();

	UFUNCTION(BlueprintCallable)
	void MoveToNextFloatingObject();

	UFUNCTION(BlueprintCallable)
	FString GetSaveLabelResultString(EFloatingObjectLabel Label);

	UFUNCTION(BlueprintCallable)
	FString GetDatasetPath();

	UFUNCTION(BlueprintCallable)
	void StartAugmentation(FTransform StartingTransform);

	UFUNCTION(BlueprintCallable)
	FString GetAugmentationFinalResultString();

	


protected: // auxiliary

	void LoadAndPreparePoints();

	void LoadAugmentables();

	void Augment(TArray<FString> Augmentable);
	FString AugmentedExampleDescriptionToString(EAugmentationObject objectType, float minrbnnr, TArray<FVector> &Points, URieglLMSQ780 * Scanner);

	void RerenderPointCloud();

	void ColorLabelsForVerification();

	void ColorPoints(TArray<FPointCloudPoint>& Points);
	void ColorPointsByClass(TArray<FPointCloudPoint>& Points);
	void ColorPointsUniform(TArray<FPointCloudPoint> & Points);
	void ColorPointsMixed(TArray<FPointCloudPoint> & Points);
	void ColorPointsByIntensity(TArray<FPointCloudPoint>& Points);
	
	void LoadRbnnResults();
	void LoadClassifications();
	void LoadIntensities();
	void LoadDesiredClassColors();

	void FilterPoints(TArray<FPointCloudPoint> & LoadedPoints);

	void FilterNonFloatingObjectPoints();

	void FilterFloorPoints();
	
	void RecomputeSelectedRbnnClusterParameters();
	void SpaceTransformPCToLocal(TArray<FPointCloudPoint> &LoadedPoints);
	void SpaceTransformPCToLocal(FVector& vec);
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
