// Fill out your copyright notice in the Description page of Project Settings.


#include "My_WidgetManager.h"
//目前只包含显示准星与中心点的功能

// Sets default values for this component's properties
UMy_WidgetManager::UMy_WidgetManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairWidgetFinder(TEXT("/Game/_project/Sources/Widgets/Crosshair/BP_CrosshairWidget"));
	if (CrosshairWidgetFinder.Succeeded())
	{
		CrosshairWidgetClass = CrosshairWidgetFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> PointWidgetFinder(TEXT("/Game/_project/Sources/Widgets/Crosshair/BP_PointWidget"));
	if (PointWidgetFinder.Succeeded())
	{
		PointWidgetClass = PointWidgetFinder.Class;
	}
}


// Called when the game starts
void UMy_WidgetManager::BeginPlay()
{
	Super::BeginPlay();

	Widget_Init();
	
}


// Called every frame
void UMy_WidgetManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UMy_WidgetManager::Widget_Init()
{
	// 实例化
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairWidgetClass);
	}
	if (PointWidgetClass)
	{
		PointWidget = CreateWidget<UUserWidget>(GetWorld(), PointWidgetClass);
	}
}


void UMy_WidgetManager::Crosshair_Show()
{
	// 添加到视口
	CrosshairWidget->AddToViewport();	
}

void UMy_WidgetManager::Crosshair_Hide()
{
	// 从视口移除
	CrosshairWidget->RemoveFromViewport();
}

void UMy_WidgetManager::Point_Show()
{
	// 添加到视口
	PointWidget->AddToViewport();	
}

void UMy_WidgetManager::Point_Hide()
{
	// 从视口移除
	PointWidget->RemoveFromViewport();
}