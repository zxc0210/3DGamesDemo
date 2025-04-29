// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Boat.generated.h"

class UFloatingPawnMovement;
class AEpicCharacter;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UBoxComponent;

UCLASS()
class FIRST_API ABoat : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoat();

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
	UStaticMeshComponent* ShipWheel;

	//相机和摇臂
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* cameraArm;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* boxCollision;
	

	AEpicCharacter* DrivingChara;
	bool bIsDriving = false;
	void DriveBoat(AEpicCharacter* chara);

	float CurrentRotationAngle = 0.0f;
	void WheelRolling(float DeltaTime);

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
	void MoveRight(const FInputActionValue& inputValue);
	void Look(const FInputActionValue& inputValue);
	void PressE(const FInputActionValue& inputValue);
	
	UFUNCTION()
	void BeginOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);
	
};


