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
void USelectionMachineComponent::SetMode(ETransformEnum transformMode) {
	TransformMode = transformMode;
}

void USelectionMachineComponent::StartSelection() {
	// spawn a BoundingBox and hold a reference to it. Spawn it about 10 meters in front of the camera.

	FVector SpawningLocation = GetOwner()->GetActorLocation() + 50.0f * GetOwner()->GetActorForwardVector();
	FTransform SpawningTransform;
	SpawningTransform.SetLocation(SpawningLocation);

	UClass* param = BoundingBox->GetClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	FVector f = spawned->GetActorLocation();

	TArray<UStaticMeshComponent*> Components;
	spawned->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		StaticMeshComponent->SetStaticMesh(DesiredStaticMesh);
	}
}

void USelectionMachineComponent::TransformX() {
	// there is a mapping to this function. Transforms the X component of the BoundingBox

	switch (TransformMode) {
	case ETransformEnum::Translation:
		break;

	case ETransformEnum::Rotation:
		break;

	case ETransformEnum::Scaling:
		break;
	}
}

void USelectionMachineComponent::TransformY() {
	switch (TransformMode) {
	case ETransformEnum::Translation:
		break;

	case ETransformEnum::Rotation:
		break;

	case ETransformEnum::Scaling:
		break;
	}
}

void USelectionMachineComponent::TransformZ() {
	switch (TransformMode) {
	case ETransformEnum::Translation:
		break;

	case ETransformEnum::Rotation:
		break;

	case ETransformEnum::Scaling:
		break;
	}
}

void USelectionMachineComponent::FinishSelection() {
	// decay the BoundingBox

	// save the boundaries into a file in world space.
	// obtain the inverse transform of the point cloud, so you
	// may know where the boundaries are in the original space
}
#pragma endregion
