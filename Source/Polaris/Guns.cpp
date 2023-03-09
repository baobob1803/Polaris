// Fill out your copyright notice in the Description page of Project Settings.


#include "Guns.h"

AGuns::AGuns()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGuns::BeginPlay()
{
	Super::BeginPlay();

}


void AGuns::PrimaryUse(FVector start, FVector direction)
{
	Super::PrimaryUse(start, direction);
	ShootHitscan(start, direction);
}

void AGuns::SecondaryUse(FVector start, FVector direction)
{
	Super::SecondaryUse(start, direction);
}

void AGuns::ThirdUse()
{
	Super::ThirdUse();
}

void AGuns::ShootHitscan(FVector start, FVector direction)
{
	FVector end = start + direction * 10000.f;
	TArray<FHitResult> outHits;

	FCollisionQueryParams collisionParams;

	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, 5.0f, 0, 5);

	if (GetWorld()->LineTraceMultiByChannel(outHits, start, end, ECC_Visibility, collisionParams))
	{
		int i = 0;
		for (auto& hit : outHits)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *outHits[i].GetComponent()->GetName()));
			i++;
		}
	}
}

void AGuns::ShootProjectile(FVector start, FVector direction)
{

}


