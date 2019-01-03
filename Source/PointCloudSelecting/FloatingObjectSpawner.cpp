// Fill out your copyright notice in the Description page of Project Settings.

#include "FloatingObjectSpawner.h"


// Sets default values
AFloatingObjectSpawner::AFloatingObjectSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
#pragma region [unreal events]
void AFloatingObjectSpawner::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFloatingObjectSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

#pragma region [API]
AActor* AFloatingObjectSpawner::SpawnFloatingObject(EAugmentationObject objectType, FVector location, FVector scaleInMeters)
{
	FActorSpawnParameters SpawnParams;
	TSubclassOf<AFloatingObject> currentType;

	switch (objectType) {
	case EAugmentationObject::Sphere:
		currentType = Bird;
		break;
	case EAugmentationObject::Cube:
		currentType = Bird;
		break;
	case EAugmentationObject::Bird:
		currentType = Bird;
		break;
	case EAugmentationObject::Airplane:
		currentType = Airplane;
		break;
	case EAugmentationObject::Balloon:
		currentType = Balloon;
		break;
	}

	//Actual Spawn. The following function returns a reference to the spawned actor
	AFloatingObject* ActorRef = GetWorld()->SpawnActor<AFloatingObject>(currentType, FTransform(location));

	TArray<UStaticMeshComponent*> Components;
	ActorRef->GetComponents<UStaticMeshComponent>(Components);
	FVector min, max;
	Components[0]->GetLocalBounds(min, max);
	Components[0]->SetMobility(EComponentMobility::Movable);

	ActorRef->SetActorScale3D(scaleInMeters / (max - min));

	return ActorRef;
}
#pragma endregion
