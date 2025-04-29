// Fill out your copyright notice in the Description page of Project Settings.


#include "Boat.h"

#include "EpicCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Uobject/ConstructorHelpers.h"

// Sets default values
ABoat::ABoat()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticM(TEXT("StaticMesh'/Game/_project/Character/Boat/Ship/Meshes/SM_ShipMainBody_00.SM_ShipMainBody_00'"));
	mc = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticM"));
	mc->SetStaticMesh(StaticM.Object);
	//mc->SetWorldScale3D(FVector(0.15,0.15,0.15));
	mc->SetRelativeRotation(FRotator(0,-90,0));
	mc->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipWheelMesh(TEXT("StaticMesh'/Game/_project/Character/Boat/Ship/Meshes/SM_ShipWheel_001.SM_ShipWheel_001'"));
	ShipWheel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipWheel"));
	ShipWheel->SetStaticMesh(ShipWheelMesh.Object);
	ShipWheel->SetupAttachment(mc, TEXT("ShipWheelSocket"));
	//ShipWheel->SetRelativeLocation(FVector(0,-3210,1210));
	ShipWheel->SetRelativeRotation(FRotator(0,0,90));

	cameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraArm"));
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("camera"));
	cameraArm->SetupAttachment(ShipWheel);			//绑定层级
	cameraArm->bUsePawnControlRotation = false;			//使用Pawn控制器（鼠标）控制相机臂跟随旋转
	cameraArm->SetRelativeLocation(FVector(0, 0, 400));
	cameraArm->SetRelativeRotation(FRotator(0,90,0));
	cameraArm->TargetArmLength = 400;
	cameraArm->bDoCollisionTest = false;	//禁用相机本身的碰撞检测，防止由于相机碰撞导致意料之外的情况出现

	camera->SetupAttachment(cameraArm);
	camera->SetRelativeLocation(FVector(0, 60, 80));
	camera->bUsePawnControlRotation = true;	//是否只旋转相机，也就是视角原地变化
	
	boxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("boxCollision"));
	boxCollision->SetupAttachment(ShipWheel);
	boxCollision->InitBoxExtent(FVector(100,100,100));
	boxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 角色
	boxCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // 可见性测试
	boxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoat::BeginOverlapFunction);
	boxCollision->OnComponentEndOverlap.AddDynamic(this, &ABoat::EndOverlapFunction);
	
	this->SourceInit();
}

// Called when the game starts or when spawned
void ABoat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShipWheel)
	{
		WheelRolling(DeltaTime);
	}
}


void ABoat::SourceInit()
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
}

// Called to bind functionality to input
void ABoat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this,&ABoat::MoveForward);
		EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &ABoat::MoveRight);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABoat::Look);
		EnhancedInputComponent->BindAction(PressKeyEAction, ETriggerEvent::Started, this, &ABoat::PressE);
	}
	
}
void ABoat::MoveForward(const FInputActionValue& inputValue)
{
	float value = inputValue.GetMagnitude();
	if (Controller != nullptr)
	{
		// 使用船只自身的朝向作为前进方向
		const FVector ForwardDirection = GetActorRightVector();
		AddMovementInput(ForwardDirection, value);
	}
}


void ABoat::MoveRight(const FInputActionValue& inputValue)
{
	float value = inputValue.GetMagnitude();
	if (Controller != nullptr && FMath::Abs(value) > KINDA_SMALL_NUMBER)
	{
		// 旋转船只（基于输入值向左或向右旋转）
		float RotationInput = value * 10 * GetWorld()->GetDeltaSeconds();
		AddActorLocalRotation(FRotator(0.0f, RotationInput, 0.0f));
	}
}

void ABoat::Look(const FInputActionValue& inputValue)
{
	FVector2D LookVector = inputValue.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}



void ABoat::DriveBoat(AEpicCharacter* chara)
{
	if (!bIsDriving)
	{
		bIsDriving = true;
		DrivingChara = chara;
		APlayerController* PlayerController = Cast<APlayerController>(chara->GetController());
		if (PlayerController)
		{
			// 先取消角色的控制权
			chara->UnPossessed();
			// 让玩家控制器控制船只
			PlayerController->Possess(this);
		}
	}
}

void ABoat::PressE(const FInputActionValue& inputValue)
{
	if (!DrivingChara)
	{
		return;
	}
	APlayerController* PlayerController = Cast<APlayerController>(this->GetController()); //此时已经是船了
	
	// 回退控制权
	this->UnPossessed();
	PlayerController->Possess(DrivingChara);

	bIsDriving = false;
	DrivingChara = nullptr;
}


void ABoat::BeginOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(AEpicCharacter::StaticClass()))
	{
		FString String = FString::Printf(TEXT("%sEnter"), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, String);
		// AEpicCharacter* chara = Cast<AEpicCharacter>(OtherActor);
		// chara->CouldDrive(this,true);
	}
	
}

void ABoat::EndOverlapFunction(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//如果是角色的话
	if (OtherActor && (OtherActor != this) && OtherActor->IsA(AEpicCharacter::StaticClass()))
	{
		FString String = FString::Printf(TEXT("%sLeave"), *OtherActor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, String);
		// AEpicCharacter* chara = Cast<AEpicCharacter>(OtherActor);
		// chara->CouldDrive(this,false);

	}
}

void ABoat::WheelRolling(float DeltaTime)
{
	if (!ShipWheel) return; // 确保ShipWheel有效
    
	// 每帧旋转1度（DeltaTime确保帧率无关）
	float RotationRate = 10.0f; // 度/秒
	CurrentRotationAngle += RotationRate * DeltaTime;
    
	// 应用旋转（假设绕Z轴旋转）
	FRotator NewRotation = FRotator(CurrentRotationAngle, 0.0f, 90.0f);
	ShipWheel->SetRelativeRotation(NewRotation);
	
	FString String = FString::Printf(TEXT("roll"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, String);
}

