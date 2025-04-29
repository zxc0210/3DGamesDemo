// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorkTableWidget.generated.h"

struct FActorResourceData;
class UTableItemWidget;
class UMy_GridWidget;
class UUniformGridPanel;
class UButton;
/**
 * 
 */
UCLASS()
class FIRST_API UWorkTableWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	static UWorkTableWidget* GetInstance();	// 全局单例获取函数
	
	void Init();
	void Update(FActorResourceData* ActorResourceData);
	
protected:
	//控件中的元素
	UPROPERTY(meta = (BindWidget))
	UButton* CloseButton;
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* ItemContainer;
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* GridContainer;

	// Item
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTableItemWidget> ItemClass;
	// 格子
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_GridWidget> GridClass;
	TArray<UMy_GridWidget*> GridWidgetsArray;  //在Array中保存所有的格子

private:
	bool bHasExecuted = false;
};
