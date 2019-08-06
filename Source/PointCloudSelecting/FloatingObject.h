// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingObject.generated.h"

UENUM(BlueprintType)
enum class EAugmentationObject : uint8
{
	Sphere						UMETA(DisplayName = "Sphere"),
	Cube						UMETA(DisplayName = "Cube"),
	Bird						UMETA(DisplayName = "Bird"),
	Airplane					UMETA(DisplayName = "Airplane"),
	Balloon						UMETA(DisplayName = "Balloon"),
	Drone						UMETA(DisplayName = "Drone")
};

UCLASS()
class POINTCLOUDSELECTING_API AFloatingObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
