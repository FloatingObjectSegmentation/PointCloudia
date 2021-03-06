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
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "../PointCloudRenderingComponent.h"
#include "AugmentationStarterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API UAugmentationStarterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAugmentationStarterComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable)
		void TransformYaw(int32 way);

	UFUNCTION(BlueprintCallable)
		void StartAugmentation();

private:
	UPointCloudRenderingComponent * GetPointCloudRenderingComponent();
	UPointCloudRenderingComponent* MainPointCloud;
	
	
};
