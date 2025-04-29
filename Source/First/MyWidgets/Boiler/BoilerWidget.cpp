// Fill out your copyright notice in the Description page of Project Settings.

#include "BoilerWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/UniformGridPanel.h"
#include "First/MyCharacter/EpicCharacter.h"
#include "First/MyWidgets/My_GridWidget.h"

void UBoilerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 绑定点击事件
	CloseButton->OnClicked.AddDynamic(this, &UBoilerWidget::OnButtonClicked);
	FireButton->OnClicked.AddDynamic(this, &UBoilerWidget::OnFireButtonClicked);

	// 加载材质
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/_project/Materials/M_ColdDown"));
	DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	DynamicMaterial->SetScalarParameterValue("color", 1.0f);
	DynamicMaterial->SetScalarParameterValue("percentage", 0.0f);
	FireImage->SetBrushFromMaterial(DynamicMaterial);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle2);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &UBoilerWidget::UpdateBarPercentage, 0.1f,true);
}

void UBoilerWidget::Init()
{
	//格子类
	UClass* GridWidgetClass = LoadClass<UMy_GridWidget>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/BP_GridWidget.BP_GridWidget_C"));
	GridClass = GridWidgetClass;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();	// 获取玩家控制器
	ACharacter* ControlledCharacter = PlayerController ? Cast<ACharacter>(PlayerController->GetPawn()) : nullptr;
	CurrentCharacter = Cast<AEpicCharacter>(ControlledCharacter);
	
	for (int i = 0; i < 8; ++i)	
	{
		UMy_GridWidget* GridWidget = CreateWidget<UMy_GridWidget>(GetWorld(), GridClass);	//创建新的格子
		GridContainer->AddChildToUniformGrid(GridWidget, i / 4, i % 4);	// 每行最多4个格子
		GridWidget->SetRelativeBag(CurrentCharacter->BagSystem, i + 100);
		GridWidgetsArray.Add(GridWidget);
	}
	if (ProgressBar)
	{
		ProgressBar->SetPercent(BarPercentage);  // 初始进度为50%
	}
	
}

void UBoilerWidget::Update(FActorResourceData* ActorResourceData)
{
	TArray<EMySourceType> ResourceTypeArray;
	TArray<int32> ResourceNumArray;
	for (int32 i = 0; i < ActorResourceData->TypeArray.Num(); ++i)
	{
		ResourceTypeArray.Add(ActorResourceData->TypeArray[i]);
		ResourceNumArray.Add(ActorResourceData->NumArray[i]);
	}
	for (int i = 0; i < 8; ++i)	
	{
		GridWidgetsArray[i]->SetHighlightImage(false);
		if (i < ResourceTypeArray.Num())	//防止越界
		{
			GridWidgetsArray[i]->SetSourceTypeName(ResourceTypeArray[i]);	//初始化格子中的资源与数量，做 ui显示
			GridWidgetsArray[i]->SetGridItemNum(ResourceNumArray[i]);
			GridWidgetsArray[i]->RefreshGrid();
		}
	}
	
}


void UBoilerWidget::OnFireButtonClicked()
{
	if (isFireButtonClicked)
	{
		return;
	}
	if (CurrentCharacter->BagSystem->GetSpecialResourcesNum(EMySourceType::Plank) == 0) //没有点火资源，这里现在只处理了木板
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Fire Material"));
		return;
	}
	isFireButtonClicked = true;
	CurrentPercentage = 1.0f;

	// 启动插值计时器
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UBoilerWidget::UpdateMaterialPercentage, 0.05f,true);
}

void UBoilerWidget::UpdateMaterialPercentage()
{
	if (CurrentPercentage > 0.05f)
	{
		// 计算插值（5秒内从 1 → 0）
		CurrentPercentage = FMath::FInterpConstantTo(CurrentPercentage, 0.0f, 0.05f, 1.0f / 5.0f);
		DynamicMaterial->SetScalarParameterValue("percentage", CurrentPercentage);
		FireImage->SetBrushFromMaterial(DynamicMaterial);
	}
	else
	{
		//  停止计时器
		GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]() { GetWorld()->GetTimerManager().ClearTimer(TimerHandle); },5.0f,false);
		DynamicMaterial->SetScalarParameterValue("percentage", 0.0f);
		FireImage->SetBrushFromMaterial(DynamicMaterial);
		isFireButtonClicked = false;

		//更新进度条
		int32 PlankNum = CurrentCharacter->BagSystem->GetSpecialResourcesNum(EMySourceType::Plank);
		if (PlankNum != 0)
		{
			Update(CurrentCharacter->BagSystem->ReduceResourceByFire(EMySourceType::Plank));
			BarPercentage += 0.1 * PlankNum;
		}
		if (!isBarUpdating)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle2);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle2, this, &UBoilerWidget::UpdateBarPercentage, 0.1f,true);
		}
	}
}

void UBoilerWidget::UpdateBarPercentage()
{
	if (BarPercentage <= 0.0f)
	{
		//  停止计时器
		GetWorld()->GetTimerManager().SetTimer(TimerHandle2,[this]() { GetWorld()->GetTimerManager().ClearTimer(TimerHandle2); },1.0f,false);
		isBarUpdating = false;
	}
	else
	{
		BarPercentage -= 0.001;
		ProgressBar->SetPercent(BarPercentage);
	}

	
}

void UBoilerWidget::OnButtonClicked()
{

}

float UBoilerWidget::GetBarPercentage()
{
	return BarPercentage;
}
