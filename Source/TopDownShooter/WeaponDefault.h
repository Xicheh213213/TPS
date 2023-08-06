// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Types.h"
#include "ProjectileDefault.h"
#include "GilzaDefault.h"
#include "MagazinDefault.h"

#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart,UAnimMontage*,Anim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFireStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd,bool, bIsSuccess,int32, AmmoSafe);

UCLASS()
class TOPDOWNSHOOTER_API AWeaponDefault : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AWeaponDefault();
	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponFireStart OnWeaponFireStart;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UArrowComponent* ShootLocation = nullptr;
	UPROPERTY(VisibleAnywhere)
		FWeaponInfo WeaponSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		FAddicionalWeaponInfo AddicionalWeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void WeaponInit();

	float GetCurrentDispersion()const;

	FVector ApplyDispersionToShoot(FVector DirectionShoot)const;

	void DispersionTick(float DeltaTime);

	FVector GetFireEndLocation()const;
	int8 GetNumberProjectileByShot() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	bool WeaponReloading=false;

	void SetWeaponStateFire(bool bIsFire);
	bool CheckWeaponCanFire();
	FMagazinInfo GetMagazin();
	FGilzaInfo GetGilza();
	FProjectileInfo GetProjectile();
	UFUNCTION()
	void Fire();


	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersionByShot();
	void ChangeDispersion();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlocFire")
	bool BlocFire = false;
	
	float FireTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTimer = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTime = 0.0f;

	bool ShouldReduceDispersion = false;

	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	int32 GetWeaponRound();

	void InitReload();
	void FinishReload();
	void CancelReload();
	bool CheckCanWeaponReload();
	int8 GetAviableAmmoForReload();

	FVector ShootEndLocation = FVector(0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Debug")
	bool ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 100.0f;

	
};
