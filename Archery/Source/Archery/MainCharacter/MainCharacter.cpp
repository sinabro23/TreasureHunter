// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MainAnimInstance.h"
#include "FireBall.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "MainPlayerController.h"
#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "RoomGate.h"
#include "SpawningPoint.h"
#include "Components/AudioComponent.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 280.f;
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 기준으로 회전한다
	CameraBoom->SocketOffset = FVector(0.f, 60.f, 90.f); // 카메라가 달릴 소켓을 움직임

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 컨트롤러따라 회전하면 안됨.

	// 마우스 돌리면 캐릭터가 회전할지안할지
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;  // Yaw는 컨트롤러에 따라 회전하게
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; // 인풋 들어오는 방향으로 캐릭터가 회전하지않음
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // 이 로테이션레이트로 회전함
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	SkillRangeParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SkillRangeParticle"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_SKILLRANGE(TEXT("ParticleSystem'/Game/RPGEffects/Particles/P_Targeting_Player_Select.P_Targeting_Player_Select'"));
	if (PS_SKILLRANGE.Succeeded())
	{
		SkillRangeParticle->SetTemplate(PS_SKILLRANGE.Object);
	}
	SkillRangeParticle->SetActive(true);
	SkillRangeParticle->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_METEOR(TEXT("ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Meteor/FX/P_Gideon_Meteor_Shower.P_Gideon_Meteor_Shower'"));
	if (PS_METEOR.Succeeded())
	{
		MeteorParticle = PS_METEOR.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_METEORAREA(TEXT("ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Meteor/FX/P_Gideon_Meteor_Portal_Fast.P_Gideon_Meteor_Portal_Fast'"));
	if (PS_METEORAREA.Succeeded())
	{
		MeteorAreaParticle = PS_METEORAREA.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_BlackholeCast(TEXT("ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Ultimate/FX/P_Gideon_Ultimate_Cast.P_Gideon_Ultimate_Cast'"));
	if (PS_BlackholeCast.Succeeded())
	{
		BlackholeCastParticle = PS_BlackholeCast.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_BlackholeShot(TEXT("ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Ultimate/FX/P_Gideon_Ultimate.P_Gideon_Ultimate'"));
	if (PS_BlackholeShot.Succeeded())
	{
		BlackholeUltimateParticle = PS_BlackholeShot.Object;
	}

	FireShieldParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireShield"));
	FireShieldParticle->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_FireShield(TEXT("ParticleSystem'/Game/RPGEffects/Particles/P_Mage_Fire_Shield.P_Mage_Fire_Shield'"));
	if (PS_FireShield.Succeeded())
	{
		FireShieldParticle->SetTemplate(PS_FireShield.Object);
	}

	PotionParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PotionParticle"));
	PotionParticle->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PS_POTION(TEXT("ParticleSystem'/Game/RPGEffects/Particles/P_Priest_Heal_Over_Time_3D.P_Priest_Heal_Over_Time_3D'"));
	if (PS_POTION.Succeeded())
	{
		PotionParticle->SetTemplate(PS_POTION.Object);
	}

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));


}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	MainPlayerController = Cast<AMainPlayerController>(GetController());
	FireShieldParticle->SetHiddenInGame(true);
	PotionParticle->SetHiddenInGame(true);
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsShieldOn)
	{
		float DamageAmoutOnShield = DamageAmount / 10.f;
		if (CurrentHP - DamageAmoutOnShield <= 0.f)
		{
			CurrentHP = 0.f;
			Die();
		}
		else
		{
			CurrentHP -= DamageAmoutOnShield;
		}

		return DamageAmoutOnShield;
	}
	else
	{
		if (CurrentHP - DamageAmount <= 0.f)
		{
			CurrentHP = 0.f;
			Die();
		}
		else
		{
			CurrentHP -= DamageAmount;
		}

		return DamageAmount;
	}

}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MeteorSkillTrail();
	BlackholeTrail();
	SetCharacterMovementSpeed();

	// 함수로빼기
	if (CharacterState == ECharacterState::ECS_Cast)
	{
		CurrentCastingTime = GetWorldTimerManager().GetTimerElapsed(ESkillTimer);
	}

	PotionParticle->SetHiddenInGame(true);

	if (IsDrinkingHPPotion)
	{
		SetHP(CurrentHP += DeltaTime * HPPotionHealAmount);
		PotionParticle->SetActive(true);
		PotionParticle->SetHiddenInGame(false);
	}

	if (IsDrinkingMPPotion)
	{
		SetMP(CurrentMP += DeltaTime * MPPotionHealAmount);
		PotionParticle->SetActive(true);
		PotionParticle->SetHiddenInGame(false);
	}
	
	if (IsShieldOn)
	{
		SetMP(CurrentMP -= DeltaTime * 10.f);
		if (CurrentMP <= 5.f)
		{
			FireShieldOff();
		}
	}

	
	CurrentMPCheck();
	
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMainCharacter::Turn); // 마우스 이동은 함수 구현 안하고 바로
	PlayerInputComponent->BindAxis("LookUp", this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("EKey", EInputEvent::IE_Pressed, this, &AMainCharacter::EkeyPressed);
	PlayerInputComponent->BindAction("OneKey", EInputEvent::IE_Pressed, this, &AMainCharacter::OneKeyPressed);
	PlayerInputComponent->BindAction("TwoKey", EInputEvent::IE_Pressed, this, &AMainCharacter::TwoKeyPressed);
	PlayerInputComponent->BindAction("ThreeKey", EInputEvent::IE_Pressed, this, &AMainCharacter::ThreeKeyPressed);
	PlayerInputComponent->BindAction("FourKey", EInputEvent::IE_Pressed, this, &AMainCharacter::FourKeyPressed);
	PlayerInputComponent->BindAction("QKey", EInputEvent::IE_Pressed, this, &AMainCharacter::QKeyPressed);
	PlayerInputComponent->BindAction("RMBButton", EInputEvent::IE_Pressed, this, &AMainCharacter::RMBButtonPressed);
	PlayerInputComponent->BindAction("RMBButton", EInputEvent::IE_Released, this, &AMainCharacter::RMBButtonReleased);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMainCharacter::FireWeapon);
	PlayerInputComponent->BindAction("NextSkill", IE_Pressed, this, &AMainCharacter::SkillChange);
	PlayerInputComponent->BindAction("FKey", IE_Pressed, this, &AMainCharacter::FKeyPressed);

	PlayerInputComponent->BindAction("Tap", EInputEvent::IE_Pressed, this, &AMainCharacter::TapKeyPressed);
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	MainAnim = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());
	if (MainAnim)
	{
		MainAnim->OnSendFireBall.AddUObject(this, &AMainCharacter::SendFireBall);
		MainAnim->OnFireBallEnd.AddUObject(this, &AMainCharacter::AttackEnd);
		MainAnim->OnBurdenEnd.AddUObject(this, &AMainCharacter::BurdenEnd);
		MainAnim->OnBurdenFire.AddUObject(this, &AMainCharacter::SendBurden);
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if (bIsBurden)
		return;

	if (Controller && Value != 0.f)
	{
		// 컨트롤러기준 정면 구하기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // X가 정면 방향이니깐
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (bIsBurden)
		return;

	if (Controller && Value != 0.f)
	{
		// 컨트롤러기준 정면 구하기
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // Y가 우측 방향이니깐
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::Turn(float Value)
{
	float TurnScaleFactor = 0.0f;
	TurnScaleFactor = MouseHipTurnRate;
	AddControllerYawInput(Value * TurnScaleFactor);
}

void AMainCharacter::LookUp(float Value)
{
	float LookUpScaleFactor = 0.0f;
	LookUpScaleFactor = MouseHipLookUpRate;
	AddControllerPitchInput(Value * LookUpScaleFactor);
}

void AMainCharacter::FireWeapon()
{
	if (bIsAttacking || bIsBurden)
		return;

	bIsAttacking = true;

	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("FireBall"));
	}
}

void AMainCharacter::MeteorKeyPressed()
{
	if (CurrentMP < MeteorMPAMount)
		return;

	if (bIsAttacking || CharacterState == ECharacterState::ECS_Cast)
		return;

	IsEKeyPressed = true;
}

void AMainCharacter::MeteorKeyReleased()
{
	if (bIsAttacking || !IsEKeyPressed)
		return;

	IsEKeyPressed = false;
	SkillRangeParticle->SetHiddenInGame(true);
	CurrentSkillName = FName("Meteor");
	// TODO ESkill 발사

	CurrentSkillMaxCastingTime = ESkillCastingTime;
	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("Cast"));
	}

	bIsCasting = true;
	
	CharacterState = ECharacterState::ECS_Cast;
	if (MainPlayerController)
	{
		MainPlayerController->SetWidgetVisiblity(true);
	}

	GetWorldTimerManager().SetTimer(
		ESkillTimer,
		this,
		&AMainCharacter::SendMeteor,
		ESkillCastingTime);
}

void AMainCharacter::MeteorSkillTrail()
{

	if (IsEKeyPressed)
	{

		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldPosition,
			CrosshairWorldDirection);

		if (bScreenToWorld) 
		{
			FHitResult ScreenTraceHit;
			const FVector Start = CrosshairWorldPosition;
			const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * ESkillRange;

			FVector BeamEndPoint = End;

			GetWorld()->LineTraceSingleByChannel(
				ScreenTraceHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility);
			if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
			{
				BeamEndPoint = ScreenTraceHit.Location;
				MeteorPosition = ScreenTraceHit.Location + FVector(0.0f, 0.0f, 1000.f);
				//DrawDebugLine(GetWorld(), Start, BeamEndPoint, FColor::Red, false, 2.f);
				//DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 2.f);
				SkillRangeParticle->SetHiddenInGame(false);
				SkillRangeParticle->SetRelativeLocation(ScreenTraceHit.Location + FVector(0.0f, 0.0f, 10.f));
			}
		}

		
	}
}

void AMainCharacter::RMBButtonPressed()
{

	switch (CurrentSkill)
	{
	case ECharacterSkill::ECS_Meteor:
		MeteorKeyPressed();
		break;
	case ECharacterSkill::ECS_Burden:
		BurdenButtonPressed();
		break;
	case ECharacterSkill::ECS_BlackHole:
		BlackHoleSkillPressed();
		break;
	case ECharacterSkill::ECS_FireShield:
		FireShieldOn();
		break;
	case ECharacterSkill::ECS_MAX:
		break;
	default:
		break;
	}
}

void AMainCharacter::RMBButtonReleased()
{

	switch (CurrentSkill)
	{
	case ECharacterSkill::ECS_Meteor:
		MeteorKeyReleased();
		break;
	case ECharacterSkill::ECS_Burden:
		break;
	case ECharacterSkill::ECS_BlackHole:
		BlackholeKeyReleased();
		break;
	case ECharacterSkill::ECS_FireShield:
		FireShieldOff();
		break;
	case ECharacterSkill::ECS_MAX:
		break;
	default:
		break;
	}

}

void AMainCharacter::SendFireBall()
{
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName("HandSocket");
	if (HandSocket)
	{
		const FTransform HandSocketTransform = HandSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, HandSocketTransform);
		}

		// 현재 뷰포트의 사이즈 구하기.
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		// 크로스헤어 위치 구하기.
		FVector2D CrosshairLocation = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		// 크로스헤어 월드 포지션을 넣어줄 변수
		FVector CrosshairPosition;
		FVector CrosshairDirection;

		// 크로스헤어 위치의 2D화면 좌표를 3D월드 스페이스로 변환
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairPosition,
			CrosshairDirection);

		// 투영이 성공한다면
		if (bScreenToWorld) 
		{
			FHitResult TraceHitResult;
			const FVector StartVector	= CrosshairPosition;
			// 크로스헤어로 부터 시작된 끝지점
			const FVector EndVector		= CrosshairPosition + CrosshairDirection * FireballRange;

			// Fireball이 날라갈 최종 위치를 넣어줄 변수
			FVector FinalEndPoint		= EndVector;

			// 크로스헤어로부터 무언가 부딪힌다면 그방향으로 향하게  위치값 구하기.
			GetWorld()->LineTraceSingleByChannel(TraceHitResult, StartVector, EndVector, ECollisionChannel::ECC_Visibility);
			if (TraceHitResult.bBlockingHit) 
			{
				// Beam end point is now trace hit location
				FinalEndPoint			= TraceHitResult.Location;
			}
			
			AFireBall* Fireball = GetWorld()->SpawnActor<AFireBall>(HandSocketTransform.GetLocation(), GetActorRotation());
			Fireball->StartFireBall(CrosshairDirection, this);
		}
	}
}

void AMainCharacter::SendMeteor()
{

	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("Rift"));

		if (MeteorParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorParticle, MeteorPosition);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorAreaParticle, MeteorPosition);
		}
	}

	bIsCasting = false;
	CharacterState = ECharacterState::ECS_Normal;
	if (MainPlayerController)
	{
		MainPlayerController->SetWidgetVisiblity(false);
	}

	if (MeteorSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MeteorSound);
	}

	/////
	MeteorAttackCheck();
	GetWorldTimerManager().SetTimer(MeteorTimer, this, &AMainCharacter::MeteorRepeat, MeteorTime);


	SetMP(CurrentMP - MeteorMPAMount);

}

void AMainCharacter::SendBurden()
{
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName("HandSocket");
	if (HandSocket)
	{
		const FTransform SocketTransform = HandSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}

		// Get current size of the viewport
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		// Get screen space location of crosshairs
		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		// Get world position and direction of crosshairs
		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldPosition,
			CrosshairWorldDirection);

		if (bScreenToWorld) // was deprojection successful?
		{
			FHitResult ScreenTraceHit;
			const FVector Start = CrosshairWorldPosition;
			const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f;

			// Set beam end point to line trace end point
			FVector BeamEndPoint = End;

			// Trace outward from crosshairs world location
			GetWorld()->LineTraceSingleByChannel(
				ScreenTraceHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility);
			if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
			{
				// Beam end point is now trace hit location
				BeamEndPoint = ScreenTraceHit.Location;
				//DrawDebugLine(GetWorld(), Start, BeamEndPoint, FColor::Red, false, 2.f);
				//DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 2.f);
			}


			FVector FireballLocation = End - SocketTransform.GetLocation();
			FireballLocation.Normalize();

			AFireBall* Fireball = GetWorld()->SpawnActor<AFireBall>(SocketTransform.GetLocation(), GetActorRotation());
			Fireball->StartFireBall(FireballLocation, this);


			SetMP(CurrentMP - FireballMPAMount);
		}
	}
}

void AMainCharacter::AttackEnd()
{
	bIsAttacking = false;
}

void AMainCharacter::BurdenEnd()
{
	bIsBurden = false;	
}

bool AMainCharacter::IsCasting()
{
	return CharacterState == ECharacterState::ECS_Cast;
}

void AMainCharacter::SetCharacterMovementSpeed()
{
	switch (CharacterState)
	{
	case ECharacterState::ECS_Normal:
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		break;
	case ECharacterState::ECS_Cast:
		GetCharacterMovement()->MaxWalkSpeed = CastingMovementSpeed;
		break;
	case ECharacterState::ECS_Shield:
		GetCharacterMovement()->MaxWalkSpeed = ShieldMovementSpeed;
		break;
	case ECharacterState::ECS_MAX:
		break;
	default:
		break;
	}
}

float AMainCharacter::GetESkillRatio()
{
	return 0.0f;
}

void AMainCharacter::SkillChange()
{
	if (bIsCasting)
		return;

	switch (CurrentSkill)
	{
	case ECharacterSkill::ECS_Meteor:
		CurrentSkill = ECharacterSkill::ECS_Burden;
		CurrentSkillName = FName("Fireball");
		break;
	case ECharacterSkill::ECS_Burden:
		CurrentSkill = ECharacterSkill::ECS_BlackHole;
		CurrentSkillName = FName("BlackHole");
		break;
	case ECharacterSkill::ECS_BlackHole:
		CurrentSkill = ECharacterSkill::ECS_FireShield;
		CurrentSkillName = FName("FireShield");
		break;
	case ECharacterSkill::ECS_FireShield:
		CurrentSkill = ECharacterSkill::ECS_Meteor;
		CurrentSkillName = FName("Meteor");
		break;
	case ECharacterSkill::ECS_MAX:
		break;
	default:
		break;
	}
}

void AMainCharacter::MeteorAttackCheck()
{
	FVector Center = MeteorPosition - FVector(0.0f, 0.0f, 1000.f);
	
	TArray<FOverlapResult> HitResults;
	FCollisionQueryParams CollsionQueryParam(NAME_None, false, this);

	bool bResult = GetWorld()->OverlapMultiByChannel(
		HitResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(400.f),
		CollsionQueryParam
	);

	if (bResult)
	{
		for (auto HitResult : HitResults)
		{
			if (HitResult.Actor.IsValid())
			{
				AEnemy* HitEnemy = Cast<AEnemy>(HitResult.Actor);
				
				if (HitEnemy)
				{
					HitEnemy->OnAttacked(MeteorDamage, this);
				}
			}
		}
	}

	//DrawDebugSphere(GetWorld(), Center, 400.f, 16, FColor::Green, false, 1.f);

}

void AMainCharacter::Die()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(FName("Death"), DeathMontage);
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetWorldTimerManager().SetTimer(DieTimerHandle, this, &AMainCharacter::RestartLevel, DieTime);
}

void AMainCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
}

void AMainCharacter::BurdenButtonPressed()
{
	if (bIsBurden || CurrentMP < FireballMPAMount || bIsAttacking)
		return;

	bIsBurden = true;
	if (MainAnim && BurdenMontage)
	{
		MainAnim->Montage_Play(BurdenMontage);
		MainAnim->Montage_JumpToSection(FName("Burden"));
	}
}

void AMainCharacter::BlackHoleSkillPressed()
{
	if (bIsAttacking || CharacterState == ECharacterState::ECS_Cast || CurrentMP < BlackholeMPAMount)
		return;

	IsBlackholeKeyPressed = true;
}

void AMainCharacter::BlackholeTrail()
{
	if (IsBlackholeKeyPressed)
	{

		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;

		bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
			UGameplayStatics::GetPlayerController(this, 0),
			CrosshairLocation,
			CrosshairWorldPosition,
			CrosshairWorldDirection);

		if (bScreenToWorld)
		{
			FHitResult ScreenTraceHit;
			const FVector Start = CrosshairWorldPosition;
			const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * ESkillRange;

			FVector BeamEndPoint = End;

			GetWorld()->LineTraceSingleByChannel(
				ScreenTraceHit,
				Start,
				End,
				ECollisionChannel::ECC_Visibility);
			if (ScreenTraceHit.bBlockingHit) // was there a trace hit?
			{
				BeamEndPoint = ScreenTraceHit.Location;
				BlackholePosition = ScreenTraceHit.Location;
				//DrawDebugLine(GetWorld(), Start, BeamEndPoint, FColor::Red, false, 2.f);
				//DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 2.f);
				SkillRangeParticle->SetHiddenInGame(false);
				SkillRangeParticle->SetRelativeLocation(ScreenTraceHit.Location + FVector(0.0f, 0.0f, 10.f));
			}
		}

	}
}

void AMainCharacter::BlackholeKeyReleased()
{
	if (bIsAttacking || !IsBlackholeKeyPressed)
		return;

	IsBlackholeKeyPressed = false;
	SkillRangeParticle->SetHiddenInGame(true);
	CurrentSkillName = FName("BlackHole");
	// TODO ESkill 발사

	CurrentSkillMaxCastingTime = BlackholeSkillCastingTime;
	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("Cast"));
	}

	bIsCasting = true;

	CharacterState = ECharacterState::ECS_Cast;
	if (MainPlayerController)
	{
		MainPlayerController->SetWidgetVisiblity(true);
	}

	GetWorldTimerManager().SetTimer(
		ESkillTimer,
		this,
		&AMainCharacter::SendBlackhole,
		BlackholeSkillCastingTime);
}

void AMainCharacter::SendBlackhole()
{
	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("Rift"));

		if (MeteorParticle && BlackholeUltimateParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlackholeCastParticle, BlackholePosition);
		    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlackholeUltimateParticle, BlackholePosition);
		}
	}

	bIsCasting = false;
	CharacterState = ECharacterState::ECS_Normal;
	if (MainPlayerController)
	{
		MainPlayerController->SetWidgetVisiblity(false);
	}

	SetMP(CurrentMP - BlackholeMPAMount);

	if (BlackholeSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BlackholeSound);
	}

	BlackholeAttackCheck();
}

void AMainCharacter::BlackholeAttackCheck()
{
	FVector Center = BlackholePosition;
	// 피격될 몬스터의 정보가 들어갈 Array
	TArray<FOverlapResult> HitResults;
	FCollisionQueryParams CollsionQueryParam(NAME_None, false, this);

	bool bResult = GetWorld()->OverlapMultiByChannel(
		HitResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(BlackholeRange),
		CollsionQueryParam
	);

	if (bResult)
	{
		for (auto HitResult : HitResults)
		{
			if (HitResult.Actor.IsValid())
			{
				AEnemy* HitEnemy = Cast<AEnemy>(HitResult.Actor);

				if (HitEnemy)
				{
					HitEnemy->OnAttackedBlackhole(BlackholeDamage, this);
				}
			}
		}
	}

}

void AMainCharacter::SetHP(float NewHP)
{
	CurrentHP = NewHP;

	if(NewHP > MaxHP)
	{
		CurrentHP = MaxHP;
	}

	if (CurrentHP < 0.f)
	{
		CurrentHP = 0.f;
		Die();
	}
}

float AMainCharacter::GetCurrentHP()
{
	return CurrentHP;
}

void AMainCharacter::SetMP(float NewMP)
{
	CurrentMP = NewMP;

	if (NewMP > MaxMP)
	{
		CurrentMP = MaxHP;
	}

	if (CurrentMP < 0.f)
	{
		CurrentMP = 0.f;
	}
}

float AMainCharacter::GetCurrentMP()
{
	return CurrentMP;
}

void AMainCharacter::TakeHPPotion()
{
	UGameplayStatics::PlaySound2D(GetWorld(), TakeItemSound);
	HPPotionCount += 1;
}

void AMainCharacter::TakeMPPotion()
{
	UGameplayStatics::PlaySound2D(GetWorld(), TakeItemSound);
	MPPotionCount += 1;
}

void AMainCharacter::TakeCoin(int32 CoinAmount)
{
	UGameplayStatics::PlaySound2D(GetWorld(), TakeCoinSound);
	CurrentCoinCount += CoinAmount;
}

void AMainCharacter::DrinkHPPotion()
{
	if (HPPotionCount <= 0)
		return;

	HPPotionCount--;
	IsDrinkingHPPotion = true;
	GetWorldTimerManager().SetTimer(HPPotionTimer, this, &AMainCharacter::EndHPHealing, 1.f);
	//SetHP(CurrentHP + HPPotionHealAmount);
}

void AMainCharacter::DrinkMPPtion()
{
	if (MPPotionCount <= 0)
		return;

	MPPotionCount--;
	IsDrinkingMPPotion = true;
	GetWorldTimerManager().SetTimer(MPPotionTimer, this, &AMainCharacter::EndMPHealing, 1.f);
}

void AMainCharacter::OneKeyPressed()
{
	if (!IsTapOn || bIsCasting)
		return;

	CurrentSkill = ECharacterSkill::ECS_Burden;
	CurrentSkillName = FName("FireBall");
	TapKeyPressed();
}

void AMainCharacter::TwoKeyPressed()
{
	if (!IsTapOn || bIsCasting)
		return;

	CurrentSkill = ECharacterSkill::ECS_Meteor;
	CurrentSkillName = FName("Meteor");
	TapKeyPressed();
}

void AMainCharacter::ThreeKeyPressed()
{
	if (!IsTapOn || bIsCasting)
		return;

	CurrentSkill = ECharacterSkill::ECS_FireShield;
	CurrentSkillName = FName("FireShield");
	TapKeyPressed();
}

void AMainCharacter::FourKeyPressed()
{
	if (!IsTapOn || bIsCasting)
		return;

	CurrentSkill = ECharacterSkill::ECS_BlackHole;
	CurrentSkillName = FName("BlackHole");
	TapKeyPressed();
}

void AMainCharacter::QKeyPressed()
{
	switch (CurrentItem)
	{
	case ECharacterItem::ECI_HPPotion:
		CurrentItem = ECharacterItem::ECI_MPPotion;
		break;
	case ECharacterItem::ECI_MPPotion:
		CurrentItem = ECharacterItem::ECI_HPPotion;
		break;
	case ECharacterItem::ECI_MAX:
		break;
	default:
		break;
	}
}

void AMainCharacter::EkeyPressed()
{
	if (DrinkPotionSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DrinkPotionSound);
	}

	switch (CurrentItem)
	{
	case ECharacterItem::ECI_HPPotion:
		DrinkHPPotion();
		break;
	case ECharacterItem::ECI_MPPotion:
		DrinkMPPtion();
		break;
	case ECharacterItem::ECI_MAX:
		break;
	default:
		break;
	}
}

void AMainCharacter::EndHPHealing()
{
	IsDrinkingHPPotion = false;
	if (CurrentHP > 100.f)
		CurrentHP = 100.f;

	PotionParticle->SetHiddenInGame(true);
}

void AMainCharacter::EndMPHealing()
{
	IsDrinkingMPPotion = false;
	if (CurrentMP > 100.f)
		CurrentMP = 100.f;

	PotionParticle->SetHiddenInGame(true);
}

void AMainCharacter::FireShieldOn()
{
	if (IsShieldOn)
		return;
	FireShieldParticle->SetActive(true, true);
	IsShieldOn = true;
	CharacterState = ECharacterState::ECS_Shield;
	FireShieldParticle->SetHiddenInGame(false);

	if (FireShieldSound)
	{
		AudioComponent = UGameplayStatics::SpawnSound2D(this, FireShieldSound);
		AudioComponent->SetActive(true);
	}
	
}

void AMainCharacter::FireShieldOff()
{
	IsShieldOn = false;
	CharacterState = ECharacterState::ECS_Normal;
	FireShieldParticle->SetHiddenInGame(true);
	if (FireShieldSound)
	{
		AudioComponent->SetActive(false);
	}
}

void AMainCharacter::MeteorRepeat()
{
	if (MeteorCount > 3)
	{
		MeteorCount = 0;
		return;
	}

	MeteorCount++;

	FVector Center = MeteorPosition - FVector(0.0f, 0.0f, 1000.f);
	TArray<FOverlapResult> HitResults;
	FCollisionQueryParams CollsionQueryParam(NAME_None, false, this);

	bool bResult = GetWorld()->OverlapMultiByChannel(
		HitResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(400.f),
		CollsionQueryParam
	);

	if (bResult)
	{
		for (auto HitResult : HitResults)
		{
			if (HitResult.Actor.IsValid())
			{
				AEnemy* HitEnemy = Cast<AEnemy>(HitResult.Actor);

				if (HitEnemy)
				{
					HitEnemy->OnAttacked(MeteorDamage, this);
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(MeteorTimer, this, &AMainCharacter::MeteorRepeat, MeteorTime);
}

void AMainCharacter::TapKeyPressed()
{
	if (IsTapOn)
	{
		IsTapOn = false;
		MainPlayerController->SetTapHUDVisibility(false);
	}
	else
	{
		IsTapOn = true;
		MainPlayerController->SetTapHUDVisibility(true);
	}

	UGameplayStatics::PlaySound2D(GetWorld(), TapSound);
}

void AMainCharacter::CurrentMPCheck()
{
	switch (CurrentSkill)
	{
	case ECharacterSkill::ECS_Meteor:
		if (CurrentMP < MeteorMPAMount)
		{
			IsCurrentSkillAvailable = false;	
		}
		else
		{
			IsCurrentSkillAvailable = true;
		}
		break;
	case ECharacterSkill::ECS_Burden:
		if (CurrentMP < FireballMPAMount)
		{
			IsCurrentSkillAvailable = false;
		}
		else
		{
			IsCurrentSkillAvailable = true;
		}
		break;
	case ECharacterSkill::ECS_BlackHole:
		if (CurrentMP < BlackholeMPAMount)
		{
			IsCurrentSkillAvailable = false;
		}
		else
		{
			IsCurrentSkillAvailable = true;
		}
		break;
	case ECharacterSkill::ECS_FireShield:
		if (CurrentMP < 5.f)
		{
			IsCurrentSkillAvailable = false;
		}
		else
		{
			IsCurrentSkillAvailable = true;
		}
		break;
	case ECharacterSkill::ECS_MAX:
		break;
	default:
		break;
	}
}

void AMainCharacter::OnHealingSpot()
{
	IsDrinkingHPPotion = true;
	IsDrinkingMPPotion = true;
}

void AMainCharacter::OutHealingSpot()
{
	IsDrinkingHPPotion = false;
	IsDrinkingMPPotion = false;
}

void AMainCharacter::SetIsOnGate(bool bIsOn)
{
	IsOnGate = bIsOn;
}

void AMainCharacter::FKeyPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("FKEY"));
	if (IsOnGate)
	{
		if (GateRoom)
		{
			int32 RemainCoinAmount = GateRoom->GetMaxCoinAmount() - GateRoom->GetCurrentCoinAmount();
			if (RemainCoinAmount >= CurrentCoinCount)
			{
				GateRoom->PutCurrentCoin(CurrentCoinCount);
				CurrentCoinCount = 0;
			}
			else
			{
				GateRoom->PutCurrentCoin(RemainCoinAmount);
				CurrentCoinCount -= RemainCoinAmount;
			}
		}
	}

	if (IsOnSpawnButton)
	{
		if (SpawnPoint)
		{
			SpawnPoint->PressButton();
		}
		
	}

	if (MainAnim && AttackMontage)
	{
		MainAnim->Montage_Play(AttackMontage);
		MainAnim->Montage_JumpToSection(FName("Interact"));
	}
}

void AMainCharacter::SetRoomGate(ARoomGate* RoomGate)
{
	GateRoom = RoomGate;
}

void AMainCharacter::RestartLevel()
{
	if (MainPlayerController)
	{
		MainPlayerController->RestartLevel();
	}
}

AMainPlayerController* AMainCharacter::GetMainPlayerController()
{
	return MainPlayerController;
}





