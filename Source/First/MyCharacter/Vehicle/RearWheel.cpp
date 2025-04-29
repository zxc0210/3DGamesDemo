// Fill out your copyright notice in the Description page of Project Settings.


#include "RearWheel.h"

URearWheel::URearWheel()
{
	AxleType = EAxleType::Rear;
	
	// 车轮
	WheelRadius = 35.0f;       // 车轮半径（单位：厘米）
	WheelWidth = 20.0f;        // 车轮宽度（厘米）
    
	// 悬挂
	SuspensionMaxRaise = 1.0f; // 悬挂最大抬高量（厘米）
	SuspensionMaxDrop = 1.0f;  // 悬挂最大压缩量
	SuspensionDampingRatio = 0.5f;    // 悬挂阻尼比
    
	// 摩擦
	FrictionForceMultiplier = 3.0f;   // 轮胎摩擦力系数
	
	//刹车
	MaxBrakeTorque = 500.0f;
	bAffectedByHandbrake = true;
}


