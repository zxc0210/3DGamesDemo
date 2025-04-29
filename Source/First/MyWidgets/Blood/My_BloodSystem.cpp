// Fill out your copyright notice in the Description page of Project Settings.


#include "My_BloodSystem.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceConstant.h"

bool UMy_BloodSystem::BloodUIIsUsing = false; // 静态变量初始化

// Sets default values for this component's properties
UMy_BloodSystem::UMy_BloodSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CharacterCurrentBlood = CharacterMaxBlood;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> BloodWidgetClass(TEXT("/Game/_project/Sources/Widgets/BloodUI/BP_BloodWidget"));
	if (BloodWidgetClass.Succeeded())
	{
		BloodClass = BloodWidgetClass.Class;
	}
	
	// 加载材质资源
	BloodUIMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/_project/Sources/Widgets/BloodUI/M_Circle"));

	// 初始化过渡曲线
	BloodCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/_project/Others/TimelineCurve"));
}


// Called when the game starts
void UMy_BloodSystem::BeginPlay()
{
	Super::BeginPlay();
	
	BloodWidget = CreateWidget<UUserWidget>(GetWorld(), BloodClass);
	BloodImage = Cast<UImage>(BloodWidget->WidgetTree->FindWidget("BloodImage"));	//BP_BloodWidget的父类未设置，所以这里还要进行绑定
	
	// 创建动态材质实例
	BloodUIMaterialInstance = UMaterialInstanceDynamic::Create(BloodUIMaterial, this);

	// 在BeginPlay()中初始化timeline组件
	if (BloodCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("BloodTimelineUpdate"));
		BloodTimeline.AddInterpFloat(BloodCurve, TimelineCallback);
		
		BloodTimeline.SetTimelineLength(1.0f); // 设置过渡时间
		BloodTimeline.SetPlayRate(4.0f);
	}
	else { UE_LOG(LogTemp, Error, TEXT("AimingCurve is not set!")); }
}


// Called every frame
void UMy_BloodSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 这行必须调用，否则无法触发下面的TimelineCallback函数，动画也就无从实现了
	BloodTimeline.TickTimeline(DeltaTime);
	if (bIsModifyBlood)
	{
		BloodTimelineUpdate(BloodTimeline.GetPlaybackPosition());
	}

}

void UMy_BloodSystem::BloodUI_Show()
{
	BloodWidget->AddToViewport();
}

void UMy_BloodSystem::BloodUI_Hide()
{
}

int32 UMy_BloodSystem::ModifyBlood(int32 ModifyNum)
{
	bIsModifyBlood = true;	//是否允许血量ui修改
	
	// 设置起始和目标血量
	StartBlood = CharacterCurrentBlood;
	CharacterCurrentBlood += ModifyNum;
	if (CharacterCurrentBlood > CharacterMaxBlood)
	{
		CharacterCurrentBlood = CharacterMaxBlood;
	}
	else if (CharacterCurrentBlood <= 0 )
	{
		CharacterCurrentBlood = 0;
	}
	
	if (BloodTimeline.IsPlaying())
	{
		BloodTimeline.Stop(); // 如果时间轴正在播放，先停止
	}
	BloodTimeline.PlayFromStart();
	
	if (CharacterCurrentBlood == 0 && IsOwnerStanding)	//如果站立时血量为 0了，那就倒地
	{
		CharacterCurrentBlood = CharacterMaxBlood;
		IsOwnerStanding = false;
		BloodUIMaterialInstance->SetScalarParameterValue(TEXT("CirclePercent"), 1.0f);
		BloodImage->SetBrushFromMaterial(BloodUIMaterialInstance);	// 将材质应用到 BloodImage
	}
	
	return CharacterCurrentBlood;
}



void UMy_BloodSystem::BloodTimelineUpdate(float Value)
{
	CurrentInterpolatedBlood = FMath::Lerp(static_cast<float>(StartBlood), static_cast<float>(CharacterCurrentBlood), Value);	// 插值血量
	float Percentage = static_cast<float>(CurrentInterpolatedBlood) / CharacterMaxBlood;
	
	if (!BloodUIIsUsing)
	{
		BloodUIIsUsing = true;
		if (BloodImage && BloodUIMaterialInstance)
		{
			BloodUIMaterialInstance->SetScalarParameterValue(TEXT("CirclePercent"), Percentage);	 // 修改动态材质实例的血量 UI百分比
			BloodImage->SetBrushFromMaterial(BloodUIMaterialInstance);	// 将材质应用到 BloodImage
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Blood Image"));
		}

		BloodUIIsUsing = false;		//避免多个actor同时修改ui
	}
	
	if (CurrentInterpolatedBlood - CharacterCurrentBlood == 0)	//达到插值目标，结束插值
	{
		bIsModifyBlood = false;
	}
	
}

bool UMy_BloodSystem::GetOwnerState()
{
	return IsOwnerStanding;
}

int32 UMy_BloodSystem::GetCurrentBlood()
{
	return CharacterCurrentBlood;
}
