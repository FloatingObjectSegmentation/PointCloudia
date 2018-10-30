// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Engine/WorldComposition.h"
#include "Engine/StaticMesh.h"
#include "Runtime/Engine/Classes/Materials/Material.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "../PointCloudRenderingComponent.h"
#include "Runtime/Core/Public/HAL/PlatformFilemanager.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "OptionMachine.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API UOptionMachine : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOptionMachine();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void ChangeColorMode();

	UFUNCTION(BlueprintCallable)
		void ChangeFilterMode();

	UFUNCTION(BlueprintCallable)
		void ChangeRbnnIndex();

	UFUNCTION(BlueprintCallable)
		void MoveToNextFloatingObject();

	UFUNCTION(BlueprintCallable)
		FString LabelCurrentObject(EFloatingObjectLabel Label);

	UFUNCTION(BlueprintCallable)
		FString GetDatasetDirectory();

	UFUNCTION(BlueprintCallable)
		FString GetDatasetName();

	UFUNCTION(BlueprintCallable)
		FString GetLabelsFolderPath();

private:
	UPointCloudRenderingComponent * PointCloudRenderingComponent;

	EFloatingSegmentColorMode CurrentColorMode = EFloatingSegmentColorMode::None;
	EFilterModeEnum CurrentFilterMode = EFilterModeEnum::None;
	int32 CurrentRbnnIdx = 0;

private:
	UPointCloudRenderingComponent* GetPointCloudRenderingComponent();
};
