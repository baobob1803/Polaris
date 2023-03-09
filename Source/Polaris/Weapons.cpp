// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons.h"

// Sets default values
AWeapons::AWeapons()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeapons::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapons::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapons::PrimaryUse(FVector start, FVector direction)
{
}

void AWeapons::SecondaryUse(FVector start, FVector direction)
{
}

void AWeapons::ThirdUse()
{
}

bool AWeapons::CheckAvailableEnergy(float* energyQuantity, bool isConsume, float cost)
{
	if (isConsume && *energyQuantity >= cost)
	{
		*energyQuantity -= cost;
		return true;
	}

	return false;
}

