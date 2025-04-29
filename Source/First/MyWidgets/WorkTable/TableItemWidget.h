// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TableItemWidget.generated.h"

class UDataTable;
struct FRecipe;
class UWorkTableWidget;
class UMy_BagSystem;
enum class EMySourceType : uint8;
class UImage;
/**
 * 
 */
UCLASS()
class FIRST_API UTableItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UTableItemWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeConstruct() override;
	//拖拽函数
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	static TArray<EMySourceType> ItemType;		//item类型
	static TArray<float> ItemTime;		//item类型
	static int32 GetCount();		
	static bool isWorking;
	static FRecipe* ReduceRecipe;
	static TMap<EMySourceType, int32> ReduceRecipe2;
	
	void InitImageByType();
	void SetIndex(int32 index,UMy_BagSystem* Bag,UWorkTableWidget* Table);
	
protected:
	//控件中的元素
	UPROPERTY(meta = (BindWidget))
	UImage* GridItemImage;
	
	UMy_BagSystem* RelativeBag;
	UWorkTableWidget* TableWidget;
	
	int32 ItemIndex = -1;	//当前Item是第几个
	float CurrentPercentage;
	FTimerHandle TimerHandle; // 计时器句柄
	
	UMaterialInterface* BaseMaterial;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;  // 动态材质实例
	UFUNCTION()
	void UpdateMaterialPercentage();
	
	UDataTable* RecipeTable;
	bool IsResourceEnoughToWork();

private:
	bool bHasExecuted = false;
};
