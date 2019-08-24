// Fill out your copyright notice in the Description page of Project Settings.

#include "AugmentationMachineComponent.h"


// Sets default values for this component's properties
UAugmentationMachineComponent::UAugmentationMachineComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


#pragma region [unreal events]

// Called when the game starts
void UAugmentationMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadAirplaneStaticMesh();
	FindFloatingObjectSpawnerInWorld();
}


// Called every frame
void UAugmentationMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
#pragma endregion

#pragma region [API]
void UAugmentationMachineComponent::StartScanning(FVector airplaneLocation, FRotator airplaneOrientation, FVector objectLocation, FVector objectScaleInMeters, EAugmentationObject object, float minRbnnR)
{
	ObjectType = object;
	MinRbnnR = minRbnnR;

	Airplane = SpawnAirplane(airplaneLocation, airplaneOrientation, FVector(50.0f, 50.0f, 50.0f));
 	AugmentedObject = Spawner->SpawnFloatingObject(ObjectType, objectLocation, objectScaleInMeters);

	URieglLMSQ780* comp = NewObject<URieglLMSQ780>(Airplane);
	comp->SetAugmentedObject(AugmentedObject);
	comp->RegisterComponent();
	Airplane->AddOwnedComponent(comp);
}
#pragma endregion

#pragma region [auxiliary]

#pragma region [airplane mesh loading]
void UAugmentationMachineComponent::LoadAirplaneStaticMesh()
{
	TSubclassOf<AActor> ClassToFind = AActor::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	FindAirplaneMesh(FoundActors);
}

void UAugmentationMachineComponent::FindAirplaneMesh(TArray<AActor *> &FoundActors)
{
	// This is a temporary and bad solution - it absolutely requires a simple cube to be
	// in the scene.

	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		if (str.Contains(TEXT("Pointer"))) {
			ExtractStaticMeshFromActor(FoundActors, i);
			break;
		}
	}
}

void UAugmentationMachineComponent::ExtractStaticMeshFromActor(TArray<AActor *> & FoundActors, const int32 &i)
{
	TArray<UStaticMeshComponent*> Components;
	FoundActors[i]->GetComponents<UStaticMeshComponent>(Components);
	if (Components.Num() > 0) {
		AirplaneStaticMesh = Components[0]->GetStaticMesh();
	}
}
#pragma endregion

#pragma region [spawning airplane]
AActor* UAugmentationMachineComponent::SpawnAirplane(FVector Location, FRotator Orientation, FVector Scale)
{
	FTransform SpawningTransform(Orientation, Location);
	//GetBoundingBoxSpawnTransform(OUT SpawningTransform);

	UClass* param = AStaticMeshActor::StaticClass();
	Airplane = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	SetAirplaneAttributes();
	Airplane->SetActorLocation(Airplane->GetActorLocation() - Airplane->GetActorForwardVector() * 0.15 * FMath::Max3(Scale.X, Scale.Y, Scale.Z));
	return Airplane;
}

void UAugmentationMachineComponent::SetAirplaneAttributes()
{
	TArray<UStaticMeshComponent*> Components;
	Airplane->GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMeshComponent->SetStaticMesh(AirplaneStaticMesh);
		//StaticMeshComponent->SetWorldRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));


		// set the target material
		//BBMaterial = GetMaterialInstance();
		//StaticMeshComponent->SetMaterial(0, BBMaterial);
	}
}
#pragma endregion


void UAugmentationMachineComponent::FindFloatingObjectSpawnerInWorld()
{
	TSubclassOf<AActor> ClassToFind = AFloatingObjectSpawner::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);
	Spawner = Cast<AFloatingObjectSpawner>(FoundActors[0]);
}
#pragma endregion
