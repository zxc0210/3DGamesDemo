// Fill out your copyright notice in the Description page of Project Settings.


#include "TableItemWidget.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "WorkTableWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"
#include "First/MyCharacter/EpicCharacter.h"
#include "First/MySoueces/MySourcesType.h"
#include "First/MyWidgets/My_BagSystem.h"

TArray<EMySourceType> UTableItemWidget::ItemType = { 
	EMySourceType::Sabre,
	EMySourceType::Fire,
	EMySourceType::Bow,
	EMySourceType::Arrow
};
TArray<float> UTableItemWidget::ItemTime = { 
	5.0f,
	2.0f,
	5.0f,
	1.0f
};
bool UTableItemWidget::isWorking = false; // 静态变量初始化
FRecipe* UTableItemWidget::ReduceRecipe = nullptr; // 静态变量初始化
TMap<EMySourceType, int32> UTableItemWidget::ReduceRecipe2 = {}; // 静态变量初始化
int32 UTableItemWidget::GetCount()
{
	return ItemType.Num();
}

UTableItemWidget::UTableItemWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BaseMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/_project/Materials/M_ColdDown"));
	RecipeTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_project/Sources/Widgets/WorkTable/WorkRecipe"));// 加载 DataTable
}

void UTableItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (bHasExecuted)
	{
		return;
	}
	//每次启动之后都会调用这里的代码
	InitImageByType();
	
	bHasExecuted = true;
}

void UTableItemWidget::SetIndex(int32 index, UMy_BagSystem* Bag,UWorkTableWidget* Table)
{
	ItemIndex = index;
	RelativeBag = Bag;
	TableWidget = Table;
}



FReply UTableItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	
	if (isWorking)
	{
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	if (!IsResourceEnoughToWork())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Enough Material"));
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}
	isWorking = true;
	
	CurrentPercentage = 1.0f;
	DynamicMaterial->SetScalarParameterValue("color", 1.0f);
	DynamicMaterial->SetScalarParameterValue("percentage", 1.0f);
	DynamicMaterial->SetVectorParameterValue("PercentColor", FLinearColor(0.7f, 0.3f, 0.1f, 1.0f));
	GridItemImage->SetBrushFromMaterial(DynamicMaterial);
	
	// 启动插值计时器
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTableItemWidget::UpdateMaterialPercentage, 0.05f,true);
	
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UTableItemWidget::IsResourceEnoughToWork()
{
	TArray<FRecipe*> Recipes;
	RecipeTable->GetAllRows<FRecipe>(TEXT("Content"), Recipes);	// 获取数据表中所有行数据
	for (FRecipe* Recipe : Recipes)	// 查找
	{
		if (Recipe->Target == ItemType[ItemIndex])
		{
			for (const auto& MaterialPair : Recipe->Material)
			{
				bool result = RelativeBag->CheckIfEnoughResource(MaterialPair.Key,MaterialPair.Value);//检测当前Actor是否有足够的资源
				if (!result)
				{
					return false;
				}
			}
			ReduceRecipe = Recipe; //更新要减少的材料
			ReduceRecipe2 = Recipe->Material;
			break;
		}
	}
	return true;
}

void UTableItemWidget::UpdateMaterialPercentage()
{
	if (CurrentPercentage > 0.05f)
	{
		// 计算插值
		CurrentPercentage = FMath::FInterpConstantTo(CurrentPercentage, 0.0f, 0.05f, 1.0f / ItemTime[ItemIndex]);
		DynamicMaterial->SetScalarParameterValue("percentage", CurrentPercentage);
		GridItemImage->SetBrushFromMaterial(DynamicMaterial);
	}
	else
	{
		// 停止计时器，结束此次点击事件,最后重置颜色
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]() { GetWorld()->GetTimerManager().ClearTimer(TimerHandle); },ItemTime[ItemIndex],false);
		DynamicMaterial->SetScalarParameterValue("percentage", 0.0f);
		DynamicMaterial->SetVectorParameterValue("PercentColor", FLinearColor(0.0f, 0.0f, 0.0f, 1.0f) );
		GridItemImage->SetBrushFromMaterial(DynamicMaterial);
		
		//更新材料数据,并更新UI显示
		 for (const auto& MaterialPair : ReduceRecipe2)
		 {
		 	RelativeBag->ReduceResourceByWork(MaterialPair.Key,MaterialPair.Value); //减少材料
		 }
		TableWidget->Update(RelativeBag->AddResourceByWork(ReduceRecipe->Target,ReduceRecipe->Quantity)); //添加合成目标资源
		isWorking = false;
	}
}



void UTableItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (isWorking)
	{
		return;
	}
	DynamicMaterial->SetScalarParameterValue("color", 0.4f);
	GridItemImage->SetBrushFromMaterial(DynamicMaterial);
}

void UTableItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (isWorking)
	{
		return;
	}
	DynamicMaterial->SetScalarParameterValue("color", 1.0f);
	GridItemImage->SetBrushFromMaterial(DynamicMaterial);
}



//根据传入的数量初始化item的图片
void UTableItemWidget::InitImageByType()
{
	if (ItemIndex >= ItemType.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Enough workTable Item"));
		return;
	}

	DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);	// 动态材质实例
	UMySourcesType* GridSourceType = NewObject<UMySourcesType>(this, UMySourcesType::StaticClass());
	UTexture2D* itemImage = GridSourceType->GetImageByType(ItemType[ItemIndex]);	// 加载纹理
	
	DynamicMaterial->SetScalarParameterValue("color", 1.0f);
	DynamicMaterial->SetScalarParameterValue("percentage", 0.0f);
	DynamicMaterial->SetVectorParameterValue("PercentColor", FLinearColor(0.0f, 0.0f, 0.0f, 1.0f) );
	
	if(!itemImage) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NO image"));
		return;
	}
	DynamicMaterial->SetTextureParameterValue("Image", itemImage);   //这一行的问题，应该是没有图片导致的
	GridItemImage->SetBrushFromMaterial(DynamicMaterial);
	
}


