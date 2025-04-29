// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/ProjectileMovementComponent.h"
#include "CoreMinimal.h"
#include "First/MySoueces/Fire/MyAssets.h"
#include "GameFramework/Actor.h"
#include "My_Arrow.generated.h"

UCLASS()
class FIRST_API AMy_Arrow : public AMyAssets
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMy_Arrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	static int32 MaxStackNum;  //每个格子该资源的最多堆叠数量
	static int32 GetMaxStackNum();

	void Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot) override;
	EMySourceType GetSourceTypeName() override;
	void SetHighLight() override;
	void DisableHighLight() override;
	
	void ArrowProjectile();
	void ArrowAttackCheck();
	
private:
	UStaticMeshComponent* StaticMC_Arrow;
	bool isAttacking;
	int32 ArrowAttackNum = 16;
	
	//箭的方向
	FVector ProjectileDirection;
	void CalculateDirection();
	
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
	
};
