// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BoilerWidget.generated.h"

class AEpicCharacter;
class UProgressBar;
struct FActorResourceData;
class UMy_GridWidget;
class UUniformGridPanel;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class FIRST_API UBoilerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;

	void Init();
	void Update(FActorResourceData* ActorResourceData);

	float GetBarPercentage();
protected:
	//控件中的元素
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
	UImage* FireImage;
	UPROPERTY(meta = (BindWidget))
	UButton* FireButton;
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridContainer;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	// 格子
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_GridWidget> GridClass;
	
	TArray<UMy_GridWidget*> GridWidgetsArray;  //在Array中保存所有的格子
	
	AEpicCharacter* CurrentCharacter;
	
	float CurrentPercentage;
	FTimerHandle TimerHandle; // 计时器句柄
	FTimerHandle TimerHandle2;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;  // 动态材质实例
	UFUNCTION()
	void UpdateMaterialPercentage();
	UFUNCTION()
	void UpdateBarPercentage();
	bool isBarUpdating = true;
	float BarPercentage = 0.5f;
	
	UFUNCTION()
	void OnFireButtonClicked();
	bool isFireButtonClicked = false;
	
	UFUNCTION()
	void OnButtonClicked();
};

