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
	LoadPointCloudPointsFromFile(LoadedPoints);

	FixPointColors(LoadedPoints);
	NormalizePointLocations(LoadedPoints);

	
	FTransform SpawningTransform;
	FVector SpawningLocation(0.0f, 0.0f, 0.0f);
	SpawningTransform.SetLocation(SpawningLocation);

	UClass* param = APointCloudActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	FVector f = spawned->GetActorLocation();
	PointCloudHostActor = dynamic_cast<APointCloudActor*>(spawned);

	UPointCloud* PointCloud = NewObject<UPointCloud>(this->StaticClass(), TEXT("PointCloud"));
	PointCloud->SetPointCloudData(LoadedPoints, true);
	PointCloudHostActor->SetPointCloud(PointCloud);


	int32 NumberOfPoints = LoadedPoints.Num();
	UE_LOG(LogTemp, Warning, TEXT("PointCloudLoaded %d"), NumberOfPoints);
}

void UPointCloudRenderingComponent::LoadPointCloudPointsFromFile(TArray<FPointCloudPoint> &LoadedPoints)
{
	EPointCloudColorMode Mode = EPointCloudColorMode::RGB;
	FPointCloudFileHeader Header = FPointCloudHelper::ReadFileHeader(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\slama.txt"));

	TArray<int32> SelectedColumns;
	for (int32 i = 0; i < 6; i++) {
		SelectedColumns.Add(i);
	}
	Header.SelectedColumns = SelectedColumns;

	FPointCloudHelper::ImportAsText(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\slama.txt"), LoadedPoints, Mode, 0, 10000000, Header);
}

void UPointCloudRenderingComponent::FixPointColors(TArray<FPointCloudPoint> &LoadedPoints)
{
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		LoadedPoints[i].Color.R = 128;
		LoadedPoints[i].Color.G = 128;
		LoadedPoints[i].Color.B = 128;
	}
}

void UPointCloudRenderingComponent::NormalizePointLocations(TArray<FPointCloudPoint> &LoadedPoints)
{
	int MinX = INT32_MAX, MinY = INT32_MAX, MinZ = INT32_MAX;
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		if (LoadedPoints[i].Location.X < MinX) MinX = LoadedPoints[i].Location.X;
		if (LoadedPoints[i].Location.Y < MinY) MinY = LoadedPoints[i].Location.Y;
		if (LoadedPoints[i].Location.Z < MinZ) MinZ = LoadedPoints[i].Location.Z;
	}
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		LoadedPoints[i].Location.X -= MinX;
		LoadedPoints[i].Location.Y -= MinY;
		LoadedPoints[i].Location.Z -= MinZ;
	}
}


// Called every frame
void UPointCloudRenderingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

