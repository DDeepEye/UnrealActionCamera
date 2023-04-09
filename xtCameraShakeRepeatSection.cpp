// Fill out your copyright notice in the Description page of Project Settings.


#include "Cameras/xtCameraShakeRepeatSection.h"
#include "Main/World/xtGameModeBase.h"
#include "Cameras/xtBaseCamera.h"
#include "Curves/CurveFloat.h"
#include "xtCameraEffectManager.h"


FxtShakeRepeatSectionInfo::FxtShakeRepeatSectionInfo()
{
	Duration = 1.0f;
	RepeatSectionRate = 0;
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

	ActorUID = 0;
	AnimUID = 0;
}

FxtShakeRepeatSectionInfo::FxtShakeRepeatSectionInfo(const UWaveOscillatorCameraShakePattern* pattern)
{
	operator=(pattern);
}

FxtShakeRepeatSectionInfo FxtShakeRepeatSectionInfo::operator = (const UWaveOscillatorCameraShakePattern* pattern)
{
	Duration = pattern->Duration;
	
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

void FxtShakeRepeatSectionInfo::RestorePattern(UWaveOscillatorCameraShakePattern* pattern)
{
	pattern->Duration = Duration;
	pattern->BlendInTime = 0;
	pattern->BlendOutTime = 0;
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

UxtCameraShakeRepeatSection::UxtCameraShakeRepeatSection(const FObjectInitializer& ObjInit)
: Super(ObjInit.SetDefaultSubobjectClass<UWaveOscillatorCameraShakePattern>(TEXT("RootShakePattern")))
, _animationTime(0)
, _repeatTime(0)
, _actorUID(0)
, _animUID(0)
, _logValue(0)
, _onceFull(false)
, _isUseCharge(false)
{

}

void UxtCameraShakeRepeatSection::Start(FxtShakeRepeatSectionInfo& shakeInfo)
{
	if (shakeInfo.Duration <= 0)
		return;

	if (!_pattern) _pattern = Cast<UWaveOscillatorCameraShakePattern>(GetRootShakePattern());

	_reset();

	shakeInfo.RestorePattern(_pattern);
	_repeatTime = shakeInfo.Duration * shakeInfo.RepeatSectionRate;
	_actorUID = shakeInfo.ActorUID;
	_animUID = shakeInfo.AnimUID;
	_isUseCharge = shakeInfo.IsUseCharge;
	Animation = shakeInfo.Animation;
	Start();
}

void UxtCameraShakeRepeatSection::Start()
{
	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	AxtGameModeBase* gameModeBase = world->GetAuthGameMode< AxtGameModeBase >();
	if (gameModeBase == nullptr)
		return;

	if (!_pattern)
	{
		_pattern = Cast<UWaveOscillatorCameraShakePattern>(GetRootShakePattern());
		if (_pattern->Duration <= 0)
			return;
	}

	_info.FOV = 0;
	_animationTime = 0;

	StartShake(world->GetFirstPlayerController()->PlayerCameraManager, 1.0f, ECameraShakePlaySpace::CameraLocal);
}

void UxtCameraShakeRepeatSection::_reset()
{
	if (_repeatTime > 0)
	{
		FMinimalViewInfo info;
		UpdateAndApplyCameraShake(_pattern->Duration, 1.0f, info);
	}

	_animationTime = 0;

	_info.Location.X = 0;
	_info.Location.Y = 0;
	_info.Location.Z = 0;

	_info.Rotation.Pitch = 0;
	_info.Rotation.Yaw = 0;
	_info.Rotation.Roll = 0;

	_info.FOV = 0;

	_actorUID = 0;
	_animUID = 0;
	_logValue = 0;
	_repeatTime = 0;

	_onceFull = false;
}

bool UxtCameraShakeRepeatSection::IsActive()
{
	return _repeatTime > 0;
}

bool UxtCameraShakeRepeatSection::IsFinished()
{
	return !(_repeatTime > 0);
}

CameraEffectType UxtCameraShakeRepeatSection::GetType()
{
	return CameraEffectType::RepeatPointShake;
}

void UxtCameraShakeRepeatSection::TickEffect(float DeltaTime, FVector& outShakeLocation, FRotator& outShakeRotator, float& outFov)
{
	if (IsActive())
	{
		if ((_animationTime += DeltaTime) > _pattern->Duration)
		{
			_animationTime = _repeatTime;
			_onceFull = true;
		}

		float anitimer = _animationTime / _pattern->Duration;
		anitimer = anitimer > 1.0f ? 1.0f : anitimer;

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

		ScrubAndApplyCameraShake(_animationTime, 1.0f, _info);

		UCurveFloat* ani = Animation._animation;
		if (ani)
		{
			_logValue = ani->GetFloatValue(anitimer);

			uint8 flags = Animation.flags;

			_info.Location.X *= (_isFlag(flags, (uint8)AnimationBitFlag::X) ? _logValue : 1.0f);
			_info.Location.Y *= (_isFlag(flags, (uint8)AnimationBitFlag::Y) ? _logValue : 1.0f);
			_info.Location.Z *= (_isFlag(flags, (uint8)AnimationBitFlag::Z) ? _logValue : 1.0f);

			_info.Rotation.Pitch *= (_isFlag(flags, (uint8)AnimationBitFlag::Pitch) ? _logValue : 1.0f);
			_info.Rotation.Yaw *= (_isFlag(flags, (uint8)AnimationBitFlag::Yaw) ? _logValue : 1.0f);
			_info.Rotation.Roll *= (_isFlag(flags, (uint8)AnimationBitFlag::Roll) ? _logValue : 1.0f);

			_info.FOV *= (_isFlag(flags, (uint8)AnimationBitFlag::Fov) ? _logValue : 1.0f);
		}

		outShakeLocation += _info.Location;
		outShakeRotator += _info.Rotation;
		outFov += _info.FOV;
	}
}

void UxtCameraShakeRepeatSection::CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov)
{
	outLocation -= _info.Location;
	outRotator -= _info.Rotation;
	outFov -= _info.FOV;
	_reset();
}

bool UxtCameraShakeRepeatSection::IsArbitraryTermination(uint64 actorUID, uint64 animUID, FVector& outLocation, FRotator& outRotator, float& outFov, float& outLogValue)
{
	if (_actorUID == actorUID && _animUID == animUID)
	{
		if(_isUseCharge) outLogValue = _onceFull ? 1.0f : _logValue;
		CleanUp(outLocation, outRotator, outFov);
		return true;
	}
	return false;
}

bool UxtCameraShakeRepeatSection::IsArbitraryTermination(uint64 actorUID, FVector& outLocation, FRotator& outRotator, float& outFov)
{
	if (_actorUID == actorUID)
	{
		CleanUp(outLocation, outRotator, outFov);
		return true;
	}
	return false;
}

