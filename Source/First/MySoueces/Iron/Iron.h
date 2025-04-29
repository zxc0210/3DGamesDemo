// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "First/MySoueces/Fire/MyAssets.h"
#include "Iron.generated.h"

class UProjectileMovementComponent;
/**
 * 
 */
UCLASS()
class FIRST_API AIron : public AMyAssets
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AIron();

	static int32 MaxStackNum;  //每个格子该资源的最多堆叠数量
	static int32 GetMaxStackNum();

	void Throw(FVector StartLocation, FVector EndLocation, FQuat ActorRotation, bool HitOrNot) override;
	EMySourceType GetSourceTypeName() override;
	void SetHighLight() override;
	void DisableHighLight() override;
	
private:
	UStaticMeshComponent* StaticMC_Iron;

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
