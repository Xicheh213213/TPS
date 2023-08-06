// Fill out your copyright notice in the Description page of Project Settings.


#include "MagazinDefault.h"
#include "Kismet/GameplayStatics.h"

void AMagazinDefault::BeginPlay()
{
	Super::BeginPlay();
}

void AMagazinDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagazinDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AMagazinDefault::MagazinLogic()
{
	if (MagazinSetting.MagazinFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MagazinSetting.MagazinFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (MagazinSetting.MagazinSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), MagazinSetting.MagazinSound, GetActorLocation());
	}

	this->Destroy();
}
