// Fill out your copyright notice in the Description page of Project Settings.


#include "GilzaDefault.h"
#include "Kismet/GameplayStatics.h"

void AGilzaDefault::BeginPlay()
{
	Super::BeginPlay();
}

void AGilzaDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGilzaDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AGilzaDefault::ImpactProjectile()
{
}

void AGilzaDefault::GilzaLogic()
{
	if (GilzaSetting.GilzaFx)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GilzaSetting.GilzaFx, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (GilzaSetting.GilzaSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GilzaSetting.GilzaSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
}
