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

#pragma region unreal events
// Called when the game starts
void USelectionMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	BoundingBox = nullptr;
	TransformMode = ETransformEnum::Translation;
	Speed = 5.0f;

	InitializeBoundingBoxAttributes();
	
}

void USelectionMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
#pragma endregion

#pragma region Blueprint API
void USelectionMachineComponent::SetMode(ETransformEnum transformMode) {
	TransformMode = transformMode;
}

void USelectionMachineComponent::StartSelection() {

	BoundingBox = SpawnBoundingBox();

	SetBoundingBoxAttributes();
}

AActor* USelectionMachineComponent::SpawnBoundingBox()
{
	FVector SpawningLocation = GetOwner()->GetActorLocation() + 50.0f * GetOwner()->GetActorForwardVector();
	FTransform SpawningTransform;
	SpawningTransform.SetLocation(SpawningLocation);

	UClass* param = AStaticMeshActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	return spawned;
}

void USelectionMachineComponent::TransformX(int32 way) {
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(way, 0 ,0));
}

void USelectionMachineComponent::TransformY(int32 way) {
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(0, way, 0));
}

void USelectionMachineComponent::TransformZ(int32 way) {
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(0, 0, way));
}

void USelectionMachineComponent::FinishSelection() {
	// decay the BoundingBox

	// save the boundaries into a file in world space.
	// obtain the inverse transform of the point cloud, so you
	// may know where the boundaries are in the original space
}
#pragma endregion

#pragma region auxiliary
void USelectionMachineComponent::InitializeBoundingBoxAttributes()
{
	TSubclassOf<AActor> ClassToFind = AStaticMeshActor::StaticClass();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	SetBoundingBoxProperties(FoundActors);
}

void USelectionMachineComponent::SetBoundingBoxProperties(TArray<AActor *> &FoundActors)
{
	// This is a temporary and bad solution - it absolutely requires a simple cube to be
	// in the scene.
	
	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
		if (str.Contains(TEXT("Cube"))) {
			BoundingBox = FoundActors[i];
			TArray<UStaticMeshComponent*> Components;
			FoundActors[i]->GetComponents<UStaticMeshComponent>(Components);
			if (Components.Num() > 0) {
				DesiredStaticMesh = Components[0]->GetStaticMesh();
			}
			break;
		}
	}
}

void USelectionMachineComponent::SetBoundingBoxAttributes()
{
	TArray<UStaticMeshComponent*> Components;
	BoundingBox->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		StaticMeshComponent->SetStaticMesh(DesiredStaticMesh);
	}
}

void USelectionMachineComponent::CommitTransformation(FVector &trans)
{
	switch (TransformMode) {
	case ETransformEnum::Translation:
		BoundingBox->SetActorLocation(BoundingBox->GetActorLocation() + trans * Speed);
		break;

	case ETransformEnum::Rotation:
		BoundingBox->SetActorRotation(BoundingBox->GetActorRotation() + FRotator(trans[0], trans[1], trans[2]) * Speed);
		break;

	case ETransformEnum::Scaling:
		BoundingBox->SetActorScale3D(BoundingBox->GetActorScale3D() + trans * Speed * 0.01f);
		break;
	}
}
#pragma endregion
