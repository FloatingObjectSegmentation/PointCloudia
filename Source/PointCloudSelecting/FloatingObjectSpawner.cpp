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

	FVector origin;
	FVector extent;
	

	TArray<UStaticMeshComponent*> Components;
	ActorRef->GetComponents<UStaticMeshComponent>(Components);
	FVector min, max;
	
	Components[0]->GetLocalBounds(min, max);
	FVector extentInScale = max - min;

	Components[0]->SetMobility(EComponentMobility::Movable);

	FVector some = ActorRef->GetActorScale3D();

	scaleInMeters = FVector(scaleInMeters[0], scaleInMeters[2], scaleInMeters[1]);
	FVector newScale = scaleInMeters / extentInScale;
	ActorRef->SetActorScale3D(newScale);


	ActorRef->GetActorBounds(false, origin, extent);
	//DrawDebugBox(GetWorld(), origin, extent, FColor::Purple, true, -1, 0, 1);

	return ActorRef;
}
#pragma endregion
