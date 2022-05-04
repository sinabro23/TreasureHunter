// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class ARCHERY_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	///** Particles to spawn when hit by bullets */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//class UParticleSystem* ImpactParticles;

	///** Sound to play when hit by bullets */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//class USoundCue* ImpactSound;

	///** Current health of the enemy */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float Health = 100.f;

	///** Maximum health of the enemy */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float MaxHealth = 100.f;

	///** Montage containing Hit and Death animations */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//UAnimMontage* HitMontage;

	//FTimerHandle HitReactTimer;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float HitReactTimeMin = 0.5f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float HitReactTimeMax = 0.75f;

	//bool bCanHitReact = true;


	///** Behavior tree for the AI Character */
	//UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	//class UBehaviorTree* BehaviorTree;

	///** Point for the enemy to move to */
	//UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	//FVector PatrolPoint;

	///** Point for the enemy to move to */
	//UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	//FVector PatrolPoint2;

	//class AEnemyController* EnemyController;

	///** Overlap sphere for when the enemy becomes hostile */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//class USphereComponent* AgroSphere;

	///** True when playing the get hit animation */
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//bool bStunned = false; // BulletHit_Implementation(FHitResult HitResult); // �ִԳ�Ƽ���̷� ������ false�� ����

	//	/** Chance of being stunned. 0: no stun chance, 1: 100% stun chance */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float StunChance = 0.5f;

	///** True when in attack range; time to attack! */
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//bool bInAttackRange;

	///** Sphere for attack range */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//USphereComponent* CombatRangeSphere;

	///** Montage containing different attacks */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//UAnimMontage* AttackMontage;

	///** The four attack montage section names */
	//FName AttackLFast = TEXT("AttackLFast");
	//FName AttackRFast = TEXT("AttackRFast");
	//FName AttackL = TEXT("AttackL");
	//FName AttackR = TEXT("AttackR");

	///** Collision volume for the left weapon */
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//class UBoxComponent* LeftWeaponCollision;

	///** Collision volume for the right weapon */
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//UBoxComponent* RightWeaponCollision;

	///** Base damage for enemy */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float BaseDamage = 20.f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//FName LeftWeaponSocket = TEXT("FX_Trail_L_01");

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//FName RightWeaponSocket = TEXT("FX_Trail_R_01");

public:
};