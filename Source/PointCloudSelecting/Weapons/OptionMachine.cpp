// Fill out your copyright notice in the Description page of Project Settings.

#include "OptionMachine.h"


// Sets default values for this component's properties
UOptionMachine::UOptionMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOptionMachine::BeginPlay()
{
	Super::BeginPlay();

	PointCloudRenderingComponent = GetPointCloudRenderingComponent();
	// ...
	
}


// Called every frame
void UOptionMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region [api]
void UOptionMachine::ChangeColorMode()
{
	switch (CurrentColorMode) {
	case EFloatingSegmentColorMode::None:
		CurrentColorMode = EFloatingSegmentColorMode::Uniform;
		break;
	case EFloatingSegmentColorMode::Uniform:
		CurrentColorMode = EFloatingSegmentColorMode::Mixed;
		break;
	case EFloatingSegmentColorMode::Mixed:
		CurrentColorMode = EFloatingSegmentColorMode::Class;
		break;
	case EFloatingSegmentColorMode::Class:
		CurrentColorMode = EFloatingSegmentColorMode::None;
		break;
	}
	PointCloudRenderingComponent->ChangeColorMode(CurrentColorMode);
}

void UOptionMachine::ChangeFilterMode()
{
	switch (CurrentFilterMode) {
	case EFilterModeEnum::None:
		CurrentFilterMode = EFilterModeEnum::FilterFloor;
		break;
	case EFilterModeEnum::FilterFloor:
		CurrentFilterMode = EFilterModeEnum::FilterNonFloating;
		break;
	case EFilterModeEnum::FilterNonFloating:
		CurrentFilterMode = EFilterModeEnum::None;
		break;
	}
	PointCloudRenderingComponent->ChangeFilterMode(CurrentFilterMode);
}

void UOptionMachine::ChangeRbnnIndex()
{
	PointCloudRenderingComponent->ChangeRbnnIndex();
}

void UOptionMachine::MoveToNextFloatingObject()
{
	PointCloudRenderingComponent->MoveToNextFloatingObject();
}

FString UOptionMachine::LabelCurrentObject(EFloatingObjectLabel Label)
{
	FString label;
	switch (Label) {
	case EFloatingObjectLabel::Floating:
		label = TEXT("Floating");
		break;
	case EFloatingObjectLabel::LikelyFloating:
		label = TEXT("LikelyFloating");
		break;
	case EFloatingObjectLabel::MaybeFloating:
		label = TEXT("MaybeFloating");
		break;
	case EFloatingObjectLabel::NotFloating:
		label = TEXT("NotFloating");
		break;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("EXPORTING LABEL: %s"), *label));
	}
	return PointCloudRenderingComponent->GetSaveLabelResultString(Label);
}

FString UOptionMachine::GetDatasetDirectory() {
	FString path = PointCloudRenderingComponent->GetDatasetPath();
	FString name = GetDatasetName();
	path.Find(name);
	return path.Left(path.Len() - name.Len());
}

FString UOptionMachine::GetDatasetName() {
	FString path = PointCloudRenderingComponent->GetDatasetPath();
	return FPaths::GetBaseFilename(path);
}

FString UOptionMachine::GetLabelsFolderPath() {
	FString datasetPath = GetDatasetDirectory();
	FString path = FPaths::Combine(datasetPath, TEXT("labels"));
	return path;
}

#pragma endregion


#pragma region [auxiliary]
UPointCloudRenderingComponent* UOptionMachine::GetPointCloudRenderingComponent()
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