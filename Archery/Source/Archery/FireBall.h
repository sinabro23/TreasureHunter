// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireBall.generated.h"

UCLASS()
class ARCHERY_API AFireBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool IsFired = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireBall", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollsion = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireBall", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FireballParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireBall", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* NoImpactParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireBall", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle = nullptr;

	FVector FireballDirection = FVector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireBall", meta = (AllowPrivateAccess = "true"))
	float Speed = 1000.f;

	float RemainTime = 0.f;

public:
	void StartFireBall(const FVector& Direction);
	void SendFireball(float DeltaTime);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
