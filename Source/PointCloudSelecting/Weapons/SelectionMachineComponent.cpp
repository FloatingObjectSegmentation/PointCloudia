// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectionMachineComponent.h"


// Sets default values for this component's properties
USelectionMachineComponent::USelectionMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

#pragma region unreal events
void USelectionMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	BoundingBox = nullptr;
	TransformMode = ETransformEnum::Translation;
	Speed = 5.0f;
	BBMaterialPath = TEXT("/Game/Materials/ParameterizedTranslucent"); // Referencing assets in UE4 code: Game refers to the content folder...

	// get the stuff from which we'll later spawn the bounding box
	InitializeBoundingBoxTemplate();
}

void USelectionMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsDying && Health > 0)
	{
		KillALittleMore();
	}
	else if (IsDying && Health == 0)
	{
		DestroyAndReinitialize();
	}
}
#pragma endregion

#pragma region Blueprint API
void USelectionMachineComponent::SetMode(ETransformEnum transformMode) 
{
	TransformMode = transformMode;
}

void USelectionMachineComponent::StartSelection() 
{
	if (BoundingBox != nullptr) return;
	BoundingBox = SpawnBoundingBox();
	SetBoundingBoxAttributes();
}

void USelectionMachineComponent::TransformX(int32 way) 
{
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(way, 0 ,0));
}

void USelectionMachineComponent::TransformY(int32 way) 
{
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(0, way, 0));
}

void USelectionMachineComponent::TransformZ(int32 way) 
{
	if (BoundingBox == nullptr) return;
	CommitTransformation(FVector(0, 0, way));
}

void USelectionMachineComponent::FinishSelection() 
{
	if (BoundingBox == nullptr) return;

	// obtain the boundaries
	FVector Origin;
	FVector BoundingExtent;
	BoundingBox->GetActorBounds(false, Origin, BoundingExtent);
	UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *(Origin.ToString()), *(BoundingExtent.ToString()));
	

	// save the boundaries into a file in world space.
	// obtain the inverse transform of the point cloud, so you
	// may know where the boundaries are in the original space

	// decay the BoundingBox
	IsDying = true;
}
#pragma endregion

#pragma region auxiliary
AActor* USelectionMachineComponent::SpawnBoundingBox()
{
	FVector SpawningLocation = GetOwner()->GetActorLocation() + 500.0f * GetOwner()->GetActorForwardVector();
	FTransform SpawningTransform;
	SpawningTransform.SetLocation(SpawningLocation);

	UClass* param = AStaticMeshActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	return spawned;
}

void USelectionMachineComponent::InitializeBoundingBoxTemplate()
{
	TSubclassOf<AActor> ClassToFind = AStaticMeshActor::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	FindTemplateMesh(FoundActors);
}

void USelectionMachineComponent::FindTemplateMesh(TArray<AActor *> &FoundActors)
{
	// This is a temporary and bad solution - it absolutely requires a simple cube to be
	// in the scene.
	
	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		if (str.Contains(TEXT("Cube"))) {		
			ExtractStaticMeshFromActor(FoundActors, i);
			break;
		}
	}
}

void USelectionMachineComponent::ExtractStaticMeshFromActor(TArray<AActor *> & FoundActors, const int32 &i)
{
	TArray<UStaticMeshComponent*> Components;
	FoundActors[i]->GetComponents<UStaticMeshComponent>(Components);
	if (Components.Num() > 0) {
		DesiredStaticMesh = Components[0]->GetStaticMesh();
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

		// set the target material
		BBMaterial = GetMaterialInstance();
		StaticMeshComponent->SetMaterial(0, BBMaterial);
	}
}

UMaterialInstanceDynamic* USelectionMachineComponent::GetMaterialInstance()
{
	UMaterial* mat = LoadObject<UMaterial>(UMaterial::StaticClass(), *BBMaterialPath);
	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(mat, mat);
	MaterialInstance->SetScalarParameterValue(TEXT("Opacity"), Health / 100.0f);
	return MaterialInstance;
}

void USelectionMachineComponent::CommitTransformation(FVector trans)
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

void USelectionMachineComponent::DestroyAndReinitialize()
{
	BoundingBox->Destroy();
	BoundingBox = nullptr;
	Health = 80.0f;
	IsDying = false;
}
void USelectionMachineComponent::KillALittleMore()
{
	Health--;
	BBMaterial->SetScalarParameterValue("Opacity", Health / 100.0f);
}
#pragma endregion
