// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileDefault.h"
#include "MagazinDefault.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API AMagazinDefault : public AProjectileDefault
{
	GENERATED_BODY()
	
public:
	FMagazinInfo MagazinSetting;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void MagazinLogic();
};
