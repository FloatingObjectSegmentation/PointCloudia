// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionMachineComponent.h"


// Sets default values for this component's properties
USelectionMachineComponent::USelectionMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USelectionMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USelectionMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region Blueprint API
void USelectionMachineComponent::SetMode() {

}

void USelectionMachineComponent::StartSelection() {

}

void USelectionMachineComponent::TransformX() {

}

void USelectionMachineComponent::TransformY() {

}

void USelectionMachineComponent::TransformZ() {

}

void USelectionMachineComponent::FinishSelection() {

}
#pragma endregion
