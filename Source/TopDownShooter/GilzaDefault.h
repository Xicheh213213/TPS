// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileDefault.h"
#include "GilzaDefault.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTER_API AGilzaDefault : public AProjectileDefault
{
	GENERATED_BODY()
	
public:
	FGilzaInfo GilzaSetting;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;
	
	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void GilzaLogic();
};
