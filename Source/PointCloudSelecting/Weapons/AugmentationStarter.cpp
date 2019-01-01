// Fill out your copyright notice in the Description page of Project Settings.

#include "AugmentationStarter.h"


// Sets default values
AAugmentationStarter::AAugmentationStarter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

#pragma region [unreal events]
// Called when the game starts or when spawned
void AAugmentationStarter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAugmentationStarter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
#pragma endregion

#pragma region [API]
#pragma endregion
