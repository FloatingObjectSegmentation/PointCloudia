// Fill out your copyright notice in the Description page of Project Settings.

#include "AugmentationStarterComponent.h"


// Sets default values for this component's properties
UAugmentationStarterComponent::UAugmentationStarterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


#pragma region [unreal events]
void UAugmentationStarterComponent::BeginPlay()
{
	Super::BeginPlay();

	MainPointCloud = GetPointCloudRenderingComponent();
	if (MainPointCloud->AugmentationMode == EAugmentationMode::NoAugmentation || MainPointCloud->AugmentationMode == EAugmentationMode::RenderAugmentationOnly)
		GetOwner()->Destroy();
}


// Called every frame
void UAugmentationStarterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
#pragma endregion

#pragma region [API]
void UAugmentationStarterComponent::TransformYaw(int32 way)
{
	if (way == 0) return;
	GetOwner()->SetActorRotation(GetOwner()->GetActorRotation() + FRotator(0, 0.3 * way, 0));
}

void UAugmentationStarterComponent::StartAugmentation() 
{
	MainPointCloud->StartAugmentation(GetOwner()->GetActorTransform());
	GetOwner()->Destroy();
}
#pragma endregion

#pragma region [auxiliary]
UPointCloudRenderingComponent* UAugmentationStarterComponent::GetPointCloudRenderingComponent()
{
	// WARNING!! Very stupid style of communication between components (dangerous and slow), 
	// it will suffice until I learn to implement better styles (IE observer pattern).
	UPointCloudRenderingComponent* result = nullptr;

	TSubclassOf<AActor> ClassToFind = AStaticMeshActor::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);
	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		if (str.Contains(TEXT("PointCloudAnchor"))) { // Dangerous -> relies on the name of the anchor never changing
			TArray<UPointCloudRenderingComponent*> Components;
			FoundActors[i]->GetComponents<UPointCloudRenderingComponent>(Components);
			if (Components.Num() > 0) {
				result = Components[0];
				break;
			}
			break;
		}
	}
	return result;
}
#pragma endregion