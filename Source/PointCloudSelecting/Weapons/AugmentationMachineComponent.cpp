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
	LoadAugmentableMeshes();
}


// Called every frame
void UAugmentationMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
#pragma endregion

#pragma region [API]
void UAugmentationMachineComponent::StartScanning(FVector airplaneLocation, FRotator airplaneOrientation, FVector objectLocation, EAugmentationObject object)
{
	Airplane = SpawnAirplane(airplaneLocation, airplaneOrientation);
	SpawnObject(objectLocation, airplaneOrientation, object);

	URieglLMSQ780* comp = NewObject<URieglLMSQ780>(Airplane);
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

	FindTemplateMesh(FoundActors);
}

void UAugmentationMachineComponent::FindTemplateMesh(TArray<AActor *> &FoundActors)
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
		//UE_LOG(LogTemp, Warning, TEXT("MESH EXTRACTED!"));
	}
}
#pragma endregion

#pragma region [augmentable object mesh loading]
void UAugmentationMachineComponent::LoadAugmentableMeshes()
{
	TSubclassOf<AActor> ClassToFind = AStaticMeshActor::StaticClass();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassToFind, FoundActors);

	for (int32 i = 0; i < FoundActors.Num(); i++) {
		FString str = FoundActors[i]->GetActorLabel();
		if (str.Contains(TEXT("Sphere"))) {
			TArray<UStaticMeshComponent*> Components;
			FoundActors[i]->GetComponents<UStaticMeshComponent>(Components);
			if (Components.Num() > 0) {
				SphereStaticMesh = Components[0]->GetStaticMesh();
			}
			break;
		}
	}
}
#pragma endregion

#pragma region [spawning airplane]
AActor* UAugmentationMachineComponent::SpawnAirplane(FVector Location, FRotator Orientation)
{
	FTransform SpawningTransform(Orientation, Location);
	//GetBoundingBoxSpawnTransform(OUT SpawningTransform);

	UClass* param = AStaticMeshActor::StaticClass();
	Airplane = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	SetBoundingBoxAttributes();
	return Airplane;
}

void UAugmentationMachineComponent::GetBoundingBoxSpawnTransform(FTransform &SpawningTransform)
{
	FVector SpawningLocation;
	FVector PlayerViewPointLocation;
	FRotator Rotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT Rotation); // OUT does nothing, but it allows us to mark out params!
	FVector RotAsVector = Rotation.Vector();
	SpawningLocation = GetOwner()->GetActorLocation() + SpawnDistance * RotAsVector;
	SpawningTransform.SetLocation(SpawningLocation);
}

void UAugmentationMachineComponent::SetBoundingBoxAttributes()
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

#pragma region [spawning object]
void UAugmentationMachineComponent::SpawnObject(FVector Location, FRotator Orientation, EAugmentationObject type)
{
	FTransform SpawningTransform(Orientation, Location);

	UClass* param = AStaticMeshActor::StaticClass();
	AActor* spawned = GetWorld()->SpawnActor(param, &SpawningTransform, FActorSpawnParameters());
	AugmentedObject = spawned;
	SetAugmentedObjectAttributes(*spawned, type);
}

void UAugmentationMachineComponent::SetAugmentedObjectAttributes(AActor& object, EAugmentationObject type)
{
	TArray<UStaticMeshComponent*> Components;
	object.GetComponents<UStaticMeshComponent>(Components);
	for (int32 i = 0; i < Components.Num(); i++) {
		UStaticMeshComponent* StaticMeshComponent = Components[i];
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		switch (type) {
		case EAugmentationObject::Cube:
			break;
		case EAugmentationObject::Sphere:
			StaticMeshComponent->SetStaticMesh(SphereStaticMesh);
			break;
		default:
			break;
		}
		// set the target material
		//BBMaterial = GetMaterialInstance();
		//StaticMeshComponent->SetMaterial(0, BBMaterial);
	}
}
#pragma endregion

#pragma endregion
