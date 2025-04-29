// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "First/MySoueces/Fire/MyAssets.h"
#include "GameFramework/Actor.h"
#include "Sabre.generated.h"

class UProjectileMovementComponent;
class ANPCBaseCharacter;

UCLASS()
class FIRST_API ASabre : public AMyAssets
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASabre();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot) override;
	EMySourceType GetSourceTypeName() override;
	void SetHighLight() override;
	void DisableHighLight() override;
	
private:
	void Construct();
	
	UPROPERTY(EditAnywhere, Category = "Sabre")
	USkeletalMeshComponent* SM_Sabre;

	int32 SabreAttackNum = 16;
	TSet<ANPCBaseCharacter*> AlreadyAttackedNpcSet;	//存放一次攻击影响到的actor集合，防止对同一个目标二次伤害
	bool isAttacking;	//攻击动画正在播放

	UPROPERTY()
	USoundBase* ExplosionSound;		//击中目标的音效
	bool AudioAlreadyPlayed;	//本次击中目标的音效是否已经播放过


	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	FQuat ThrowActorRotation;
	FVector ThrowActorLocation;
	bool HitTraceAEnd = false;
	bool isOverlap = false;
	
	UFUNCTION()
	void BeginOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
	
public:
	
	void SabreAttackStart();
	void SabreAttackEnd();
	void SabreAttackCheck();
};
