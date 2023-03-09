// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Weapons.generated.h"

UCLASS()
class POLARIS_API AWeapons : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapons();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UFUNCTION(BlueprintCallable)
		virtual void PrimaryUse(FVector start, FVector direction);

	UFUNCTION(BlueprintCallable)
		virtual void SecondaryUse(FVector start, FVector direction);

	UFUNCTION(BlueprintCallable)
		virtual void ThirdUse();

	bool CheckAvailableEnergy(float* energyQuantity, bool isConsume, float cost);
};
