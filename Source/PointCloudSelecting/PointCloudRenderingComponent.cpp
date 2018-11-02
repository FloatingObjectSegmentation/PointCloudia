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

	LoadAndPreparePoints();

	if (UseFancyFeatures) {
		LoadRbnnResults();
		LoadClassifications();
		LoadIntensities();
		LoadDesiredClassColors();
	}

	RerenderPointCloud();

	UE_LOG(LogTemp, Warning, TEXT("Point loaded %s"), *LoadedPoints[0].Color.ToString());
}

void UPointCloudRenderingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// deplete marker and destroy after some time
	if (MarkerHealth > 0) {
		MarkerHealth--;

		if (MarkerMaterialInstance)
			MarkerMaterialInstance->SetScalarParameterValue(TEXT("Opacity"), (float)MarkerHealth / 300.0f);

		if (MarkerHealth == 0) {
			if (Marker) {
				Marker->Destroy();
				Marker = nullptr;
			}
		}
	}

}
#pragma endregion

#pragma region API
FString UPointCloudRenderingComponent::ProcessSelectedPoints(FVector& CenterInWorldSpace, FVector& BoundingBox)
{
	UE_LOG(LogTemp, Warning, TEXT("Inside ProcessSelectedPoints"));
	TArray<int32> QueryResultIndices;
	FindSelectionIndices(CenterInWorldSpace, BoundingBox, QueryResultIndices);
	TArray<FPointCloudPoint> SelectedPoints = GetPointSubset(QueryResultIndices);
	FString PointCloudTxt = SelectedPointsToPointCloudTxtFormatString(SelectedPoints);
	MarkSubsetWithinLoadedPoints(QueryResultIndices);
	RerenderPointCloud();
	return PointCloudTxt;
}
void UPointCloudRenderingComponent::ChangeColorMode(EFloatingSegmentColorMode mode)
{
	FloatingSegmentColorMode = mode;
	RerenderPointCloud();
}
void UPointCloudRenderingComponent::ChangeFilterMode(EFilterModeEnum mode)
{
	FilterMode = mode;
	RerenderPointCloud();
}
void UPointCloudRenderingComponent::ChangeRbnnIndex()
{
	currentRbnnIndex = (currentRbnnIndex + 1) % RbnnResults.Num();
	RecomputeSelectedRbnnClusterParameters();
	RerenderPointCloud();
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Viewing RBNN results for radius = %s"), *RbnnResults[currentRbnnIndex][0]));
	}
}

void UPointCloudRenderingComponent::MoveToNextFloatingObject()
{
	// change the current viewed object index
	currentViewedClusterIndex = (currentViewedClusterIndex + 1) % RbnnClusterIndices.Num();

	// get the location of new viewed object
	FPointCloudPoint ClusterPoint = LoadedPoints[CurrentClusterToClusterPointIndicesMap[RbnnClusterIndices[currentViewedClusterIndex]][0]];
	FVector Location = FVector(ClusterPoint.Location.X, ClusterPoint.Location.Y, ClusterPoint.Location.Z);
	FRotator rotator = UKismetMathLibrary::FindLookAtRotation(Location + FVector(0.0f, 30.0f, 30.0f), Location);

	// move the camera to that location
	APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerPawn->SetActorLocationAndRotation(Location + FVector(0.0f, 30.0f, 30.0f), rotator);

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Showing canditate %d / %d"), currentViewedClusterIndex, RbnnClusterIndices.Num()));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Contains %d points"), CurrentClusterToClusterPointIndicesMap[RbnnClusterIndices[currentViewedClusterIndex]].Num()));
		for (int i = 0; i < CurrentClusterToClusterPointIndicesMap[RbnnClusterIndices[currentViewedClusterIndex]].Num(); i++) {
			int index = CurrentClusterToClusterPointIndicesMap[RbnnClusterIndices[currentViewedClusterIndex]][i];
			float x = LoadedPoints[index].Location.X;
			float y = LoadedPoints[index].Location.Y;
			float z = LoadedPoints[index].Location.Z;
			int32 cls = Classifications[index];
			int32 rbnncls = FCString::Atoi(*RbnnResults[currentRbnnIndex][index + 1]);
			// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Orange, FString::Printf(TEXT("loc=(%f, %f, %f), class=%d, rbnn=%d"), x, y, z, cls, rbnncls));
		}
	}

	FTransform f(rotator, Location, FVector(0.15f, 0.15f, 0.15f));

	UClass* param = AStaticMeshActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &f, FActorSpawnParameters());
	

	// box spawning
	
	UStaticMesh* desired = nullptr;
	TSubclassOf<AActor> ClassToFind = AStaticMeshActor::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);
	
	
	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		if (str.Contains(TEXT("Cube"))) {
			TArray<UStaticMeshComponent*> Components;
			FoundActors[i]->GetComponents<UStaticMeshComponent>(Components);
			if (Components.Num() > 0) {
				desired = Components[0]->GetStaticMesh();
				//UE_LOG(LogTemp, Warning, TEXT("MESH EXTRACTED!"));
			}
			break;
		}
	}

	TArray<UStaticMeshComponent*> Components;
	spawned->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		StaticMeshComponent->SetStaticMesh(desired);

		UMaterial* mat = LoadObject<UMaterial>(UMaterial::StaticClass(), *MarkerMaterialPath);
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(mat, mat);
		MaterialInstance->SetScalarParameterValue(TEXT("Opacity"), 1.0f);

		MarkerMaterialInstance = MaterialInstance;

		StaticMeshComponent->SetMaterial(0, MaterialInstance);
	}

	if (Marker) {
		Marker->Destroy();
		Marker = nullptr;
	}
	Marker = spawned;
	MarkerHealth = 300;
}

FString UPointCloudRenderingComponent::GetSaveLabelResultString(EFloatingObjectLabel Label)
{
	// determine which points belong to the chosen cluster in the first place
	TArray<int32> loadedPointsIndices = CurrentClusterToClusterPointIndicesMap[RbnnClusterIndices[currentViewedClusterIndex]];

	int32 LabelInt = 0;
	switch (Label) {
	case EFloatingObjectLabel::Floating:
		LabelInt = 0;
		break;
	case EFloatingObjectLabel::LikelyFloating:
		LabelInt = 1;
		break;
	case EFloatingObjectLabel::MaybeFloating:
		LabelInt = 2;
		break;
	case EFloatingObjectLabel::NotFloating:
		LabelInt = 3;
		break;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("CANDIDATE NUMBER: %d, INDEX: %d"), currentViewedClusterIndex, RbnnClusterIndices[currentViewedClusterIndex]));
	}

	// save the points to disk
	FString total;
	total.Append(FString::Printf(TEXT("%d\n"), LabelInt)); // label
	total.Append(RbnnResults[currentRbnnIndex][0]); // rbnn radius
	total.Append("\n");
	for (int32 index : loadedPointsIndices)
	{
		FString line = FString::Printf(TEXT("%d\n"), index); // indices into the original dataset
		total.Append(line);
	}
	return total;
}


FString UPointCloudRenderingComponent::GetDatasetPath() {
	return PointCloudFile;
}
#pragma endregion

#pragma region auxiliary
void UPointCloudRenderingComponent::LoadAndPreparePoints()
{
	GetPointCloudPoints(LoadedPoints);
	FindExtremes(LoadedPoints); // Needed to be able to compute transformations between PC, Local and World space
	SpaceTransformPCToLocal(LoadedPoints);
}

void UPointCloudRenderingComponent::RerenderPointCloud()
{
	FilteredPoints.Empty();

	// copy originals to result buffer
	TArray<FPointCloudPoint> NewPointCloud;
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {
		float Rnorm = LoadedPoints[i].Color.R / 256.0f;
		float Gnorm = LoadedPoints[i].Color.G / 256.0f;
		float Bnorm = LoadedPoints[i].Color.B / 256.0f;
		NewPointCloud.Emplace(LoadedPoints[i].Location.X, LoadedPoints[i].Location.Y, LoadedPoints[i].Location.Z,
			Rnorm, Gnorm, Bnorm);
	}
	FilteredPoints = NewPointCloud;

	if (UseFancyFeatures) {
		// index preserved operations
		ColorPoints(FilteredPoints);

		// index not preserved operations
		FilterPoints(FilteredPoints);
	}

	if (PointCloudHostActor != nullptr)
		PointCloudHostActor->Destroy();
	PointCloudHostActor = nullptr;
	UPointCloud* tmpPointCloud = PrepareRenderingSettings(FilteredPoints, "PointCloud2", "Settings2");
	SpawnPointCloudHostActor(FTransform(FVector(0.0f)));
	PointCloudHostActor->SetPointCloud(tmpPointCloud);
}

void UPointCloudRenderingComponent::LoadRbnnResults()
{
	FString FloatingObjectFileContent;
	FFileHelper::LoadFileToString(FloatingObjectFileContent, *FloatingObjectFile);

	// split by lines
	TArray<FString> Lines;
	FloatingObjectFileContent.ParseIntoArray(Lines, TEXT("\n"));

	// determine which one you want to use and parse it

	// store all lines into array
	for (int32 lineIdx = 0; lineIdx < Lines.Num(); lineIdx++) {
		FString CurrentLine = Lines[lineIdx];

		TArray<FString> Values;
		CurrentLine.ParseIntoArray(Values, TEXT(" "));

		RbnnResults.Push(Values);
	}

	// set the current index
	for (int32 lineIdx = 0; lineIdx < Lines.Num(); lineIdx++) {

		if (FCString::Atof(*RbnnResults[lineIdx][0]) == preferredFloatingObjectRadius) {
			currentRbnnIndex = lineIdx;
			break;
		}
		else if (lineIdx == Lines.Num() - 1) {
			currentRbnnIndex = 0;
			UE_LOG(LogTemp, Warning, TEXT("Warning: The preferred RBNN radius result was not found in the results file. The last line in the file was used instead"));
		}
	}

	// compute the selected rbnn cluster parameters
	RecomputeSelectedRbnnClusterParameters();
}
void UPointCloudRenderingComponent::LoadClassifications()
{
	FString ClassFileContent;
	FFileHelper::LoadFileToString(ClassFileContent, *PointCloudClassFile);
	TArray<FString> Array;
	ClassFileContent.ParseIntoArray(Array, TEXT("\n"));
	for (int32 i = 0; i < Array.Num(); i++) {
		int32 ClassValue = FCString::Atoi(*Array[i]);
		Classifications.Add(ClassValue);
	}
}
void UPointCloudRenderingComponent::LoadIntensities()
{
	FString IntensityFileContent;
	FFileHelper::LoadFileToString(IntensityFileContent, *PointCloudIntensityFile);
	TArray<FString> Array;
	IntensityFileContent.ParseIntoArray(Array, TEXT("\n"));
	for (int32 i = 0; i < Array.Num(); i++) {
		float IntensityValue = FCString::Atof(*Array[i]);
		Intensities.Add(IntensityValue);
	}
}
void UPointCloudRenderingComponent::LoadDesiredClassColors() {
	// non-obligatory
	IPlatformFile& f = FPlatformFileManager::Get().GetPlatformFile();
	if (!f.FileExists(*ClassColorsFile))
		return;

	FString ColorsFileContent;
	FFileHelper::LoadFileToString(ColorsFileContent, *ClassColorsFile);
	TArray<FString> Array;
	ColorsFileContent.ParseIntoArray(Array, TEXT("\n"));
	for (int32 i = 0; i < Array.Num(); i++) {

		FString key;
		FString value;
		FString Separator = TEXT(":");
		Array[i].Split(Separator, &key, &value);
		key.Trim();
		value.Trim();

		int32 Key = FCString::Atoi(*key);

		TArray<FString> vector;
		value.ParseIntoArray(vector, TEXT(" "));
		int32 R = FCString::Atoi(*vector[0]);
		int32 G = FCString::Atoi(*vector[1]);
		int32 B = FCString::Atoi(*vector[2]);

		ClassToColorMap.Add(Key, FVector(R, G, B));
	}
}

#pragma region [processing selection]
void UPointCloudRenderingComponent::MarkSubsetWithinLoadedPoints(TArray<int32> &QueryResultIndices)
{
	for (auto& i : QueryResultIndices) {
		LoadedPoints[i].Color.R = 255;
		LoadedPoints[i].Color.G = 0;
		LoadedPoints[i].Color.B = 0;
	}
}
TArray<FPointCloudPoint> UPointCloudRenderingComponent::GetPointSubset(TArray<int32> &QueryResultIndices)
{
	TArray<FPointCloudPoint> QueryResult;
	for (auto& i : QueryResultIndices) {
		// normalizing colors is not a side effect of this function. It just needs to be done so because
		// the point cloud plugin is made so.
		float normR = LoadedPoints[i].Color.R / 256.0f;
		float normG = LoadedPoints[i].Color.G / 256.0f;
		float normB = LoadedPoints[i].Color.B / 256.0f;
		QueryResult.Emplace(LoadedPoints[i].Location.X, LoadedPoints[i].Location.Y, LoadedPoints[i].Location.Z,
			normR, normG, normB);
	}
	return QueryResult;
}
void UPointCloudRenderingComponent::FindSelectionIndices(FVector & CenterInWorldSpace, FVector & BoundingBox, TArray<int32> &QueryResultIndices)
{
	for (int32 i = 0; i < LoadedPoints.Num(); i++) {

		if (CenterInWorldSpace.X + BoundingBox.X > LoadedPoints[i].Location.X &&
			CenterInWorldSpace.X - BoundingBox.X < LoadedPoints[i].Location.X &&
			CenterInWorldSpace.Y + BoundingBox.Y > LoadedPoints[i].Location.Y &&
			CenterInWorldSpace.Y - BoundingBox.Y < LoadedPoints[i].Location.Y &&
			CenterInWorldSpace.Z + BoundingBox.Z > LoadedPoints[i].Location.Z &&
			CenterInWorldSpace.Z - BoundingBox.Z < LoadedPoints[i].Location.Z)
		{
			QueryResultIndices.Add(i);
		}
	}
}
FString UPointCloudRenderingComponent::SelectedPointsToPointCloudTxtFormatString(TArray<FPointCloudPoint> PointsToSave)
{
	// transform points to PC space
	for (int32 i = 0; i < PointsToSave.Num(); i++) {

		// locations
		PointsToSave[i].Location.X = MaxX - PointsToSave[i].Location.X;
		PointsToSave[i].Location.Y += MinY;
		PointsToSave[i].Location.Z += MinZ;

		// colors
		PointsToSave[i].Color.R = PointsToSave[i].Color.R * 255 * 255;
		PointsToSave[i].Color.G = PointsToSave[i].Color.G * 255 * 255;
		PointsToSave[i].Color.B = PointsToSave[i].Color.B * 255 * 255;
	}

	// save the points to disk
	FString total;
	for (auto& val : PointsToSave)
	{
		FString line = FString::Printf(TEXT("%.2f %.2f %.2f %d %d %d"), val.Location.X, val.Location.Y, val.Location.Z, (int32)val.Color.R, (int32)val.Color.G, (int32)val.Color.B);
		total.Append(line);
		total.Append("\n");
	}

	/* Class doesn't save the file anymore, merely returns the string
	FGuid guid = FGuid::NewGuid();
	FString filename = SavingFolder;
	filename.Append("/");
	filename.Append(guid.ToString() + TEXT(".txt"));
	SavingFile = filename;
	*/
	return total;
}

#pragma endregion
#pragma endregion

#pragma region auxiliary to auxiliary
#pragma region [setup]

void UPointCloudRenderingComponent::GetPointCloudPoints(TArray<FPointCloudPoint> &LoadedPoints)
{
	LoadedPoints = LoadPointCloudFromFileTXT(PointCloudFile, FVector2D(0.0f, 256.0f * 256.0f - 1.0f));
}

TArray<FPointCloudPoint> UPointCloudRenderingComponent::LoadPointCloudFromFileTXT(FString filename, FVector2D RgbRange)
{
	EPointCloudColorMode Mode = EPointCloudColorMode::RGB;
	FPointCloudFileHeader Header = FPointCloudHelper::ReadFileHeader(filename);
	Header.RGBRange = RgbRange;

	TArray<int32> SelectedColumns;
	for (int32 i = 0; i < 6; i++) {
		SelectedColumns.Add(i);
	}
	Header.SelectedColumns = SelectedColumns;

	TArray<FPointCloudPoint> points;
	FPointCloudHelper::ImportAsText(filename, points, Mode, 0, 500000000, Header);
	return points;
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

#pragma region [rerender]
UPointCloud* UPointCloudRenderingComponent::PrepareRenderingSettings(TArray<FPointCloudPoint> &Points, FString pointCloudName, FString settingsName)
{
	UPointCloud* PointCloud = NewObject<UPointCloud>(this->StaticClass(), *pointCloudName);
	UPointCloudSettings* PointCloudSettings = NewObject<UPointCloudSettings>(this->StaticClass(), *settingsName);
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
#pragma endregion

#pragma region [filtering]
void UPointCloudRenderingComponent::FilterPoints(TArray<FPointCloudPoint> & Points)
{
	if (FilterMode == EFilterModeEnum::None)
		return;
	else if (FilterMode == EFilterModeEnum::FilterFloor) {
		FilterFloorPoints();
	}
	else if (FilterMode == EFilterModeEnum::FilterNonFloating) {
		FilterNonFloatingObjectPoints();
	}
}
void UPointCloudRenderingComponent::FilterFloorPoints()
{
	TArray<FPointCloudPoint> TempPoints;
	for (int32 i = 0; i < Classifications.Num(); i++) {

		if (Classifications[i] != 2) {
			TempPoints.Add(FilteredPoints[i]);
		}
	}
	FilteredPoints = TempPoints;
}
void UPointCloudRenderingComponent::FilterNonFloatingObjectPoints()
{
	/// WARNING: DOES NOT ALWAYS WORK CORRECTLY. NEED TO DEBUG. DO NOT USE FOR THE TIME BEING.
	TArray<FString> FOPoints = RbnnResults[currentRbnnIndex];
	TArray<FPointCloudPoint> TempPoints;
	// warn rbnn results are shifted for 1 to the right, because the first element is radius
	for (int32 i = 1; i < FOPoints.Num() - 2; i++) {
		if (FCString::Atof(*FOPoints[i]) != -1) {
			TempPoints.Add(FilteredPoints[i - 1]);
		}
	}
	FilteredPoints = TempPoints;
}
#pragma endregion

#pragma region [coloring floating objects]
void UPointCloudRenderingComponent::ColorPoints(TArray<FPointCloudPoint>& Points)
{
	switch (FloatingSegmentColorMode) {
	case EFloatingSegmentColorMode::None:
		return;
		break;
	case EFloatingSegmentColorMode::Class:
		ColorPointsByClass(Points);
		break;
	case EFloatingSegmentColorMode::Mixed:
		ColorPointsMixed(Points);
		break;
	case EFloatingSegmentColorMode::Uniform:
		ColorPointsUniform(Points);
		break;
	case EFloatingSegmentColorMode::Intensity:
		ColorPointsByIntensity(Points);
		break;
	}
}

void UPointCloudRenderingComponent::ColorPointsByIntensity(TArray<FPointCloudPoint>& Points) {
	// color the points by the actual underlying class
	for (int i = 0; i < Intensities.Num(); i++) {
		float intensity = Intensities[i];
		Points[i].Color.R = FMath::Min(255.0f, intensity);
		Points[i].Color.G = 0;
		Points[i].Color.B = FMath::Min(255.0f, intensity / 256.0f);
	}
}

void UPointCloudRenderingComponent::ColorPointsByClass(TArray<FPointCloudPoint>& Points) {
	// color the points by the actual underlying class
	for (int i = 0; i < Classifications.Num(); i++) {
		if (ClassToColorMap.Contains(Classifications[i])) {
			FVector color = ClassToColorMap[Classifications[i]];
			Points[i].Color.R = color.X;
			Points[i].Color.G = color.Y;
			Points[i].Color.B = color.Z;
		}
	}
}

void UPointCloudRenderingComponent::ColorPointsUniform(TArray<FPointCloudPoint> & Points)
{
	// update the actual stuff
	TArray<FString> tmp = RbnnResults[currentRbnnIndex];
	for (int32 i = 0; i < tmp.Num() - 2; i++) { // -2 to avoid errors of boundaries

												// watch out: the first value in the results line is the radius, thus the indices are shifted by 1 to the right
		int32 CurrentValue = FCString::Atoi(*tmp[i + 1]);

		if (CurrentValue != -1) { // mark the floating point

			Points[i].Color.R = 255;
			Points[i].Color.G = 0;
			Points[i].Color.B = 0;
		}
	}
}

void UPointCloudRenderingComponent::ColorPointsMixed(TArray<FPointCloudPoint> & Points)
{
	// update the actual stuff
	TArray<FString> tmp = RbnnResults[currentRbnnIndex];
	TMap<int32, int32> ClassToColorMap;
	for (int32 i = 0; i < tmp.Num() - 2; i++) { // -2 to avoid errors of boundaries

												// watch out: the first value in the results line is the radius, thus the indices are shifted by 1 to the right
		int32 CurrentValue = FCString::Atoi(*tmp[i + 1]);
		if (i % 100000 == 0)
			UE_LOG(LogTemp, Warning, TEXT("colored %d"), i);
		if (CurrentValue != -1) { // mark the floating point

			if (!ClassToColorMap.Contains(CurrentValue)) {
				ClassToColorMap.Add(CurrentValue, FMath::RandRange(0, 5));
			}

			switch (ClassToColorMap[CurrentValue]) {
			case 0:
				Points[i].Color.R = 255;
				Points[i].Color.G = 0;
				Points[i].Color.B = 0;
				break;
			case 1:
				Points[i].Color.R = 0;
				Points[i].Color.G = 255;
				Points[i].Color.B = 0;
				break;
			case 2:
				Points[i].Color.R = 0;
				Points[i].Color.G = 0;
				Points[i].Color.B = 255;
				break;
			case 3:
				Points[i].Color.R = 255;
				Points[i].Color.G = 255;
				Points[i].Color.B = 0;
				break;
			case 4:
				Points[i].Color.R = 255;
				Points[i].Color.G = 0;
				Points[i].Color.B = 255;
				break;
			case 5:
				Points[i].Color.R = 0;
				Points[i].Color.G = 255;
				Points[i].Color.B = 255;
				break;
			}
		}

	}
}
#pragma endregion

#pragma region [changing rbnn radius]
void UPointCloudRenderingComponent::RecomputeSelectedRbnnClusterParameters()
{
	CurrentClusterToClusterPointIndicesMap.Empty();
	RbnnClusterIndices.Empty();


	TArray<FString> CurrentRbnnResults = RbnnResults[currentRbnnIndex];
	TSet<int32> ClusterIndexSet;
	for (int pointIdx = 1; pointIdx < CurrentRbnnResults.Num() - 2; pointIdx++) {
		if (CurrentRbnnResults[pointIdx] != "-1") {
			int32 clusterIdx = FCString::Atoi(*CurrentRbnnResults[pointIdx]);
			ClusterIndexSet.Add(clusterIdx);

			// add the locations so we can know where to move the camera when we select a new cluster
			if (!CurrentClusterToClusterPointIndicesMap.Contains(clusterIdx)) {
				TArray<int32> insert;
				insert.Add(pointIdx - 1);
				CurrentClusterToClusterPointIndicesMap.Add(clusterIdx, insert);
			}
			else {
				CurrentClusterToClusterPointIndicesMap[clusterIdx].Add(pointIdx - 1);
			}
		}
	}

	/* for (auto& Elem : CurrentClusterToClusterPointIndicesMap) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Cluster %d has %d members"), Elem.Key, Elem.Value.Num()));
		}
	} */

	// store cluster indices - to know which ones you can choose from for viewing.
	
	RbnnClusterIndices = ClusterIndexSet.Array();
}
#pragma endregion

#pragma endregion
