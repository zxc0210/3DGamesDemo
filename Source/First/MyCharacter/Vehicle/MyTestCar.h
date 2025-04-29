// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "MyTestCar.generated.h"

class ABaseLadder;
class ABoiler;
class AEpicCharacter;
class UBoxComponent;
class UChaosWheeledVehicleMovementComponent;
class UMyWheelMesh;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class FIRST_API AMyTestCar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	

public:
	AMyTestCar();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mc2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ShipWheel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* ShipWheelTable;
	//相机和摇臂
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* cameraArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABoiler* AttachedBoiler;  // 锅炉实例
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	ABaseLadder* LeftLadder;  // 左侧梯子

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	ABaseLadder* RightLadder; // 右侧梯子
	
	void AttachActorsToShip();

	AEpicCharacter* DrivingChara;
	bool bIsDriving = false;
	void DriveBoat(AEpicCharacter* chara);

	float CurrentRotationAngle = 0.0f;
	void WheelRolling(float DeltaTime);

	bool isStopTraction = false;
	float PowerValue = 0.0f;

	int32 WheelNum = 4;
	float steering = 0.0f;
	UPROPERTY(EditAnywhere)
	UCurveFloat* EngineCurve;

	//插件中的VehicleMovementComponent = CreateDefaultSubobject<UChaosVehicleMovementComponent, UChaosWheeledVehicleMovementComponent>(TEXT("VehicleMovementComp"));
	//实际创建的是派生类UChaosWheeledVehicleMovementComponent的对象，返回的是基类的指针
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "VehicleMovement")
	UChaosWheeledVehicleMovementComponent* WheelMovement;
	
	float getSpeed();
	void SetTraction(float powerValue,float brakeValue);

private:
	void SourceInit();
	
	//添加IA和IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MyMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveForwardAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveRightAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressKeyEAction;
	
	void MoveForward(const FInputActionValue& inputValue);
	void MoveForwardRelease();
	void MoveRight(const FInputActionValue& inputValue);
	void MoveRightRelease();
	void Look(const FInputActionValue& inputValue);
	void PressE(const FInputActionValue& inputValue);
	
	UFUNCTION()
	void BeginOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
};



