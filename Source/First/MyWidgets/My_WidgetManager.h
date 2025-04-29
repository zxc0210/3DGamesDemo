// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "My_WidgetManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRST_API UMy_WidgetManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMy_WidgetManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
protected:
	// 控件蓝图类
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CrosshairWidget")
	TSubclassOf<UUserWidget> CrosshairWidgetClass;
    // 控件本身
	UPROPERTY()
	UUserWidget* CrosshairWidget;
	
	// 控件蓝图类
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PointWidget")
	TSubclassOf<UUserWidget> PointWidgetClass;
	// 控件本身
	UPROPERTY()
	UUserWidget*  PointWidget;
	
public:
	// 函数声明
	UFUNCTION()
	void Widget_Init();

	UFUNCTION(Category = "CrosshairWidget")
	void Crosshair_Show();

	UFUNCTION( Category = "CrosshairWidget")
	void Crosshair_Hide();
	
	UFUNCTION(Category = "PointWidget")
	void Point_Show();

	UFUNCTION( Category = "PointWidget")
	void Point_Hide();
};
