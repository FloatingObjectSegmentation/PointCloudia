// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "Engine/WorldComposition.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SelectionMachineComponent.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ETransformEnum : uint8
{
	Translation 	UMETA(DisplayName = "Translation"),
	Rotation    	UMETA(DisplayName = "Rotation"),
	Scaling  	    UMETA(DisplayName = "Scaling")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POINTCLOUDSELECTING_API USelectionMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USelectionMachineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETransformEnum TransformMode;

	AActor* BoundingBox;
	AActor* DesiredBBActor;
	UStaticMesh *DesiredStaticMesh;

public:
	UFUNCTION(BlueprintCallable, Category = "Performance")
	void SetMode(ETransformEnum tmode);

	UFUNCTION(BlueprintCallable, Category = "Performance")
	void StartSelection();

	UFUNCTION(BlueprintCallable, Category = "Performance")
	void FinishSelection();

	UFUNCTION(BlueprintCallable, Category = "Performance")
	void TransformX();

	UFUNCTION(BlueprintCallable, Category = "Performance")
	void TransformY();

	UFUNCTION(BlueprintCallable, Category = "Performance")
	void TransformZ();
};
