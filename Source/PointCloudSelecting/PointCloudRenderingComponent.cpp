// Fill out your copyright notice in the Description page of Project Settings.

#include "PointCloudRenderingComponent.h"


// Sets default values for this component's properties
UPointCloudRenderingComponent::UPointCloudRenderingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


#pragma region unreal events
void UPointCloudRenderingComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadPointsFromFile(LoadedPoints);
	FindExtremes(LoadedPoints); // Needed to be able to compute transformations between PC, Local and World space
	SpaceTransformPCToLocal(LoadedPoints);
	PointCloud = PrepareRenderingSettings(LoadedPoints); // use a predefined configuration (for now)
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
// WARNING! These functions are unsafe and you have to be careful handling them.
// Need to remember which space you are in!

void UPointCloudRenderingComponent::QueryForRegion(FVector& CenterInWorldSpace, FVector& BoundingBox) 
{
	TArray<FPointCloudPoint> QueryResult;

	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		
		if (CenterInWorldSpace.X + BoundingBox.X / 2.0f > LoadedPoints[i].Location.X &&
			CenterInWorldSpace.X - BoundingBox.X / 2.0f < LoadedPoints[i].Location.X &&
			CenterInWorldSpace.Y + BoundingBox.Y / 2.0f > LoadedPoints[i].Location.Y &&
			CenterInWorldSpace.Y - BoundingBox.Y / 2.0f < LoadedPoints[i].Location.Y &&
			CenterInWorldSpace.Z + BoundingBox.Z / 2.0f > LoadedPoints[i].Location.Z &&
			CenterInWorldSpace.Z - BoundingBox.Z / 2.0f < LoadedPoints[i].Location.Z) 
		{
			UE_LOG(LogTemp, Warning, TEXT("YAAAAY!!!!"));
			LoadedPoints[i].Color.R = 255;
			LoadedPoints[i].Color.G = 0;
			LoadedPoints[i].Color.B = 0;
			LoadedPoints[i].Color.A = 150;
		}
	}

	// copy points
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		QueryResult.Emplace(LoadedPoints[i].Location.X, LoadedPoints[i].Location.Y, LoadedPoints[i].Location.Z,
			(float&)LoadedPoints[i].Color.R, (float&)LoadedPoints[i].Color.G, (float&)LoadedPoints[i].Color.B);
	}

	UPointCloud* tmpPointCloud = PrepareRenderingSettings(QueryResult);
	PointCloudHostActor->SetPointCloud(PointCloud);
	//PointCloud = tmpPointCloud;
}
void UPointCloudRenderingComponent::SavePoints(TArray<FPointCloudPoint> PointsToSave)
{
	// transform points to PC space
	for (int32 i = 0; i < PointsToSave.Num(); i++) {
		PointsToSave[i].Location.X = MaxX - PointsToSave[i].Location.X;
		PointsToSave[i].Location.Y += MinY;
		PointsToSave[i].Location.Z += MinZ;
	}

	// save the points to disk
}
#pragma endregion

#pragma region auxiliary
UPointCloud* UPointCloudRenderingComponent::PrepareRenderingSettings(TArray<FPointCloudPoint> &Points)
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
	PointCloud->SetPointCloudData(Points, true);
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

void UPointCloudRenderingComponent::FindExtremes(TArray<FPointCloudPoint> & LoadedPoints)
{
	MaxX = INT32_MIN;
	MinY = INT32_MAX;
	MinZ = INT32_MAX;
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		if (LoadedPoints[i].Location.X > MaxX) MaxX = LoadedPoints[i].Location.X; // reflect through X axis
		if (LoadedPoints[i].Location.Y < MinY) MinY = LoadedPoints[i].Location.Y;
		if (LoadedPoints[i].Location.Z < MinZ) MinZ = LoadedPoints[i].Location.Z;
	}
}

void UPointCloudRenderingComponent::SpaceTransformPCToLocal(TArray<FPointCloudPoint> &LoadedPoints) {

	// It appears that it needs to be reflected through the X axis to be correctly visualized.
	// Something may be wrong in the transformation, but as long as we are getting the correct
	// object out, it will suffice for this application.
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		LoadedPoints[i].Location.X = MaxX - LoadedPoints[i].Location.X;
		LoadedPoints[i].Location.Y -= MinY;
		LoadedPoints[i].Location.Z -= MinZ;
	}

}
#pragma endregion
