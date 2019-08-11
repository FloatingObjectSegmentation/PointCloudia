// Fill out your copyright notice in the Description page of Project Settings.

#include "RieglLMSQ780.h"


// Sets default values for this component's properties
URieglLMSQ780::URieglLMSQ780()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URieglLMSQ780::BeginPlay()
{
	Super::BeginPlay();

	// init laser rotation parameters
	PhiMaxRadians = PI * PhiMaxDegrees / 180.0;
	Alpha = PhiMaxRadians * 2 * SpeedAngular / (float)Frequency;
	Down = FVector(0, 0, -1);
	Direction = FVector(0, 0, -1);
	DirectionIntermediate = Direction;

	RecalcRotationMatrix();

	// init plane velocity parameters
	StartingPoint = GetOwner()->GetActorLocation();
	CurrentPoint = StartingPoint;
	dPoint = SpeedX / (float)Frequency;

	// Get the PointCloudRenderingComponent
	// PCRenderer = GetPointCloudRenderingComponent(); Don't really need now because we are not updating it
}

// Called every frame
void URieglLMSQ780::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector RayCastStart, RayCastEnd;

	for (int i = 0; i < StepsPerFrame; i++) {

		// break if enough distance traveled
		CurrentDistanceTraveled += dPoint;
		if (CurrentDistanceTraveled > MaxDistanceTraveled) {
			//UE_LOG(LogTemp, Warning, TEXT("AUGMENTATION COMPLETED"));
			return;
		}

		// update algorithm parameters
		CurrentPoint += FrameMatrix.TransformVector(FVector(dPoint, 0, 0));
		DirectionIntermediate = RotationMatrix.TransformVector(DirectionIntermediate);

		// update rotation direction if max reached
		double thresh = PhiMaxRadians;
		double distance = acos(Dot3(Down, DirectionIntermediate));
		//UE_LOG(LogTemp, Warning, TEXT("%f"), sqrt(pow(Direction.X, 2) + pow(Direction.Y, 2) + pow(Direction.Z, 2)));
		if (distance > thresh) {
			Alpha = -Alpha;
			RecalcRotationMatrix();
		}

		Direction = FrameMatrix.TransformVector(DirectionIntermediate);

		// define raycast parameters
		RayCastStart = CurrentPoint;
		RayCastEnd = RayCastStart + Direction * pow(10, 6);
		ECollisionChannel Channel = ECollisionChannel::ECC_Visibility; // this won't actually have multi line trace!

		TArray<FHitResult> Hits;
		GetWorld()->LineTraceMultiByChannel(Hits, RayCastStart, RayCastEnd, Channel);

		// save the intensity and location
		for (int32 i = 0; i < Hits.Num(); i++) {

			if (Hits[i].GetActor() == augmentedObject) {
				FVector SurfaceNormal = Hits[i].ImpactNormal;
				FVector HitDirection = -Direction;
				SurfaceNormal.Normalize();
				HitDirection.Normalize();
				
				float intensity = Dot3(SurfaceNormal, HitDirection);
				FVector location = Hits[i].Location;

				Points.Add(location);
				Intensities.Add(intensity);
			}

		}
	}
	/*DrawDebugLine(GetWorld(),
		RayCastStart,
		RayCastEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		10.0f);*/

	/*RerenderCounter++;
	if (RerenderCounter % RerenderInterval == 0) {
		RerenderPointCloud();
	}*/
}

#pragma region [auxiliary]
void URieglLMSQ780::RecalcRotationMatrix()
{
	float yaw = -GetOwner()->GetActorRotation().Yaw * PI / 180.0;

	FrameMatrix = FMatrix(FPlane(cos(yaw), -sin(yaw), 0, 0),
		FPlane(sin(yaw), cos(yaw), 0, 0),
		FPlane(0, 0, 1, 0),
		FPlane(0, 0, 0, 1));

	RotationMatrix = FMatrix(FPlane(1, 0, 0, 0),
		FPlane(0, cos(Alpha), -sin(Alpha), 0),
		FPlane(0, sin(Alpha), cos(Alpha), 0),
		FPlane(0, 0, 0, 1));
}
#pragma endregion

/*DrawDebugLine(GetWorld(),
RayCastStart,
RayCastEnd,
FColor(255, 0, 0),
false,
0.f,
0.f,
10.0f);*/

// compute the hits and store them to array