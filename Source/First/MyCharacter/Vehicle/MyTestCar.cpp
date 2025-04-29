// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTestCar.h"

#include "ChaosVehicleMovementComponent.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FrontWheel.h"
#include "RearWheel.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "First/MyCharacter/EpicCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "First/MySoueces/Ladder/BaseLadder.h"
#include "First/MyWidgets/Boiler/Boiler.h"
#include "Uobject/ConstructorHelpers.h"

// Sets default values
AMyTestCar::AMyTestCar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	auto assetSkeletal = ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("SkeletalMesh'/Game/Vehicles/OffroadCar/SKM_Offroad.SKM_Offroad'"));
	GetMesh()->SetSkeletalMesh(assetSkeletal.Object);
	GetMesh()->SetSimulatePhysics(true);

	//请注意静态网格的位置不能低于骨骼网格
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(TEXT("/Game/Vehicles/OffroadCar/SM_Offroad_Body"));
	mc = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM"));
	mc->SetStaticMesh(StaticM.Object);
	mc->SetWorldScale3D(FVector(0.1,0.1,0.1));
	mc->SetRelativeRotation(FRotator(0,0,0));
	mc->SetupAttachment(RootComponent);
	mc->SetCollisionEnabled(ECollisionEnabled::NoCollision); //无碰撞
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM2(TEXT("/Game/_project/Character/Boat/Ship/Meshes/SM_ShipMainBody_00"));
	mc2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM2"));
	mc2->SetStaticMesh(StaticM2.Object);
	mc2->SetRelativeRotation(FRotator(0,-90,0));
	

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipWheelTableMesh(TEXT("StaticMesh'/Game/_project/Character/Boat/Ship/Meshes/SM_ShipWheel_002.SM_ShipWheel_002'"));
	ShipWheelTable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipWheelTable"));
	ShipWheelTable->SetStaticMesh(ShipWheelTableMesh.Object);
	ShipWheelTable->SetupAttachment(mc2, TEXT("ShipWheelSocket"));
	ShipWheelTable->SetRelativeRotation(FRotator(0,0,-90));
	ShipWheelTable->SetRelativeLocation(FVector(0,-3170,1210));
	ShipWheelTable->SetCollisionEnabled(ECollisionEnabled::NoCollision); //无碰撞
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipWheelMesh(TEXT("StaticMesh'/Game/_project/Character/Boat/Ship/Meshes/SM_ShipWheel_001.SM_ShipWheel_001'"));
	ShipWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipWheel"));
	ShipWheel->SetStaticMesh(ShipWheelMesh.Object);
	ShipWheel->SetupAttachment(mc2, TEXT("ShipWheelSocket"));
	ShipWheel->SetRelativeRotation(FRotator(0,0,-90));
	ShipWheel->SetRelativeLocation(FVector(0,-3170,1210));
	
	
	cameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraArm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	cameraArm->SetupAttachment(ShipWheel);			//绑定层级
	cameraArm->bUsePawnControlRotation = true;			//使用Pawn控制器（鼠标）控制相机臂跟随旋转
	cameraArm->SetRelativeRotation(FRotator(0,0,0));
	cameraArm->TargetArmLength = 1500;
	cameraArm->bDoCollisionTest = false;	//禁用相机本身的碰撞检测，防止由于相机碰撞导致意料之外的情况出现
	camera->SetupAttachment(cameraArm);
	camera->SetRelativeLocation(FVector(0, 60, 80));
	camera->bUsePawnControlRotation = false;	//是否只旋转相机，也就是视角原地变化

	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollision"));
	boxCollision->SetupAttachment(ShipWheel);
	boxCollision->InitBoxExtent(FVector(100,100,100));
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	boxCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AMyTestCar::BeginOverlapFunction);
	boxCollision->OnComponentEndOverlap.AddDynamic(this, &AMyTestCar::EndOverlapFunction);
	
	this->SourceInit();
	
	WheelMovement = Cast<UChaosWheeledVehicleMovementComponent>(GetVehicleMovementComponent());
	WheelMovement->WheelSetups.SetNum(WheelNum);
	WheelMovement->EngineSetup.TorqueCurve.ExternalCurve = EngineCurve;
	
	// 左前轮
	WheelMovement->WheelSetups[0].WheelClass = UFrontWheel::StaticClass();
	WheelMovement->WheelSetups[0].BoneName = TEXT("PhysWheel_FL"); // 骨骼名需匹配
		
	// 右前轮
	WheelMovement->WheelSetups[1].WheelClass = UFrontWheel::StaticClass();
	WheelMovement->WheelSetups[1].BoneName = TEXT("PhysWheel_FR");
	
	// 左后轮
	WheelMovement->WheelSetups[2].WheelClass = URearWheel::StaticClass();
	WheelMovement->WheelSetups[2].BoneName = TEXT("PhysWheel_BL");
	WheelMovement->WheelSetups[2].AdditionalOffset = FVector(0, 0, 0);

	// 右后轮
	WheelMovement->WheelSetups[3].WheelClass = URearWheel::StaticClass();
	WheelMovement->WheelSetups[3].BoneName = TEXT("PhysWheel_BR");
}


void AMyTestCar::SourceInit()
{
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
	
	static ConstructorHelpers::FObjectFinder<UInputAction> Key_E_Finder(TEXT("/Game/_project/Input/Actions/IA_KeyE"));
	PressKeyEAction = Key_E_Finder.Object;
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFinder(TEXT("/Game/VehicleTemplate/Blueprints/OffroadCar/OffroadCar_TorqueCurve"));
	EngineCurve = CurveFinder.Object;
	
}

void AMyTestCar::AttachActorsToShip()
{
	//锅炉
	AttachedBoiler = GetWorld()->SpawnActor<ABoiler>(ABoiler::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
	AttachedBoiler->AttachToComponent(mc2, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
  
	AttachedBoiler->SetActorRelativeLocation(FVector(0,-3850,1210));
	AttachedBoiler->SetActorRelativeRotation(FRotator(0, 90, 0));

	// 梯子
	LeftLadder = GetWorld()->SpawnActor<ABaseLadder>(ABaseLadder::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
	RightLadder = GetWorld()->SpawnActor<ABaseLadder>(ABaseLadder::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
	
	LeftLadder->AttachToComponent(mc2,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	LeftLadder->SetActorRelativeLocation(FVector(-800, -1000, 0));
	LeftLadder->SetLadderRungsNum(16);
	
	RightLadder->AttachToComponent(mc2,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	RightLadder->SetActorRelativeLocation(FVector(800, -1000, 0));
	RightLadder->SetActorRelativeRotation(FRotator(0, 180, 0));
	RightLadder->SetLadderRungsNum(16);
}

// Called when the game starts or when spawned
void AMyTestCar::BeginPlay()
{
	Super::BeginPlay();
	
	//this->SetActorTickEnabled(false);	//禁用tick
	// 创建并附着Actor
	AttachActorsToShip();
	
}

// Called every frame
void AMyTestCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(isStopTraction)
	{
		float speed = getSpeed();
		if(speed<1 && speed>-1)
		{
			SetTraction(0.0f,0.0f);
			isStopTraction = false;
		}
		
	}

	FRotator mcWorldRotation = mc->GetComponentRotation();
	mc2->SetWorldLocation(mc->GetComponentLocation() + FVector(0, 0, 500));
	mc2->SetWorldRotation(FRotator(
		mcWorldRotation.Pitch,
		mcWorldRotation.Yaw - 90.0f, // Yaw方向增加90度
		mcWorldRotation.Roll
	));
	
	// if (ShipWheel)
	// {
	// 	WheelRolling(DeltaTime);
	// }

	if (DrivingChara)
	{
		DrivingChara ->SetActorLocation(ShipWheel->GetComponentLocation() + FVector(-100, 0, 0)); // 使用方向向量更可靠);
	}
	
}

// Called to bind functionality to input
void AMyTestCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this,&AMyTestCar::MoveForward);
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Completed, this,&AMyTestCar::MoveForwardRelease);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AMyTestCar::MoveRight);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Completed, this, &AMyTestCar::MoveRightRelease);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyTestCar::Look);
		EnhancedInputComponent->BindAction(PressKeyEAction, ETriggerEvent::Started, this, &AMyTestCar::PressE);
	}
	
}

void AMyTestCar::MoveForward(const FInputActionValue& inputValue)
{
	float value = inputValue.GetMagnitude();
	isStopTraction = false;

	if(value > 0) //按下W
	{
		//首先判断是否是同方向，如果是的话判断是否超过最大速度，如果是的话就让油门刹车都为0，否则的话判断就让油门逐渐增加，不是同方向的话全速刹车
		if(value * WheelMovement->GetForwardSpeed() > 0) //同方向
		{
			if(getSpeed()>50.0f)
			{
				SetTraction(0.0f,0.0f);
				return;
			}
			if(PowerValue < 0.2f)
			{
				PowerValue = 0.2f;
			}
			PowerValue = FMath::Clamp(PowerValue + 0.01f, 0.2f, 1.0f);
			SetTraction(PowerValue,0.0f);
		}
		else
		{
			SetTraction(1.0f,0.0f);
		}
	}
	else if(value < 0)
	{
		if(value * WheelMovement->GetForwardSpeed() > 0) //同方向
		{
			if(getSpeed()>20.0f)
			{
				SetTraction(0.0f,0.0f);
				return;
			}
			if(PowerValue > -0.2f)
			{
				PowerValue = -0.2f;
			}
			PowerValue = FMath::Clamp(PowerValue - 0.02f, -1.0f, -0.2f);
			SetTraction(0.0f,-1 * PowerValue);
		}
		else
		{
			SetTraction(0.0f,1.0f);
		}
	}
}

void AMyTestCar::MoveForwardRelease()
{
	isStopTraction = true;

	if(WheelMovement->GetForwardSpeed() > 0)
	{
		SetTraction(0.0f,0.3f);

	}
	else
	{
		SetTraction(0.7f,0.0f);
	}
		
	
}

void AMyTestCar::MoveRight(const FInputActionValue& inputValue)
{
	float value = inputValue.GetMagnitude();
	if(value>0)
	{
		steering += 0.003;
		if(steering>0.5){
			steering = 0.5;
		}
	}
	else
	{
		steering -= 0.003;
		if(steering<-0.5){
			steering = -0.5;
		}
	}
	
	WheelMovement->SetSteeringInput(steering);
	

}

void AMyTestCar::MoveRightRelease()
{
	//WheelMovement->SetSteeringInput(0.0f);
}

void AMyTestCar::Look(const FInputActionValue& inputValue)
{
	FVector2D LookVector = inputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void AMyTestCar::DriveBoat(AEpicCharacter* chara)
{
	if (!bIsDriving)
	{
		APlayerController* PlayerController = Cast<APlayerController>(chara->GetController());
		DrivingChara = chara;
		bIsDriving = true;
		DrivingChara->SetActorHiddenInGame(true);
		SetActorTickEnabled(true);	//启用tick
		if (PlayerController)
		{
			// 先取消角色的控制权
			DrivingChara->UnPossessed();
			// 让玩家控制器控制船只
			PlayerController->Possess(this);
		}
	}
}


void AMyTestCar::PressE(const FInputActionValue& inputValue)
{
	if (!DrivingChara)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(this->GetController()); //此时已经是船了
	DrivingChara->SetActorHiddenInGame(false);
	PlayerController->Possess(DrivingChara);
	//this->UnPossessed();
	
	//DrivingChara = nullptr;
	bIsDriving = false;
	DrivingChara->SetIsDriving(false);
}


float AMyTestCar::getSpeed()
{
	FVector Velocity  = GetMesh()->GetComponentVelocity();
	float CurrentSpeed = Velocity.Size() * 0.036f;
	return CurrentSpeed;
}

void AMyTestCar::SetTraction(float powerValue, float brakeValue)
{
	WheelMovement->SetThrottleInput(powerValue);
	WheelMovement->SetBrakeInput(brakeValue);
}


void AMyTestCar::WheelRolling(float DeltaTime)
{
	if (!ShipWheel) return; // 确保ShipWheel有效
    
	// 每帧旋转1度（DeltaTime确保帧率无关）
	float RotationRate = 10.0f; // 度/秒
	CurrentRotationAngle += RotationRate * DeltaTime;
    
	// 应用旋转（假设绕Z轴旋转）
	FRotator NewRotation = FRotator(CurrentRotationAngle, 0.0f, 90.0f);
	ShipWheel->SetRelativeRotation(NewRotation);
	
}

void AMyTestCar::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
								 const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(AEpicCharacter::StaticClass()))
	{
		ShipWheel->SetRenderCustomDepth(true);
		AEpicCharacter* chara = Cast<AEpicCharacter>(OtherActor);
		chara->CouldDrive(this,true);
	}
	
}

void AMyTestCar::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//如果是角色的话
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(AEpicCharacter::StaticClass()))
	{
		ShipWheel->SetRenderCustomDepth(false);
		AEpicCharacter* chara = Cast<AEpicCharacter>(OtherActor);
		chara->CouldDrive(this,false);

	}
}
