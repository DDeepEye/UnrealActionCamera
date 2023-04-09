// Fill out your copyright notice in the Description page of Project Settings.
#include "xtCameraShake.h"
#include "Main/World/xtGameModeBase.h"
#include "Cameras/xtBaseCamera.h"
#include "Curves/CurveFloat.h"
#include "xtCameraEffectManager.h"



FxtShakeInfo::FxtShakeInfo()
{
	Duration = 1.0f;
	BlendInTime = 0;
	BlendOutTime = 0;

	LocationAmplitudeMultiplier = 1;
	LocationFrequencyMultiplier = 1;
	X.Amplitude = 0;
	X.Frequency = 0;
	Y.Amplitude = 0;
	Y.Frequency = 0;
	Z.Amplitude = 0;
	Z.Frequency = 0;

	RotationAmplitudeMultiplier = 1;
	RotationFrequencyMultiplier = 1;
	Pitch.Amplitude = 0;
	Pitch.Frequency = 0;
	Yaw.Amplitude = 0;
	Yaw.Frequency = 0;
	Roll.Amplitude = 0;
	Roll.Frequency = 0;

	FOV.Amplitude = 0;
	FOV.Frequency = 0;
}

FxtShakeInfo::FxtShakeInfo(const UWaveOscillatorCameraShakePattern* pattern)
{
	operator=(pattern);
}

FxtShakeInfo FxtShakeInfo::operator = (const UWaveOscillatorCameraShakePattern* pattern)
{
	Duration = pattern->Duration;
	BlendInTime = pattern->BlendInTime;
	BlendOutTime = pattern->BlendOutTime;
	LocationAmplitudeMultiplier = pattern->LocationAmplitudeMultiplier;
	LocationFrequencyMultiplier = pattern->LocationFrequencyMultiplier;
	X = pattern->X;
	Y = pattern->Y;
	Z = pattern->Z;
	RotationAmplitudeMultiplier = pattern->RotationAmplitudeMultiplier;
	RotationFrequencyMultiplier = pattern->RotationFrequencyMultiplier;
	Pitch = pattern->Pitch;
	Yaw = pattern->Yaw;
	Roll = pattern->Roll;
	FOV = pattern->FOV;

	return *this;
}

void FxtShakeInfo::RestorePattern(UWaveOscillatorCameraShakePattern* pattern)
{
	pattern->Duration = Duration;
	pattern->BlendInTime = BlendInTime;
	pattern->BlendOutTime = BlendOutTime;
	pattern->LocationAmplitudeMultiplier = LocationAmplitudeMultiplier;
	pattern->LocationFrequencyMultiplier = LocationFrequencyMultiplier;
	pattern->X = X;
	pattern->Y = Y;
	pattern->Z = Z;
	pattern->RotationAmplitudeMultiplier = RotationAmplitudeMultiplier;
	pattern->RotationFrequencyMultiplier = RotationFrequencyMultiplier;
	pattern->Pitch = Pitch;
	pattern->Yaw = Yaw;
	pattern->Roll = Roll;
	pattern->FOV = FOV;
}


UxtCameraShake::UxtCameraShake(const FObjectInitializer& ObjInit)
: Super(ObjInit.SetDefaultSubobjectClass<UWaveOscillatorCameraShakePattern>(TEXT("RootShakePattern")))
, LocationScale(1.0f)
, RotationScale(1.0f)
, _pattern(nullptr)
,_animationTime(0.0f)
{
	bSingleInstance = true;
}

void UxtCameraShake::Start(FxtShakeInfo& shakeInfo)
{
	if (shakeInfo.Duration <= 0)
	{
		return;
	}
	
	if (!_pattern)
	{
		_pattern = Cast<UWaveOscillatorCameraShakePattern>(GetRootShakePattern());
	}
	shakeInfo.RestorePattern(_pattern);
	Animations = shakeInfo.Animations;
	Start(1, 1 ,1);
}

void UxtCameraShake::Start(float locationScale, float rotaionScale, float fovScale)
{
	UWorld* world = GetWorld();
	if (world == nullptr)
	{
		return;
	}

	AxtGameModeBase* gameModeBase = world->GetAuthGameMode< AxtGameModeBase >();
	if (gameModeBase == nullptr) {
		return;
	}

	if (!_pattern)
	{
		_pattern = Cast<UWaveOscillatorCameraShakePattern>(GetRootShakePattern());
		if (_pattern->Duration <= 0)
		{
			return;
		}
	}

	_reset();

	if(IsActive()) StopShake();
	
	LocationScale = locationScale;
	RotationScale = rotaionScale;
	FovScale = fovScale;

	StartShake(world->GetFirstPlayerController()->PlayerCameraManager, 1.0f, ECameraShakePlaySpace::CameraLocal);
}

bool UxtCameraShake::IsActive()
{
	return Super::IsActive();
}

bool UxtCameraShake::IsFinished()
{
	return Super::IsFinished();
}

CameraEffectType UxtCameraShake::GetType()
{
	return CameraEffectType::Shake;
}

void UxtCameraShake::TickEffect(float DeltaTime, FVector& outShakeLocation, FRotator& outShakeRotator, float& outFov)
{
	if (IsActive())
	{
		float anitimer = (_animationTime += DeltaTime) / _pattern->Duration;

		outShakeLocation -= _info.Location;
		outShakeRotator -= _info.Rotation;
		outFov -= _info.FOV;

		_info.Location.X = 0;
		_info.Location.Y = 0;
		_info.Location.Z = 0;

		_info.Rotation.Pitch = 0;
		_info.Rotation.Yaw = 0;
		_info.Rotation.Roll = 0;

		_info.FOV = 0;

		UpdateAndApplyCameraShake(DeltaTime, 1.0f, _info);

		for (int32 i = 0; i < Animations.Num(); ++i)
		{
			UCurveFloat* ani = Animations[i]._animation;
			if (ani == nullptr)
			{
				continue;
			}

			uint8 flags = Animations[i].flags;
			
			_info.Location.X *= (_isFlag(flags, (uint8)AnimationBitFlag::X) ? ani->GetFloatValue(anitimer) : 1.0f);
			_info.Location.Y *= (_isFlag(flags, (uint8)AnimationBitFlag::Y) ? ani->GetFloatValue(anitimer) : 1.0f);
			_info.Location.Z *= (_isFlag(flags, (uint8)AnimationBitFlag::Z) ? ani->GetFloatValue(anitimer) : 1.0f);


			_info.Rotation.Pitch *= (_isFlag(flags, (uint8)AnimationBitFlag::Pitch) ? ani->GetFloatValue(anitimer) : 1.0f);
			_info.Rotation.Yaw *= (_isFlag(flags, (uint8)AnimationBitFlag::Yaw) ? ani->GetFloatValue(anitimer) : 1.0f);
			_info.Rotation.Roll *= (_isFlag(flags, (uint8)AnimationBitFlag::Roll) ? ani->GetFloatValue(anitimer) : 1.0f);

			_info.FOV *= (_isFlag(flags, (uint8)AnimationBitFlag::Fov) ? ani->GetFloatValue(anitimer) : 1.0f);
		}
		
		_info.Location *= LocationScale;
		_info.Rotation *= RotationScale;

		outShakeLocation += _info.Location;
		outShakeRotator += _info.Rotation;
		outFov += _info.FOV;
	}

	if (IsFinished()) _reset();
}

void UxtCameraShake::CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov)
{
	outLocation -= _info.Location;
	outRotator -= _info.Rotation;
	outFov -= _info.FOV;
	_reset();
}

void UxtCameraShake::_reset()
{
	_animationTime = 0;

	_info.Location.X = 0;
	_info.Location.Y = 0;
	_info.Location.Z = 0;

	_info.Rotation.Pitch = 0;
	_info.Rotation.Yaw = 0;
	_info.Rotation.Roll = 0;

	_info.FOV = 0;
}
