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


#pragma region unreal events
void UPointCloudRenderingComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<FPointCloudPoint> LoadedPoints;
	LoadPointsFromFile(LoadedPoints);
	SpaceTransformPCToLocal(LoadedPoints);
	UPointCloud* PointCloud = PrepareRenderingSettings(LoadedPoints); // use a predefined configuration (for now)
	SpawnPointCloudHostActor(FTransform(FVector(0.0f)));
	PointCloudHostActor->SetPointCloud(PointCloud);

	UE_LOG(LogTemp, Warning, TEXT("Point loaded %s"), *LoadedPoints[0].Color.ToString());
}

void UPointCloudRenderingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
#pragma endregion

#pragma region API

void UPointCloudRenderingComponent::SpaceTransformPCToLocal(TArray<FPointCloudPoint> &LoadedPoints) {

	// It appears that it needs to be reflected through the X axis to be correctly visualized.
	// Something may be wrong in the transformation, but as long as we are getting the correct
	// object out, it will suffice for this application.

	int MaxX = INT32_MIN, MinY = INT32_MAX, MinZ = INT32_MAX;
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		if (LoadedPoints[i].Location.X > MaxX) MaxX = LoadedPoints[i].Location.X; // reflect through X axis
		if (LoadedPoints[i].Location.Y < MinY) MinY = LoadedPoints[i].Location.Y;
		if (LoadedPoints[i].Location.Z < MinZ) MinZ = LoadedPoints[i].Location.Z;
	}
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		LoadedPoints[i].Location.X = MaxX - LoadedPoints[i].Location.X;
		LoadedPoints[i].Location.Y -= MinY;
		LoadedPoints[i].Location.Z -= MinZ;
	}
	
}

void UPointCloudRenderingComponent::SpaceTransformLocalToPC(TArray<FPointCloudPoint> &LoadedPoints) {
	// TODO
}

void UPointCloudRenderingComponent::SpaceTransformWorldToPC(TArray<FPointCloudPoint> &LoadedPoints) {
	// TODO
}
#pragma endregion

#pragma region auxiliary
UPointCloud* UPointCloudRenderingComponent::PrepareRenderingSettings(TArray<FPointCloudPoint> &LoadedPoints)
{
	UPointCloud* PointCloud = NewObject<UPointCloud>(this->StaticClass(), TEXT("PointCloud"));
	UPointCloudSettings* PointCloudSettings = NewObject<UPointCloudSettings>(this->StaticClass(), TEXT("PointCloudSettings"));
	PointCloudSettings->RenderMethod = EPointCloudRenderMethod::Sprite_Unlit_RGB;
	PointCloudSettings->SpriteSize = FVector2D(0.4f, 0.4f);
	PointCloudSettings->Scale = FVector(1.0f);
	PointCloudSettings->Brightness = 8.f;
	PointCloudSettings->Saturation = 4.5f;
	PointCloudSettings->SectionSize = FVector(100.f);
	PointCloud->SetSettings(PointCloudSettings);
	PointCloud->SetPointCloudData(LoadedPoints, true);
	return PointCloud;
}

void UPointCloudRenderingComponent::SpawnPointCloudHostActor(FTransform const &SpawningTransform)
{
	UClass* param = APointCloudActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	FVector f = spawned->GetActorLocation();
	PointCloudHostActor = dynamic_cast<APointCloudActor*>(spawned);
}

void UPointCloudRenderingComponent::LoadPointsFromFile(TArray<FPointCloudPoint> &LoadedPoints)
{
	
	EPointCloudColorMode Mode = EPointCloudColorMode::RGB;
	FPointCloudFileHeader Header = FPointCloudHelper::ReadFileHeader(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\simon.txt"));
	Header.RGBRange = FVector2D(0.0f, 256.0f * 256.0f - 1.0f);

	TArray<int32> SelectedColumns;
	for (int32 i = 0; i < 6; i++) {
		SelectedColumns.Add(i);
	}
	Header.SelectedColumns = SelectedColumns;

	FPointCloudHelper::ImportAsText(TEXT("C:\\Users\\km\\Desktop\\graphics\\data\\simon.txt"), LoadedPoints, Mode, 0, 500000000, Header); 
}
#pragma endregion
