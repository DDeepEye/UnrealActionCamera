// Fill out your copyright notice in the Description page of Project Settings.


#include "xtBaseCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "NexterCommon/Logger/xtLogger.h"
#include "Actors/Character/xtCharacter.h"
#include "UI/xtInputManager.h"
#include "xtCameraFunctionalParts.h"
#include "Camera/CameraShakeBase.h"
#include "PerlinNoiseCameraShakePattern.h"
#include "GameFramework/PlayerController.h"
#include "xtCameraEffectManager.h"

// Sets default values
AxtBaseCamera::AxtBaseCamera()
	:IsTrace(true)
	,_prevBias(0,0,0)
	,IsFovEdit(false)
	,MinDist(0), MaxDist(500), AddMoveRate(7)
	,MinFov(15.f), MaxFov(180.f)
	,MaxBotPitch(-65.f), MaxTopPitch(65.f), PitchRate(3), PitchInverse(false), YawRate(3), YawInverse(false)
	,MovingSpeed(550), UpDownMorphRate(0.2f), LimitChaseTailDist(180), OverlapAlpha(0.3f)
	,ChaseTarget(nullptr), MoveMethod(nullptr), LookUpMethod(nullptr)
	,PrevLocation(0,0,0)	
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Center = CreateDefaultSubobject<USceneComponent>(TEXT("Center"));
	Center->SetupAttachment(RootComponent);

	CameraRod = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraRod"));
	CameraRod.Get()->SetupAttachment(Center);
	CameraRod.Get()->TargetArmLength = 350.0f; // The camera follows at this distance behind the character		
	CameraRod.Get()->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera.Get()->SetupAttachment(CameraRod, USpringArmComponent::SocketName);
	Camera.Get()->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	OverlapSpace = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSpace"));
	OverlapSpace->SetupAttachment(Camera.Get());
}

// Called when the game starts or when spawned
void AxtBaseCamera::BeginPlay()
{
	Super::BeginPlay();

	if (LimitChaseTailDist <= SMALL_NUMBER) LimitChaseTailDist = SMALL_NUMBER;

	if (_moveMethodClass != nullptr)
	{
		MoveMethod = NewObject<UxtCameraFunctionalParts>(this, _moveMethodClass);
		MoveMethod->SetTarget(this);
	}

	if (_lookUpMethodClass != nullptr)
	{
		LookUpMethod = NewObject<UxtCameraFunctionalParts>(this, _lookUpMethodClass);
		LookUpMethod->SetTarget(this);
	}
}

// Called every frame
void AxtBaseCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MoveMethod)		MoveMethod.Get()->Activate(DeltaTime);
	if (LookUpMethod)	LookUpMethod.Get()->Activate(DeltaTime);

	if (Shake != nullptr)
	{
		if (Shake->IsActive())
		{
			FMinimalViewInfo info;
			Shake->UpdateAndApplyCameraShake(DeltaTime, 120.0f, info);
			SetBias(FVector2D(info.Location));
		}
	}
}

void AxtBaseCamera::Prepare()
{
	BIND_INPUT_AXIS(ExtAxisType::Turn, &AxtBaseCamera::OnInputYaw);
	BIND_INPUT_AXIS(ExtAxisType::Lookup, &AxtBaseCamera::OnInputPitch);
	///BIND_INPUT_AXIS(ExtAxisType::MouseWheel, &AxtBaseCamera::Enlarge);
}
void AxtBaseCamera::Destroy()
{
	UNBIND_INPUT_AXIS(ExtAxisType::Turn, &AxtBaseCamera::OnInputYaw);
	UNBIND_INPUT_AXIS(ExtAxisType::Lookup, &AxtBaseCamera::OnInputPitch);
	///UNBIND_INPUT_AXIS(ExtAxisType::MouseWheel, &AxtBaseCamera::Enlarge);
}

void AxtBaseCamera::SetFov(float fov)
{
	fov = fov < MinFov ? MinFov : fov;
	Camera.Get()->FieldOfView = fov > MaxFov ? MaxFov : fov;
}

float AxtBaseCamera::GetFov()
{
	return Camera.Get()->FieldOfView;
}

float AxtBaseCamera::GetDistance()
{
	return CameraRod.Get()->TargetArmLength;
}

void AxtBaseCamera::SetDistance(float dist)
{
	dist = dist < MinDist ? MinDist : dist;
	dist = dist > MaxDist ? MaxDist : dist;
	CameraRod.Get()->TargetArmLength = dist;
}

float AxtBaseCamera::AddDistance(float value)
{
	float armLength = CameraRod.Get()->TargetArmLength + (value * AddMoveRate);
	armLength = armLength < MinDist ? MinDist : armLength;
	armLength = armLength > MaxDist ? MaxDist : armLength;
	CameraRod.Get()->TargetArmLength = armLength;
	OverlapSpace.Get()->UpdateOverlaps();
	return armLength;
}

void AxtBaseCamera::SetTraceTarget(class USceneComponent* chaseTarget)
{
	if (!chaseTarget)
	{
		ChaseTarget = nullptr;
		return;
	}

	ChaseTarget = chaseTarget;
	SetActorTransform(FTransform(ChaseTarget.Get()->GetComponentRotation(), ChaseTarget.Get()->GetComponentLocation()));
	PrevLocation = GetActorLocation();
}

void AxtBaseCamera::LookUp(class USceneComponent* lookUp)
{
	LookUpTarget = lookUp;
}

void AxtBaseCamera::SetIsTrace(bool isTrace)
{
	IsTrace = isTrace;
}

void AxtBaseCamera::Enlarge(float value)
{
	if (FMath::Abs(value) <= SMALL_NUMBER)
		return;

	if (IsFovEdit) SetFov(GetFov() + value);
	else AddDistance(value);
}


void AxtBaseCamera::OnInputPitch(float value)
{
	if (FMath::Abs(value) <= SMALL_NUMBER)
		return;

	FRotator rot = GetActorRotation() + FRotator(value * (PitchInverse ? -PitchRate: PitchRate), 0.f, 0.f);
	rot.Pitch = rot.Pitch < MaxBotPitch ? MaxBotPitch : rot.Pitch;
	rot.Pitch = rot.Pitch > MaxTopPitch ? MaxTopPitch : rot.Pitch;
	rot.Roll = 0;
	SetActorRotation(rot);
}

void AxtBaseCamera::OnInputYaw(float value)
{
	if (FMath::Abs(value) <= SMALL_NUMBER)
		return;

	FRotator rot = GetActorRotation() + FRotator(0.0f, value * (YawInverse ? -YawRate : YawRate), 0.f);
	rot.Roll = 0;
	SetActorRotation(rot);
}

void AxtBaseCamera::NotifyActorBeginOverlap(AActor* otherActor)
{
	AxtCharacter* actor = Cast<AxtCharacter>(otherActor);
	if (actor != nullptr) actor->SetAlpha(OverlapAlpha);
}

void AxtBaseCamera::NotifyActorEndOverlap(AActor* otherActor)
{
	AxtCharacter* actor = Cast<AxtCharacter>(otherActor);
	if (actor != nullptr) actor->SetAlpha(0);
}

void AxtBaseCamera::SetBias(FVector2D localBias)
{
	SetBias(FVector(0, localBias.X, localBias.Y));
}

void AxtBaseCamera::SetBias(FVector localBias)
{
	FVector bias = CameraRod->GetRelativeLocation() + localBias - _prevBias;
	CameraRod->SetRelativeLocation(bias);
	_prevBias = localBias;
}


