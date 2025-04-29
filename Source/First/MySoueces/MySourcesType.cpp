// Fill out your copyright notice in the Description page of Project Settings.


#include "MySourcesType.h"

UMySourcesType::UMySourcesType()
{
	// 初始化图片资源
	InitializeSources();
}

UMySourcesType* UMySourcesType::GetInstance()
{
	static UMySourcesType* Instance = NewObject<UMySourcesType>();
	return Instance;
}

void UMySourcesType::InitializeSources()
{
	// 加载图片资源
	NullGridImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/GridImage.GridImage"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ImageAsset(TEXT("/Game/_project/Sources/Widgets/BagUI/Image/high-shot"));
	BowImage = ImageAsset.Object;
	ArrowImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/arrow-cluster"));
	AxeImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/tomahawk"));
	FireImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/primitive-torch"));
	SabreImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/crossed-sabres"));
	PlankImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/wood-beam"));
	IronImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/_project/Sources/Widgets/BagUI/Image/gold-bar"));
}

UTexture2D* UMySourcesType::GetImageByType(EMySourceType E_SourceType)
{
	switch (E_SourceType)
	{
	case EMySourceType::NoneSource:
		SourceImage = NullGridImage;
		break;
	case EMySourceType::Axe:
		SourceImage = AxeImage;
		break;
	case EMySourceType::Bow:
		SourceImage = BowImage;
		break;
	case EMySourceType::Arrow:
		SourceImage = ArrowImage;
		break;
	case EMySourceType::Fire:
		SourceImage = FireImage;
		break;
	case EMySourceType::Sabre:
		SourceImage = SabreImage;
		break;
	case EMySourceType::Plank:
		SourceImage = PlankImage;
		break;
	case EMySourceType::Iron:
		SourceImage = IronImage;
		break;
	default:
		SourceImage = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("error!image has been reset"));
		break;
	}
	return SourceImage;
}


