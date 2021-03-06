// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSendFireBallDelegate)
DECLARE_MULTICAST_DELEGATE(FOnSendFireBallEndDelegate)
DECLARE_MULTICAST_DELEGATE(FOnCastingEndDelegate)
DECLARE_MULTICAST_DELEGATE(FOnBurdenEndDelegate)
DECLARE_MULTICAST_DELEGATE(FOnBurdenFireDelegate)
/**
 * 
 */
UCLASS()
class ARCHERY_API UMainAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// 블루프린트 이벤트그래프에서 속성들 업데이트해서 받을 것
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	class AMainCharacter* MainCharacter = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	float Speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", Meta = (AllowPrivateAccess = "true"))
	float LastMovementOffsetYaw = 0.f; // 멈출때의 오프셋을 위함

public:
	UFUNCTION()
	void AnimNotify_SendFireBall();

	UFUNCTION()
	void AnimNotify_FireballAttackEnd();

	UFUNCTION()
	void AnimNotify_CastingEnd();

	UFUNCTION()
	void AnimNotify_BurdenEnd();

	UFUNCTION()
	void AnimNotify_BurdenFire();

	void CalculateMovementOffset();

public:
	FOnSendFireBallDelegate OnSendFireBall;
	FOnSendFireBallEndDelegate OnFireBallEnd;
	FOnCastingEndDelegate OnCastingEnd;
	FOnBurdenEndDelegate OnBurdenEnd;
	FOnBurdenFireDelegate OnBurdenFire;
};
