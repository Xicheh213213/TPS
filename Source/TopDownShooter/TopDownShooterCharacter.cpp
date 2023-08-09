 // Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownShooterCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPSGameInstance.h"
#include "Engine/World.h"

ATopDownShooterCharacter::ATopDownShooterCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTPSInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UTPSCharacterHealthComponent>(TEXT("HealthComponent"));

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATopDownShooterCharacter::InitWeapon);
	}
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATopDownShooterCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if(CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if(myPC)
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);

		}
	}
	MovementTick(DeltaSeconds);
}
void ATopDownShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}
}

void ATopDownShooterCharacter::SetupPlayerInputComponent(UInputComponent* NewInputComponent)
{
	Super::SetupPlayerInputComponent(NewInputComponent);
	NewInputComponent->BindAxis(TEXT("MoveForward"), this, &ATopDownShooterCharacter::InputAxisX);
	NewInputComponent->BindAxis(TEXT("MoveRight"), this, &ATopDownShooterCharacter::InputAxisY);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooterCharacter::InputAttackPressed);
	NewInputComponent->BindAction(TEXT("FireEvent"), EInputEvent::IE_Released, this, &ATopDownShooterCharacter::InputAttackRelesed);
	NewInputComponent->BindAction(TEXT("ReloadEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooterCharacter::TryReloadWeapon);
	NewInputComponent->BindAction(TEXT("SwitchWeaponNextEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooterCharacter::TrySwitchNextWeapon);
	NewInputComponent->BindAction(TEXT("SwitchWeaponPreviosEvent"), EInputEvent::IE_Pressed, this, &ATopDownShooterCharacter::TrySwitchPreviosWeapon);
}



void ATopDownShooterCharacter::InputAxisY(float Value)
{ 
	AxisY = Value;
}

void ATopDownShooterCharacter::InputAxisX(float Value)
{
	AxisX = Value;
}

void ATopDownShooterCharacter::InputAttackPressed()
{
	AttackCharEvent(true);
}

void ATopDownShooterCharacter::InputAttackRelesed()
{
	AttackCharEvent(false);
}

void ATopDownShooterCharacter::MovementTick(float Deltatime)
{

	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
	AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

	
	if (MovementState == EMovementState::Shift_State)
	{
		FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
		FRotator myRotator = myRotationVector.ToOrientationRotator();
		SetActorRotation((FQuat(myRotator)));
	}
	else 
	{


		APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (myController)
		{
			FHitResult ResultHit;
			myController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery6, false, ResultHit);
		
			
			float FindRotaterResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
			SetActorRotation(FQuat(FRotator(0.0f, FindRotaterResultYaw, 0.0f)));
			

			

			if (CurrentWeapon)
			{
				FVector Displacement = FVector(0);
				switch (MovementState)
				{
				case EMovementState::Aim_State:
					Displacement = FVector(0.0f, 0.0f, 160.0f);
					CurrentWeapon->ShouldReduceDispersion = true;
					break;
				case EMovementState::Walk_State:
					Displacement = FVector(0.0f, 0.0f, 100.0f);
					CurrentWeapon->ShouldReduceDispersion = true;
					break;
				case EMovementState::Run_State:
					Displacement = FVector(0.0f, 0.0f, 120.0f);
					CurrentWeapon->ShouldReduceDispersion = false;
					break;
				case EMovementState::Shift_State:
					break;
				default:
					break;
				}

				CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
			}

		}
	}

		
			

	
}

void ATopDownShooterCharacter::AttackCharEvent(bool bIsFire)
{
	
	if (MovementState != EMovementState::Shift_State)
	{
		
		
			AWeaponDefault* myWeapon = nullptr;
			myWeapon = GetCurrentWeapon();
			if (myWeapon)
			{
				myWeapon->SetWeaponStateFire(bIsFire);
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("ATopDownShooterCharacter::AttackCharEvent - CurrentWeapon -NULL"));
		
	}
}


void ATopDownShooterCharacter::CharacterUpdate()
{
	float ResSpeed = 600.f;
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunState;
		break;
	case EMovementState::Shift_State:
		ResSpeed = MovementInfo.ShiftState;
		break;
	default:
		break;
	}
	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}
void ATopDownShooterCharacter::ChangeMovementState(EMovementState NewMovementState)
{
	MovementState = NewMovementState;
	CharacterUpdate();
}

void ATopDownShooterCharacter::WeaponReloadStart(UAnimMontage*Anim)
{
	NativeEventReloadAnimStart(Anim);
}

void ATopDownShooterCharacter::WeaponFireStart()
{
	if (InventoryComponent && CurrentWeapon)
		InventoryComponent->SetAddicionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AddicionalWeaponInfo);
	NativeEventFireStart();


	
}

void ATopDownShooterCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent&&CurrentWeapon)
	{
		InventoryComponent->WeaponChangeAmmo(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAddicionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AddicionalWeaponInfo);
	}
	NativeEventReloadAnimEnd(bIsSuccess);
}

void ATopDownShooterCharacter::NativeEventFireStart_Implementation()
{
}

void ATopDownShooterCharacter::NativeEventReloadAnimStart_Implementation(UAnimMontage*Anim)
{

}

void ATopDownShooterCharacter::NativeEventReloadAnimEnd_Implementation(bool bIsSuccess)
{

}

AWeaponDefault* ATopDownShooterCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATopDownShooterCharacter::InitWeapon(FName IdWeaponName, FAddicionalWeaponInfo WeaponAddicionalInfo, int32 NewCurrentIndexWeapon)
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if(myGI)
	{
		if(myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))	
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("hand_rSocket"));
					CurrentWeapon = myWeapon;
					myWeapon->WeaponSetting = myWeaponInfo;
					//myWeapon->AddicionalWeaponInfo.Round = myWeaponInfo.MaxRound;
					myWeapon->ReloadTime = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon(MovementState);
					myWeapon->AddicionalWeaponInfo = WeaponAddicionalInfo;
					CurrentIndexWeapon = NewCurrentIndexWeapon;
					
					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ATopDownShooterCharacter::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ATopDownShooterCharacter::WeaponReloadEnd);	
					myWeapon->OnWeaponFireStart.AddDynamic(this, &ATopDownShooterCharacter::WeaponFireStart);
					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
						CurrentWeapon->InitReload();
					if(InventoryComponent)
					InventoryComponent->OnWeaponAmmoAvaible.Broadcast(myWeapon->WeaponSetting.WeaponType);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ATopDownShooterCharacter::InitWeapon - Weapon not found in table -NULL"));
		}
	}
	
}
void ATopDownShooterCharacter::TryReloadWeapon()
{
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound&&CurrentWeapon->CheckCanWeaponReload())
		CurrentWeapon->InitReload();
	}
}

void ATopDownShooterCharacter::TrySwitchNextWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AddicionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();
			
		}
		if(InventoryComponent)
		{
			if(InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon+1,OldIndex,OldInfo,true))
			{ }
		}
	
	}
}

void ATopDownShooterCharacter::TrySwitchPreviosWeapon()
{
	if (InventoryComponent->WeaponSlots.Num() > 1)
	{
		int8 OldIndex = CurrentIndexWeapon;
		FAddicionalWeaponInfo OldInfo;
		if (CurrentWeapon)
		{
			OldInfo = CurrentWeapon->AddicionalWeaponInfo;
			if (CurrentWeapon->WeaponReloading)
				CurrentWeapon->CancelReload();

		}
		if (InventoryComponent)
		{
			if (InventoryComponent->SwitchWeaponToIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo,false))
			{ }
		}

	}
}


UDecalComponent* ATopDownShooterCharacter::GetCursorToWorld()
{
	return CurrentCursor;
}



