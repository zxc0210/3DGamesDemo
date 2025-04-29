// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPCBaseCharacter.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Wolf.generated.h"

class AEpicCharacter;

UCLASS()
class FIRST_API AWolf : public ANPCBaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWolf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	virtual void ReduceNPCBlood(int32 ReduceNum) override;
	virtual TArray<EMySourceType> GetDropResourceType() override;
	virtual TArray<int32> GetDropResourceNum() override;
	
	static TArray<EMySourceType> GenerateResourceType;		//死亡后掉落的资源类型

	UFUNCTION(BlueprintCallable)
	float GetWolfSpeed() const;
	
	UFUNCTION(BlueprintCallable, Category = "Wolf")
	void AttackOccurredFromBP();
	
	UFUNCTION(BlueprintCallable, Category = "Wolf")
	bool GetAllowAttackState();
	
private:
	USkeletalMeshComponent* SM_Wolf;

	UCapsuleComponent* CapsuleComponent;

	TArray<int32> GenerateResourceNum; //生成的资源数量
	
	bool IsDied = false;
	int32 WolfDamageAmount = -14;
	
	class AMyAIController* AIController;
	
	AEpicCharacter* EpicCharacter;
	
	bool IsAllowAttack = false;
	bool bPerceivingCharacter = false;
	AActor* PerceivedCharacter = nullptr;
	
	TArray<AActor*> PerceivedActors;	// 存储感知到的所有Actor
	
	UPROPERTY(VisibleAnywhere, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;		//AI感知组件
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);	 // 当感知到Actor时调用
	
	UFUNCTION(BlueprintCallable)
	bool GetWolfAliveState();
};
