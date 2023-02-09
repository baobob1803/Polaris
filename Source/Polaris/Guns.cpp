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


void AGuns::PrimaryUse()
{
	Super::PrimaryUse();
}

void AGuns::SecondaryUse()
{
	Super::SecondaryUse();
}

void AGuns::ThirdUse()
{
	Super::ThirdUse();
}

void AGuns::ShootHitscan(FVector start, FVector direction)
{
	FVector end = start + direction * 500.0f;
	TArray<FHitResult> outHit;

	FCollisionQueryParams collisionParams;

	if (GetWorld()->LineTraceMultiByChannel(outHit, start, end, ECC_Visibility, collisionParams))
	{
		int i = 0;
		/*for each (auto var in outHit)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *outHit[i].GetComponent()->GetName()));
			i++;
		}*/
	}
}

void AGuns::ShootProjectile(FVector start, FVector direction)
{

}


