// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "My_BloodSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRST_API UMy_BloodSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMy_BloodSystem();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	void BloodUI_Show();
	void BloodUI_Hide();
	int32 ModifyBlood(int32 ModifyNum);
	
	UFUNCTION(BlueprintCallable)
	bool GetOwnerState();

	int32 GetCurrentBlood();

	static bool BloodUIIsUsing;
	
private:
	// Blood控件UI
	UPROPERTY(EditAnywhere, Category = "BloodSystem")
	TSubclassOf<UUserWidget> BloodClass;
	UPROPERTY()
	UUserWidget* BloodWidget;
	UPROPERTY(meta = (BindWidget))
	class UImage* BloodImage;
	
	bool IsOwnerStanding = true;
	
	UMaterialInterface* BloodUIMaterial;	//血量UI材质
	UMaterialInstanceDynamic* BloodUIMaterialInstance;	// 动态材质实例
	
	int32 CharacterMaxBlood = 100;
	int32 CharacterCurrentBlood;
	int32 StartBlood;             // 起始血量
	float CurrentInterpolatedBlood; // 当前插值血量
	
	bool bIsModifyBlood;
	FTimeline BloodTimeline;
	UPROPERTY(EditAnywhere)
	UCurveFloat* BloodCurve;	//timeline的曲线
	UFUNCTION()		//这里一定要使用UFUNCTION,因为后面用了回调函数
	void BloodTimelineUpdate(float Value);
	

};
