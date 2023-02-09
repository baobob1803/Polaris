#pragma once

#include "CoreMinimal.h"
#include "Weapons.h"
#include "Guns.generated.h"

/**
 * 
 */
UCLASS()
class POLARIS_API AGuns : public AWeapons
{
	GENERATED_BODY()

public:

	AGuns();

protected:
	virtual void BeginPlay() override;

	virtual void PrimaryUse() override;
	virtual void SecondaryUse() override;
	virtual void ThirdUse() override;

	//virtual void Shoot();

	void ShootHitscan(FVector start, FVector direction);
	void ShootProjectile(FVector start, FVector direction);

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle primaryShotInfo;

	UPROPERTY(EditAnywhere)
	FDataTableRowHandle secondaryShotInfo;
};
