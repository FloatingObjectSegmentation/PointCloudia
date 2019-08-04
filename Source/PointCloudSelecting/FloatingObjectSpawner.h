// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL INCLUDES
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/WorldComposition.h"
#include "DrawDebugHelpers.h"

// PROJECT INCLUDES
#include "FloatingObject.h"

// MANDATORY INCLUDE
#include "FloatingObjectSpawner.generated.h"

UCLASS()
class POINTCLOUDSELECTING_API AFloatingObjectSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingObjectSpawner();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AFloatingObject> Bird;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AFloatingObject> Airplane;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AFloatingObject> Balloon;
	
	AActor* SpawnFloatingObject(EAugmentationObject objectType, FVector location, FVector scaleInMeters);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
