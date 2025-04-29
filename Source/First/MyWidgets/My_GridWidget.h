// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "First/MySoueces/MySourcesType.h"
#include "My_GridWidget.generated.h"

//使用背包类的前向声明，避免循环依赖
class UMy_BagSystem;

/**
 * 
 */

UCLASS()
class FIRST_API UMy_GridWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	//拖拽函数
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

protected:
	// 所属背包的指针
	UMy_BagSystem* OwingBag;
	UMy_BagSystem* RelativeBag;
	
	// 格子控件UI
	UPROPERTY(EditAnywhere, Category = "BagSystem")
	TSubclassOf<UUserWidget> DragUIClass;
	
	//格子控件中的元素
	UPROPERTY(meta = (BindWidget))
	UButton* GridButton;
	UPROPERTY(meta = (BindWidget))
	UImage* GridImage;
	UPROPERTY(meta = (BindWidget))
	UImage* GridItemImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GridItemNum; // 文本控件

	int32 GridId = -1;
	int32 ItemNum = 0;
	EMySourceType SourceTypeName = EMySourceType::NoneSource;	//默认为空资源


public:
	static UTexture2D* DragGridImage; // 保存拖拽时当前格子的item图片
	static int32 DragGridItemNum;
	static int32 DragGridId;	//保存当前拖拽的格子id
	static EMySourceType DragGridType;	//保存当前拖拽的格子id
	
	void SetRelativeBag(UMy_BagSystem* Bag, int32 Index);
	void SetOwingBag(UMy_BagSystem* Bag, int32 Index);	//设置所属的背包索引以及格子id
	
	void HideImage();
	void SetHighlightImage(bool bHighlight);
	void SetPreselectImage();
	void SetImage(EMySourceType st);
	void SetSourceTypeName(EMySourceType st);
	EMySourceType GetSourceTypeName();
	void ModifyGridItemNum(int32 num) ;
	int32 GetGridItemNum();
	void SetGridItemNum(int32 num);
	
	UFUNCTION()
	void OnButtonClicked();	// AddDynamic绑定的函数必须要用UFUNCTION() 标记
	UFUNCTION()
	void RefreshGrid(); //刷新格子

};
