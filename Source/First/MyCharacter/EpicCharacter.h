// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "First/MySoueces/Fire/Fire.h"
#include "First/MySoueces/Weapon/Bow/My_Bow.h"
#include "First/MyWidgets/My_WidgetManager.h"
#include "First/MyWidgets/My_BagSystem.h"
#include "First/MyWidgets/Blood/My_BloodSystem.h"
#include "GameFramework/Character.h"
#include "EpicCharacter.generated.h"

class AMyTestCar;
class ABaseLadder;
class UClimbMovementComponent;
class UClimbSystem;
class ABaseWorldAssets;
class ABoat;
class ASabre;
class ANPCBaseCharacter;

UCLASS()
class FIRST_API AEpicCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEpicCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float tickTime = 0.0f;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	USkeletalMeshComponent* skeletal;

	//相机和摇臂
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* cameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereCollision;
	UFUNCTION(BlueprintCallable)
	void DisableSphereCollision();
	
	UFUNCTION(BlueprintCallable)
	void SabreAttackEnd();
	
	//角色的pitch
	UFUNCTION(BlueprintCallable, Category = "CharacterParam")
	float GetCharacterPitch() const;
	
	UFUNCTION()
	void SightCheck();
	
	//攻击动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* FistAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SabreAttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReDrawBowMontage;
	
	TSet<ANPCBaseCharacter*> AlreadyAttackedNpcSet;	//存放一次攻击影响到的actor集合，防止对同一个目标二次伤害
	void FistAttack();
	bool isAttackAinmEnd = true;
	
	//瞄准系统
	UPROPERTY(BlueprintReadWrite, Category="Aiming")
	bool isAimingSystem=false;
	FVector OriginalCameraLocation;
	FVector AimingCameraLocation;	//进入瞄准视角后预设的相机位置，也就是相机的偏移量
	float OriginalFOV;
	
	FTimeline AimingTimeline;
	UPROPERTY(EditAnywhere)
	UCurveFloat* AimingCurve;	//timeline的曲线
	UFUNCTION()		//这里一定要使用UFUNCTION,因为后面用了回调函数
	void UpdateCameraPosition(float Value);
	
	// 背包系统
	bool bisBagSystem=false;
	UMy_BagSystem* BagSystem;
	
	// 血量系统
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMy_BloodSystem* BloodSystem;
	
	//弓箭对象
	AMy_Bow *MyBow;
	
	// WidgetManager对象
	UMy_WidgetManager* WidgetManagerComponent;
	
	AMyAssets* NextPickupActor;		//检测到的即将拾取的资源对象
	AMyAssets* HandResourceActor;	//手中资源对
	ANPCBaseCharacter* OpenWhichNPCBag;		//检测到的即将拾取的资源对象
	ABaseWorldAssets* OpenWhichWorldAssetsBag;		//检测到的即将拾取的资源对象
	bool isHandEmpty = true;
	bool isResourceBagOpen;
	bool isWorldAssetBagOpen;
	void UpdateHandActorByDrag(EMySourceType st);
	FName ChooseSlot(EMySourceType st) const;	// 根据资源类型选择 Socket
	void UpdateHandActorFromNull(EMySourceType st);

	//驾驶系统
	bool AbleDriver = false;
	bool IsDirving = false;
	AMyTestCar* boat;
	void CouldDrive(AMyTestCar* WhichBoat,bool couldOrNot);
	void SetIsDriving(bool isDrive);

	//攀爬系统
	bool AbleClimb = false;
	UPROPERTY()
	ABaseLadder* ClimbLadder;
	bool isClimbSystem = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClimbSystem* ClimbSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClimbMovementComponent* ClimbMovementComp; //移动组件
	float ClimbDistance = 0.0f;
	void EnterClimbState();
	void QuitClimbState();
	FVector GetLadderRungLocation(bool isNext);
	void AlignToLadder();

	
	UMovementComponent* GetCurrentMovementComponent();
	
private:
	void Construct();
	void SourceInit();
	void FirstPersonGame();
	void ThirdPersonGame();

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
	class UInputAction* MouseLeftAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseRightAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MouseWheelUpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressOneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressKeyEAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressKeyFAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressKeyQAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressShiftAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PressSpaceAction;
	
	void MoveForward(const FInputActionValue& inputValue);
	void MoveForward_Release(const FInputActionValue& inputValue);
	void MoveRight(const FInputActionValue& inputValue);
	void Look(const FInputActionValue& inputValue);
	void MouseWheelUp(const FInputActionValue& inputValue);
	void MouseLeft_Point(const FInputActionValue& inputValue);
	void MouseRight(const FInputActionValue& inputValue);
	void MouseRight_Release(const FInputActionValue& inputValue);
	void PressOne(const FInputActionValue& inputValue);
	void PressShift(const FInputActionValue& inputValue);
	void PressShift_Cancel(const FInputActionValue& inputValue);
	void PressF(const FInputActionValue& inputValue);
	void PressE(const FInputActionValue& inputValue);
	void PressQ(const FInputActionValue& inputValue);
	void PressSpace(const FInputActionValue& inputValue);
	
	//声明动态绑定函数
	UFUNCTION()
	void BeginOverlapFunction(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 声明重叠事件的回调函数
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
