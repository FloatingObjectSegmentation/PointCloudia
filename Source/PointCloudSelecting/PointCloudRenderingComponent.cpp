// Fill out your copyright notice in the Description page of Project Settings.

#include "PointCloudRenderingComponent.h"


// Sets default values for this component's properties
UPointCloudRenderingComponent::UPointCloudRenderingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPointCloudRenderingComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<FPointCloudPoint> LoadedPoints;
	EPointCloudColorMode Mode = EPointCloudColorMode::RGB;
	FPointCloudFileHeader Header = FPointCloudHelper::ReadFileHeader(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\slama.txt"));

	TArray<int32> SelectedColumns;
	for (int32 i = 0; i < 6; i++) {
		SelectedColumns.Add(i);
	}
	Header.SelectedColumns = SelectedColumns;

	FPointCloudHelper::ImportAsText(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\slama.txt"), LoadedPoints, Mode, 0, 10, Header);
	int32 NumberOfPoints = LoadedPoints.Num();
	UE_LOG(LogTemp, Warning, TEXT("PointCloudLoaded %d"), NumberOfPoints);
}


// Called every frame
void UPointCloudRenderingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

