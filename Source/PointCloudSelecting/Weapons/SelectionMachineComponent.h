// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SelectionMachineComponent.generated.h"


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

private:
	UFUNCTION(BlueprintCallable, Category = "Performance")
	void SetMode();

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
