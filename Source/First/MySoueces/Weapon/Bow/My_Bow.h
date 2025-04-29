// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "My_Arrow.h"
#include "First/MySoueces/Fire/MyAssets.h"
#include "GameFramework/Actor.h"
#include "My_Bow.generated.h"

UENUM(BlueprintType)
enum class EBowState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Fire UMETA(DisplayName = "Fire"),
	Aiming UMETA(DisplayName = "Aiming")
};

UCLASS()
class FIRST_API AMy_Bow : public AMyAssets
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMy_Bow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
private:
	USkeletalMeshComponent* SM_Bow;
	
	//动画蓝图
	UPROPERTY(EditAnywhere, Category = "BowAnimation", meta = (AllowPrivateAccess = "true"))
	UAnimInstance* BowAnimInstance;
	// //动画蒙太奇
	// UPROPERTY(EditAnywhere, Category = "BowAnimation", meta = (AllowPrivateAccess = "true"))
	// UAnimMontage* Montage_DrawOnly;

	AMy_Arrow *MyArrow;

public:

	// 弓箭状态枚举
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BowState")
	EBowState E_BowState;
	void SetBowState(EBowState state);
	
	void Bow_Idle();
	UFUNCTION(BlueprintCallable, Category = "SetBowState")
	void Bow_AimingOnly();
	void Bow_Fire();
	EMySourceType GetSourceTypeName() override;
};
