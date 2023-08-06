// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "WeaponDefault.h"
#include "TPSInventoryComponent.h"
#include "TopDownShooterCharacter.generated.h"

UCLASS(Blueprintable)
class ATopDownShooterCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	ATopDownShooterCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	
	// позволяет переопределить любые кнопки.
	virtual void SetupPlayerInputComponent(class UInputComponent* InputConponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UTPSInventoryComponent* InventoryComponent;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UDecalComponent* CursorToWorld;


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		EMovementState MovementState = EMovementState::Run_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FCharacterSpeed MovementInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
		FVector CursorSize = FVector(20.f, 40.f, 40.f);
	//Weapon	
	AWeaponDefault* CurrentWeapon = nullptr;


	UDecalComponent* CurrentCursor = nullptr;
	UFUNCTION()
		void InputAxisY(float Value);
	UFUNCTION()
		void InputAxisX(float Value);
	UFUNCTION()
		void InputAttackPressed();
	UFUNCTION()
		void InputAttackRelesed();
	float AxisX = 0.0f;
	float AxisY = 0.0f;
//TickFunction
	UFUNCTION()
	void MovementTick(float Deltatime);
	UFUNCTION(BlueprintCallable)
		void AttackCharEvent(bool bIsFire);
	UFUNCTION(BlueprintCallable)
		void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
		void ChangeMovementState(EMovementState NewMovementState);
	//WeaponReload
	UFUNCTION()
		void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
		void WeaponFireStart();
	UFUNCTION()
		void WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake);
	//Native Events
	UFUNCTION(BlueprintNativeEvent)
		void NativeEventFireStart();
	UFUNCTION(BlueprintNativeEvent)
		void NativeEventReloadAnimStart(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
		void NativeEventReloadAnimEnd(bool bIsSuccess);
	

	UFUNCTION(BlueprintCallable)
		AWeaponDefault* GetCurrentWeapon();
			UFUNCTION(BlueprintCallable)

		void InitWeapon(FName IdWeaponName,FAddicionalWeaponInfo WeaponAddicionalInfo, int32 NewCurrentIndexWeapon);

		UFUNCTION(BlueprintCallable)
		void TryReloadWeapon();
		UFUNCTION(BlueprintCallable)
			void TrySwitchNextWeapon();
		UFUNCTION(BlueprintCallable)
			void TrySwitchPreviosWeapon();
		UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
			int32 CurrentIndexWeapon = 0;
	UFUNCTION(BlueprintCallable)
		UDecalComponent* GetCursorToWorld();
};

