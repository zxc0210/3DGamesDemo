// Fill out your copyright notice in the Description page of Project Settings.

#include "EpicCharacter.h"

#include "Boat.h"
#include "NPCBaseCharacter.h"
#include "Climb/ClimbMovementComponent.h"
#include "Climb/ClimbSystem.h"
#include "Components/SphereComponent.h"
#include "First/MySoueces/BaseWorldAssets/BaseWorldAssets.h"
#include "First/MySoueces/Ladder/BaseLadder.h"
#include "First/MySoueces/Weapon/Sabre/Sabre.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Vehicle/MyTestCar.h"


// Sets default values
AEpicCharacter::AEpicCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer) 
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->Construct();
    
	ClimbMovementComp = ObjectInitializer.CreateDefaultSubobject<UClimbMovementComponent>(this, TEXT("ClimbMovementComp"));    // 攀爬移动组件实例
    
	AutoPossessPlayer = EAutoReceiveInput::Player0;     //控制玩家0  
	this ->ThirdPersonGame();        //使用第三人称模式
}




void AEpicCharacter::Construct()
{
	skeletal = this->FindComponentByClass<USkeletalMeshComponent>();
	if (skeletal)
	{
		auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/_project/Character/Erika_Archer/Assets/Erika_Archer.Erika_Archer'"));
		if (assetSkeletal.Succeeded())
		{
			skeletal->SetSkeletalMesh(assetSkeletal.Object);
			skeletal->SetRelativeLocation(FVector(0, 0, -80));
			skeletal->SetRelativeRotation(FRotator(0, -100, 0));
		}
	}
	
	cameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraArm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	
	// 在构造函数中创建 Sphere 碰撞体
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(GetMesh()); // 先附加到角色网格上，稍后会调整到正确插槽
	SphereCollision->InitSphereRadius(10.0f);    // 设置碰撞体的半径
	SphereCollision->SetRelativeLocation(FVector(0, 80, 70));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//瞄准系统相关参数
	OriginalCameraLocation = FVector(0, 0, 0);
	AimingCameraLocation = FVector(0, 40, 40);
	OriginalFOV = camera->FieldOfView;		// 记录初始 FOV
	
	// WidgetManager对象
	WidgetManagerComponent = CreateDefaultSubobject<UMy_WidgetManager>(TEXT("WidgetManagerComponent"));

	// 背包系统
	BagSystem = CreateDefaultSubobject<UMy_BagSystem>(TEXT("BagSystem"));
	BagSystem ->SetOwningCharacter(this);
	
	// 血量系统
	BloodSystem = CreateDefaultSubobject<UMy_BloodSystem>(TEXT("BloodSystem"));

	// 攀爬系统
	ClimbSystem = CreateDefaultSubobject<UClimbSystem>(TEXT("ClimbSystem"));
	ClimbSystem->SetOwningCharacter(this);

	this->SourceInit();
}

//资源的绑定初始化
void AEpicCharacter::SourceInit()
{
	//设置动画蓝图
	static ConstructorHelpers::FClassFinder<UAnimInstance> MyCharacterAnimBP(TEXT("AnimBlueprint'/Game/_project/Character/Erika_Archer/Animation/MyArcher_AnimBP.MyArcher_AnimBP_C'"));
	if (MyCharacterAnimBP.Succeeded())
	{
		skeletal->SetAnimInstanceClass(MyCharacterAnimBP.Class);
	}
	
	AimingCurve = LoadObject<UCurveFloat>(nullptr, TEXT("/Game/_project/Others/TimelineCurve"));
	
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextFinder(TEXT("/Game/_project/Input/IMC_oriPerson"));
	if (MappingContextFinder.Succeeded())
	{
		MyMappingContext = MappingContextFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveForwardFinder(TEXT("/Game/_project/Input/Actions/MoveForward"));
	MoveForwardAction = MoveForwardFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveRightFinder(TEXT("/Game/_project/Input/Actions/MoveRight"));
	MoveRightAction = MoveRightFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> LookFinder(TEXT("/Game/_project/Input/Actions/Look"));
	LookAction = LookFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLeftFinder(TEXT("/Game/_project/Input/Actions/MouseLeft"));
	MouseLeftAction = MouseLeftFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseRightFinder(TEXT("/Game/_project/Input/Actions/MouseRight"));
	MouseRightAction = MouseRightFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> MouseWheelUpFinder(TEXT("/Game/_project/Input/Actions/MouseWheelUp"));
	MouseWheelUpAction = MouseWheelUpFinder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_1_Finder(TEXT("/Game/_project/Input/Actions/IA_Key1"));
	PressOneAction = Key_1_Finder.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Key_E_Finder(TEXT("/Game/_project/Input/Actions/IA_KeyE"));
	PressKeyEAction = Key_E_Finder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_F_Finder(TEXT("/Game/_project/Input/Actions/IA_KeyF"));
	PressKeyFAction = Key_F_Finder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_Q_Finder(TEXT("/Game/_project/Input/Actions/IA_KeyQ"));
	PressKeyQAction = Key_Q_Finder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_Shift_Finder(TEXT("/Game/_project/Input/Actions/IA_Shift"));
	PressShiftAction = Key_Shift_Finder.Object;
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_Space_Finder(TEXT("/Game/_project/Input/Actions/IA_Space"));
	PressSpaceAction = Key_Space_Finder.Object;
	
	// 初始化动画蒙太奇
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj(TEXT("AnimMontage'/Game/_project/Character/Erika_Archer/Animation/Montage/ArcherCharacter_PaunchMontage'"));
	if (MontageObj.Succeeded())
	{
		FistAttackMontage = MontageObj.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj2(TEXT("AnimMontage'/Game/_project/Character/Erika_Archer/Animation/Montage/ArcherCharacter_SabreAttack_Montage'"));
	if (MontageObj2.Succeeded())
	{
		SabreAttackMontage = MontageObj2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageObj3(TEXT("AnimMontage'/Game/_project/Character/Erika_Archer/Animation/Montage/ArcherCharacter_ReDrawBow_Montage'"));
	if (MontageObj3.Succeeded())
	{
		ReDrawBowMontage = MontageObj3.Object;
	}
}

// Called when the game starts or when spawned
void AEpicCharacter::BeginPlay()
{
	Super::BeginPlay();

	ClimbMovementComp->Deactivate();    // 默认不启用

	GetCharacterMovement()->bOrientRotationToMovement = false;		//角色移动时人物朝向发生移动的方向
	cameraArm->bDoCollisionTest = false;
	
	//必须在BeginPlay()中初始化timeline组件，否则可能导致异常
	if (AimingCurve)
	{
		FOnTimelineFloat TimelineCallback;
		TimelineCallback.BindUFunction(this, FName("UpdateCameraPosition"));
		AimingTimeline.AddInterpFloat(AimingCurve, TimelineCallback);
		AimingTimeline.SetTimelineLength(1.0f); // 设置过渡时间为 0.25秒
		AimingTimeline.SetPlayRate(4.0f);
	}
	else { UE_LOG(LogTemp, Error, TEXT("AimingCurve is not set!")); }

	UCapsuleComponent* myCapsuleComponent = GetCapsuleComponent();
	if (myCapsuleComponent)
	{
		myCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AEpicCharacter::BeginOverlapFunction);
		myCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &AEpicCharacter::EndOverlapFunction);
	}
	
	if (SphereCollision)
	{
		// 设置 SphereCollision 的位置和旋转与插槽一致
		SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AEpicCharacter::OnSphereBeginOverlap);
		SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AEpicCharacter::OnSphereEndOverlap);
	}

	WidgetManagerComponent->Point_Show();	//显示中心点
	BloodSystem->BloodUI_Show();

	bisBagSystem=true;
	BagSystem->BagUI_Show();
	
}

UMovementComponent* AEpicCharacter::GetCurrentMovementComponent()
{
	// 优先返回激活的自定义组件
	if (ClimbMovementComp->IsActive())
	{
		return ClimbMovementComp;
	}
	return GetCharacterMovement();
}

// Called every frame
void AEpicCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// 这行必须调用，否则无法触发下面的TimelineCallback函数，动画也就无从实现了
	AimingTimeline.TickTimeline(DeltaTime);
	// 更新相机位置，确保相机与角色旋转保持同步
	if (isAimingSystem)
	{
		UpdateCameraPosition(AimingTimeline.GetPlaybackPosition());
	}

	if (tickTime>0.1f)
	{
		SightCheck();
		tickTime = 0.0f;
	}
	tickTime += DeltaTime;
}

// Called to bind functionality to input
void AEpicCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MyMappingContext, 0);
		}
	}

	//绑定输入操作对应的函数
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this,&AEpicCharacter::MoveForward);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this,&AEpicCharacter::MoveForward_Release);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AEpicCharacter::MoveRight);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEpicCharacter::Look);
		EnhancedInputComponent->BindAction(MouseWheelUpAction, ETriggerEvent::Triggered, this,&AEpicCharacter::MouseWheelUp);
		EnhancedInputComponent->BindAction(MouseLeftAction, ETriggerEvent::Started, this, &AEpicCharacter::MouseLeft_Point);
		EnhancedInputComponent->BindAction(MouseRightAction, ETriggerEvent::Started, this, &AEpicCharacter::MouseRight);
		EnhancedInputComponent->BindAction(MouseRightAction, ETriggerEvent::Completed, this, &AEpicCharacter::MouseRight_Release);
		EnhancedInputComponent->BindAction(PressOneAction, ETriggerEvent::Started, this, &AEpicCharacter::PressOne);
		EnhancedInputComponent->BindAction(PressKeyEAction, ETriggerEvent::Started, this, &AEpicCharacter::PressE);
		EnhancedInputComponent->BindAction(PressKeyFAction, ETriggerEvent::Started, this, &AEpicCharacter::PressF);
		EnhancedInputComponent->BindAction(PressKeyQAction, ETriggerEvent::Started, this, &AEpicCharacter::PressQ);
		EnhancedInputComponent->BindAction(PressShiftAction, ETriggerEvent::Started, this, &AEpicCharacter::PressShift);
		EnhancedInputComponent->BindAction(PressShiftAction, ETriggerEvent::Completed, this,&AEpicCharacter::PressShift_Cancel);
		EnhancedInputComponent->BindAction(PressSpaceAction, ETriggerEvent::Started, this, &AEpicCharacter::PressSpace);
		//EnhancedInputComponent->BindAction(PressSpaceAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
	
}


float AEpicCharacter::GetCharacterPitch() const
{
	float Pitch = 0.0f;
	FRotator ControlRotation = GetControlRotation();	// 获取角色的控制旋转
	if (ControlRotation.Pitch>260)
	{
		Pitch=ControlRotation.Pitch - 360.0f;
	}
	if (ControlRotation.Pitch<100)
	{
		Pitch=ControlRotation.Pitch;
	}
	return Pitch;
}

void AEpicCharacter::FirstPersonGame()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;		//相机旋转时人物跟随旋转,确保相机始终对准人物的背面
	bUseControllerRotationRoll = false;
	
	camera->SetRelativeLocation(FVector(50, 0, 50));
	camera->SetRelativeRotation(FRotator(0, 0, 0));
	camera->bUsePawnControlRotation = true;		
}

//组件的初始化
void AEpicCharacter::ThirdPersonGame()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;		//true：相机旋转时人物跟随旋转,确保相机始终对准人物的背面
	bUseControllerRotationRoll = false;

	cameraArm->SetupAttachment(skeletal);			//绑定层级
	cameraArm->bUsePawnControlRotation = true;			//使用Pawn控制器（鼠标）控制相机臂跟随旋转
	cameraArm->SetRelativeLocation(FVector(0, 0, 100));
	cameraArm->TargetArmLength = 500;
	cameraArm->bDoCollisionTest = false;	//禁用相机本身的碰撞检测，防止由于相机碰撞导致意料之外的情况出现

	camera->SetupAttachment(cameraArm);
	camera->SetRelativeLocation(FVector(0, 60, 80));
	camera->bUsePawnControlRotation = false;	//是否只旋转相机，也就是视角原地变化
}   

void AEpicCharacter::MoveForward(const FInputActionValue& inputValue)
{
	float Value = inputValue.GetMagnitude();
	if (!IsDirving && boat)
	{
		boat->SetActorTickEnabled(false);	//禁用tick
	}
	
	if (isClimbSystem)
	{
		//攀爬状态：WS=上下移动（基于角色面朝方向的垂直轴）
		// const FVector DirectionVector = FVector::CrossProduct(GetActorForwardVector(), GetActorRightVector());
		// AddMovementInput(DirectionVector, Value);
	}
	else
	{
		// 默认状态：WS=前后移动
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);
		FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void AEpicCharacter::MoveForward_Release(const FInputActionValue& inputValue)
{
	if (!isClimbSystem) return;

	//ClimbDistance =
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Realse"));

}

void AEpicCharacter::MoveRight(const FInputActionValue& inputValue)
{
	if (isClimbSystem) return;

	float value = inputValue.GetMagnitude();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, value);
	}
}

void AEpicCharacter::Look(const FInputActionValue& inputValue)
{
	FVector2D LookVector = inputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void AEpicCharacter::MouseWheelUp(const FInputActionValue& inputValue)
{
	if (bisBagSystem)
	{
		EMySourceType CurrentGridType = BagSystem->WheelUp(inputValue.GetMagnitude()); //修改当前格子并获取变换后的格子资源类型
		if (!isHandEmpty)	//如果现在手中不为空那就销毁
		{
			isHandEmpty = true;
			HandResourceActor->Destroy();	
			HandResourceActor = nullptr;
		}
		//然后根据变换后的类型进行创建
		if (CurrentGridType != EMySourceType::NoneSource)
		{
			// 获取对应的资源类
			TSubclassOf<AMyAssets> ResourceClass = AMyAssets::GetActorClassByType(CurrentGridType);
			if (ResourceClass)
			{
				// 生成新的资源对象并附着到角色
				HandResourceActor = GetWorld()->SpawnActor<AMyAssets>(ResourceClass, GetActorLocation(), FRotator::ZeroRotator);
				FName TargetSocket = ChooseSlot(HandResourceActor->GetSourceTypeName());
				HandResourceActor->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TargetSocket);
				isHandEmpty = false;
			}
		}
	}
	else
	{
		float value = inputValue.GetMagnitude();
		float armLenth = cameraArm->TargetArmLength;
		armLenth -= value * 50;
		if (armLenth >= 600)
		{
			armLenth = 600;
		}
		else if (armLenth <= 0)
		{
			armLenth = 0;
			this->FirstPersonGame();
		}
		else
		{
			this->ThirdPersonGame();
		}
		cameraArm->TargetArmLength = armLenth;
	}
}

void AEpicCharacter::MouseLeft_Point(const FInputActionValue& inputValue)
{
	EMySourceType st;
	if (isHandEmpty)
	{
		st=EMySourceType::NoneSource;
	}
	else
	{
		st = HandResourceActor->GetSourceTypeName();
	}
	
	switch (st)
	{
	case EMySourceType::NoneSource:	//空手
		if (isAttackAinmEnd)	//一次攻击
		{
			GetMesh()->GetAnimInstance()->Montage_Play(FistAttackMontage,0.25f);
			isAttackAinmEnd = false;
			//启动重叠事件检测
			SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
			SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); 
			SphereCollision->IgnoreActorWhenMoving(this,true);
		}
		break;
	case EMySourceType::Sabre:
		{
			ASabre* sabre = Cast<ASabre>(HandResourceActor);
			if (isAttackAinmEnd)	//一次攻击
			{
				GetMesh()->GetAnimInstance()->Montage_Play(SabreAttackMontage,0.33f);
				sabre->SabreAttackStart();
				isAttackAinmEnd = false;
			}
			break;
		}
		
	case EMySourceType::Bow:
		if (isAimingSystem)
		{
			if (MyBow)
			{
				if (BagSystem->ReduceSpecialResource(EMySourceType::Arrow))	//如果有箭的话
				{
					MyBow->Bow_Fire();
					GetMesh()->GetAnimInstance()->Montage_Play(ReDrawBowMontage,1.0f);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Have No Arrow"));
				}
			}
		}
		break;
	default:
		break;
	}
	
}

void AEpicCharacter::MouseRight(const FInputActionValue& inputValue)
{
	if (isHandEmpty)
	{
		return;
	}
	
	if (HandResourceActor->GetSourceTypeName()==EMySourceType::Bow)
	{
		MyBow = Cast<AMy_Bow>(HandResourceActor);
		isAimingSystem=true;
		AimingTimeline.PlayFromStart();			//调用timeline改变相机视角

		if (MyBow)
		{
			MyBow->Bow_AimingOnly();
		}
		WidgetManagerComponent->Crosshair_Show();	// 显示准星
	
	}
}

void AEpicCharacter::MouseRight_Release(const FInputActionValue& inputValue)
{
	
	// 获取松开右键时timeline的时间，用于设置蓄力的大小
	float CurrentTime = AimingTimeline.GetPlaybackPosition();
	
	AimingTimeline.Reverse();	//还原视角         
	
	if (MyBow)
	{
		MyBow->Bow_Idle();
	}
	WidgetManagerComponent->Crosshair_Hide(); // 隐藏准星
	isAimingSystem=false;
}

void AEpicCharacter::UpdateCameraPosition(float Value)
{
	//调整相机位置
	FVector RightVector = GetActorRightVector();
	FVector ForwardVector = GetActorForwardVector();
	//计算角色在当前朝向下，要达到瞄准相机位置在xyz方向分别需要的偏移量
	FVector AimingOffset = AimingCameraLocation.X * ForwardVector + AimingCameraLocation.Y * RightVector + FVector(0, 0, AimingCameraLocation.Z);
	//根据偏移量调整相机位置
	FVector NewLocation = FMath::Lerp(OriginalCameraLocation, OriginalCameraLocation + AimingOffset, Value);
	cameraArm->TargetOffset = NewLocation;
	
	//调整相机视场
	float NewFOV = FMath::Lerp(OriginalFOV, OriginalFOV - 20, Value);
	camera->SetFieldOfView(NewFOV);
	
}

void AEpicCharacter::PressOne(const FInputActionValue& inputValue)
{
	// if (MyBow == nullptr)
	// {
	// 	// 创建弓箭对象并设置初始位置
	// 	MyBow = GetWorld()->SpawnActor<AMy_Bow>(AMy_Bow::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	// 	// 将弓箭对象附着到角色的左手插槽上
	// 	MyBow->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Hand_LSocket"));	
	// }
	
	// if (MyFire == nullptr)
	// {
	// 	// 创建弓箭对象并设置初始位置
	// 	MyFire = GetWorld()->SpawnActor<AFire>(AFire::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	// 	MyFire->DisablePhysics();
	// 	// 将弓箭对象附着到角色的左手插槽上
	// 	MyFire->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HitJudgeSlot"));
	// }
	if (HandResourceActor == nullptr)
	{
		// 创建军刀对象并设置初始位置
		HandResourceActor = GetWorld()->SpawnActor<ASabre>(ASabre::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		// 将军刀对象附着到角色的左手插槽上
		HandResourceActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Hand_RSocket_Sabre"));
		isHandEmpty = false;
	}
}

void AEpicCharacter::PressE(const FInputActionValue& inputValue)
{
	SightCheck();

	if (NextPickupActor)
	{
		BagSystem->AddResourceToBag(NextPickupActor->GetSourceTypeName(), isHandEmpty);	//添加到背包
		if (HandResourceActor == nullptr)	//现在手中没有东西
		{
			UpdateHandActorFromNull(NextPickupActor->GetSourceTypeName());
		}
		NextPickupActor->Destroy();		// 拾取后销毁Actor实例
		NextPickupActor = nullptr;
		return;
	}
	if (OpenWhichNPCBag)
	{
		if (!isResourceBagOpen)
		{
			BagSystem->OpenResourceBag(OpenWhichNPCBag, isHandEmpty);
			isResourceBagOpen = true;
		}
		else
		{
			BagSystem->RemoveResourceBagGrid();
			isResourceBagOpen = false;
		}
		return;
	}
	if (OpenWhichWorldAssetsBag)
	{
		if (!isWorldAssetBagOpen)
		{
			FActorResourceData* ActorResourceData = BagSystem->OpenWorldAssetBag(OpenWhichWorldAssetsBag, isHandEmpty);//全局资源列表中该Actor的资源数
			OpenWhichWorldAssetsBag->UI_Show();
			OpenWhichWorldAssetsBag->UI_Update(ActorResourceData);
			isWorldAssetBagOpen = true;
		}
		else
		{
			BagSystem->ClearActorID();
			OpenWhichWorldAssetsBag->UI_Hide();
			isWorldAssetBagOpen = false;
		}
		return;
	}
	if (AbleDriver)
	{
		this->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		IsDirving = true;
		boat->DriveBoat(this);
		//this->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		return;
	}
	if (AbleClimb)
	{
		if (!isClimbSystem)
		{
			EnterClimbState();
			if (true)	//默认是爬梯子
			{
				FVector Location = GetLadderRungLocation(false);
				ClimbSystem->ToClimbMontagePlay(Location);	//播放动画蒙太奇
				AlignToLadder();
				isClimbSystem = true;
			}
		}
		else
		{
			QuitClimbState();
			ClimbSystem->LeaveClimbMontagePlay();	//播放动画蒙太奇
		}
	}
}

void AEpicCharacter::SightCheck()
{
	// 获取摄像机的位置和旋转
	FVector CameraLocation=camera->GetComponentLocation();
	FRotator CameraRotation=camera->GetComponentRotation();
	FVector LineTraceEnd = CameraLocation + (CameraRotation.Vector() * 1200.0f);	// 瞄准方向从摄像机初始位置开始，到距前向矢量1000个单位距离的位置结束
	
	FHitResult HitResult;	// 射线追踪
	GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, LineTraceEnd, ECC_Visibility);

	if (OpenWhichWorldAssetsBag)	//先取消高亮
	{
		OpenWhichWorldAssetsBag->DisableHighLight();
	}
	if (NextPickupActor)
	{
		NextPickupActor->DisableHighLight();
	}
	if (HitResult.bBlockingHit)
	{
		// 判断击中物体是否为 AMyAssets 类或其子类
		if (HitResult.GetActor()->IsA(AMyAssets::StaticClass()))
		{
			NextPickupActor = Cast<AMyAssets>(HitResult.GetActor());	 //获取击中物体的指针
			if (!NextPickupActor->isPickedAsset())
			{
				NextPickupActor = nullptr;
			}
			NextPickupActor->SetHighLight();	//高亮显示
			OpenWhichNPCBag = nullptr;
			OpenWhichWorldAssetsBag = nullptr;
		}
		else if (HitResult.GetActor()->IsA(ANPCBaseCharacter::StaticClass()))
		{
			OpenWhichNPCBag = Cast<ANPCBaseCharacter>(HitResult.GetActor());
			OpenWhichWorldAssetsBag = nullptr;	//这里物品击中后已经被销毁了所以只要另一个
		}
		else if (HitResult.GetActor()->IsA(ABaseWorldAssets::StaticClass()))
		{
			OpenWhichWorldAssetsBag = Cast<ABaseWorldAssets>(HitResult.GetActor());
			OpenWhichWorldAssetsBag->SetHighLight();	//高亮显示
			OpenWhichNPCBag = nullptr;
		}
		else
		{
			NextPickupActor = nullptr;
			OpenWhichNPCBag = nullptr;
			OpenWhichWorldAssetsBag = nullptr;
		}
		// 绘制击中物体的射线
		//DrawDebugLine(GetWorld(), CameraLocation, HitResult.Location, FColor::Red, false, 1.0f, 0, 5.0f);
	}
	else{}	
}


void AEpicCharacter::PressF(const FInputActionValue& inputValue)
{
	if (isAimingSystem)
	{
		return;
	}
	if (!isHandEmpty)
	{
		//获取角色各方向矢量
		FVector RightVector = GetActorRightVector();
		FVector ForwardVector = GetActorForwardVector();
		FVector DropLocation = FVector(100,0,-100);  //检测终点与起始点的偏移量
		// 进行射线检测以找到地面
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector OffsetVector = DropLocation.X * ForwardVector + DropLocation.Y * RightVector + FVector(0, 0, DropLocation.Z); //计算当前位置到坠落地点的偏移量
		FVector TraceEndLocation = StartLocation + 2 * OffsetVector;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.AddIgnoredActor(HandResourceActor);
		GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, TraceEndLocation, ECC_Visibility, QueryParams);	//射线检测

		//移除附着
		HandResourceActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		HandResourceActor->SetActorLocation(StartLocation);
		
		if (HitResult.bBlockingHit)  //是否有碰撞发生
		{
			FRotator GroundRotation;
			if (HandResourceActor->GetSourceTypeName() == EMySourceType::Fire )
			{
				GroundRotation = UKismetMathLibrary::MakeRotFromXZ(HitResult.ImpactNormal, ForwardVector);  // X与击中位置法线方向对齐，Z与角色前向矢量方向对齐
			}
			else if (HandResourceActor->GetSourceTypeName() == EMySourceType::Arrow || HandResourceActor->GetSourceTypeName() == EMySourceType::Iron )
			{
				GroundRotation = UKismetMathLibrary::MakeRotFromZX(HitResult.ImpactNormal, ForwardVector);  // X与击中位置法线方向对齐，Z与角色前向矢量方向对齐
			}
			else
			{
				GroundRotation = UKismetMathLibrary::MakeRotFromZY(HitResult.ImpactNormal, ForwardVector);  // X与击中位置法线方向对齐，Z与角色前向矢量方向对齐
			}
			HandResourceActor->Throw(StartLocation, HitResult.ImpactPoint, GroundRotation.Quaternion(), true);	//将FRotator转化为四元数FQuat
			if (HitResult.ImpactNormal.Z < 0.5f) //如果斜面角度过高
			{
				//todo 沿着斜面滑落
			}
		}
		else
		{
			//没有击中，直接扔到偏移位置
			HandResourceActor->Throw(StartLocation, TraceEndLocation, FQuat::Identity, false);
		}
		
		//手中物品已丢完
		if (BagSystem->ReduceResourceFromBag() == 0)	
		{
			HandResourceActor = nullptr; 
			isHandEmpty = true;
		}
		else
		{
			// 物品还有剩余，那么创建一个相同的资源对象
			UClass* ActorClass = HandResourceActor->GetClass(); // 获取父类对象指向的实际的子类
			HandResourceActor = GetWorld()->SpawnActor<AMyAssets>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator);
			FName TargetSocket = ChooseSlot(HandResourceActor->GetSourceTypeName());
			HandResourceActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TargetSocket);
		}
	}
}

void AEpicCharacter::PressQ(const FInputActionValue& inputValue)
{
	if (!bisBagSystem)
	{
		bisBagSystem=true;
		BagSystem->BagUI_Show();
	}
	else 
	{
		if (!isResourceBagOpen)
		{
			BagSystem->BagUI_Hide();
			bisBagSystem = false;
		}
	}
}

void AEpicCharacter::UpdateHandActorByDrag(EMySourceType st)
{
	if (!isHandEmpty)
	{
		HandResourceActor->Destroy();	
		HandResourceActor = nullptr;
		isHandEmpty = true;
	}
	UpdateHandActorFromNull(st);
}

FName AEpicCharacter::ChooseSlot(EMySourceType st) const
{
	switch (st)
	{
	case EMySourceType::Sabre:
		return TEXT("Hand_RSocket_Sabre");
	case EMySourceType::Plank:
		return TEXT("Hand_RSocket_Plank");
	case EMySourceType::Iron:
		return TEXT("Hand_RSocket_Iron");
	case EMySourceType::Arrow:
		return TEXT("Hand_RSocket_Arrow");
	case EMySourceType::Fire:
		return TEXT("Hand_RSocket");
	case EMySourceType::Bow:
		return TEXT("Hand_LSocket"); 
	default:
		return TEXT("Hand_RSocket");
	}
}

void AEpicCharacter::UpdateHandActorFromNull(EMySourceType st)
{
	TSubclassOf<AMyAssets> ResourceClass = AMyAssets::GetActorClassByType(st);
	if (ResourceClass)
	{
		// 生成新的资源对象并附着到角色
		HandResourceActor = GetWorld()->SpawnActor<AMyAssets>(ResourceClass, GetActorLocation(), FRotator::ZeroRotator);
		FName TargetSocket = ChooseSlot(st);
		HandResourceActor->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TargetSocket);
		isHandEmpty = false;
	}
}

void AEpicCharacter::PressShift(const FInputActionValue& inputValue)
{
	SightCheck();
	// if (HandResourceActor == nullptr)
	// {
	// 	// 创建弓箭对象并设置初始位置
	// 	HandResourceActor = GetWorld()->SpawnActor<AMy_Bow>(AMy_Bow::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	// 	// 将弓箭对象附着到角色的左手插槽上
	// 	HandResourceActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Hand_LSocket"));
	// 	isHandEmpty = false;
	// 	MyBow = Cast<AMy_Bow>(HandResourceActor);
	// }
}

void AEpicCharacter::PressShift_Cancel(const FInputActionValue& inputValue)
{
	if (FistAttackMontage && GetMesh()->GetAnimInstance())
	{
		//GetMesh()->GetAnimInstance()->Montage_Stop(0.05f, MontageToPlay); // 0.2f 为停止动画的淡出时间，可以根据需要调整
	}
}

void AEpicCharacter::PressSpace(const FInputActionValue& inputValue)
{
	if (isClimbSystem)
	{
		if (true)
		{
			// 跳跃式爬梯子
			FVector NextRungLocation = GetLadderRungLocation(true);
			if ((FMath::Abs(NextRungLocation.Z - GetActorLocation().Z) < 5)) //如果梯子有一点点倾斜就不会满足，因此这里我们只看Z
			{
				//离开梯子
				ClimbSystem->ClimbToLadderTopMontagePlay();
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Exited"));
				QuitClimbState();
				return;
			}
			ClimbSystem->ClimbUpMontagePlay(NextRungLocation);
		}
	}
}

void AEpicCharacter::DisableSphereCollision()
{
	//动画结束后从蓝图通知调用
	isAttackAinmEnd = true;

	AlreadyAttackedNpcSet.Empty();	//清空本次攻击影响到的actor集合
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AEpicCharacter::SabreAttackEnd()
{
	isAttackAinmEnd = true;
	ASabre* sabre = Cast<ASabre>(HandResourceActor);
	sabre->SabreAttackEnd();
}

void AEpicCharacter::FistAttack()
{
	
}

void AEpicCharacter::CouldDrive(AMyTestCar* WhichBoat, bool couldOrNot)
{
	if (couldOrNot)
	{
		boat = WhichBoat;
		AbleDriver = true;
	}
	else
	{
		//boat = nullptr;
		AbleDriver = false;
	}
}

void AEpicCharacter::SetIsDriving(bool isDrive)
{
	IsDirving = isDrive;
}

void AEpicCharacter::EnterClimbState()
{
	// 启用攀爬组件
	GetCurrentMovementComponent()->Deactivate();
	ClimbMovementComp->Activate(true);
	ClimbMovementComp->SetUpdatedComponent(GetMesh()); // 绑定到骨骼网格体
	bUseControllerRotationYaw = false;
	ClimbMovementComp->StartClimbing(true);
}

void AEpicCharacter::QuitClimbState()
{
	// 禁用攀爬组件
	ClimbMovementComp->StartClimbing(false);
	ClimbMovementComp->Deactivate();
	GetCurrentMovementComponent()->Activate(true);
	bUseControllerRotationYaw = true;
	isClimbSystem = false;

}

FVector AEpicCharacter::GetLadderRungLocation(bool isNext)
{
	
	FVector CharacterHeadLocation = GetActorLocation();
	CharacterHeadLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();	// 计算角色头部位置
	FVector ClosestRungLocation = CharacterHeadLocation;
	if (ClimbLadder)
	{
		ClosestRungLocation = ClimbLadder->GetClosestRungsLocation(CharacterHeadLocation,isNext);	// 获取最近的横档偏移位置
	}
	return ClosestRungLocation;
}

void AEpicCharacter::AlignToLadder()
{
	if (ClimbLadder)
	{
		FVector LadderNormal = ClimbLadder->GetNormal();
        
		// 仅需要调整 Yaw方向的旋转
		FRotator TargetRotation = LadderNormal.Rotation();
		TargetRotation.Pitch = 0.0f;
		TargetRotation.Roll = 0.0f;
		SetActorRotation(TargetRotation);
	}
}

void AEpicCharacter::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	FString String = FString::Printf(TEXT("%s"), *OtherActor->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, String);

	if (OtherActor && (OtherActor != this) && OtherActor->IsA(ABaseLadder::StaticClass()))
	{
		AbleClimb = true;
		ClimbLadder = Cast<ABaseLadder>(OtherActor);
	}
}

void AEpicCharacter::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(ABaseLadder::StaticClass()))
	{
		AbleClimb = false;
		ClimbLadder = nullptr;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Exited Ladder Collision"));
	}
}

void AEpicCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(ANPCBaseCharacter::StaticClass()) )
	{
		FString String = FString::Printf(TEXT("%s"), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, String);
		
		ANPCBaseCharacter* AttackedNPC = Cast<ANPCBaseCharacter>(OtherActor);
		if (AttackedNPC)
		{
			// 检查是否已经攻击过这个NPC
			if (!AlreadyAttackedNpcSet.Contains(AttackedNPC))
			{
				// 没有攻击过才减少血量并记录
				AttackedNPC->ReduceNPCBlood(8);
				AlreadyAttackedNpcSet.Add(AttackedNPC);
			}
			
		}
	}
}

void AEpicCharacter::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
	
	}
}