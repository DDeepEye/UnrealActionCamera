// Fill out your copyright notice in the Description page of Project Settings.


#include "Cameras/xtCameraBias.h"


UxtCameraBias::UxtCameraBias(const FObjectInitializer& ObjInit)
: LocationScale(1.0f)
, RotationScale(1.0f)
, FovScale(1.0f)
,_timer(0)
{

}

void UxtCameraBias::Start(FxtCameraBiasInfo& shakeInfo)
{
	_biasInfo = shakeInfo;
	_reset();
}

bool UxtCameraBias::IsActive()
{
	return _biasInfo.IsMaintain ? true : _timer < _biasInfo.Duration;
}

bool UxtCameraBias::IsFinished()
{
	return _biasInfo.IsMaintain ? false : _timer >= _biasInfo.Duration;
}

CameraEffectType UxtCameraBias::GetType()
{
	return CameraEffectType::Bias;
}

void UxtCameraBias::TickEffect(float DeltaTime, FVector& outLocation, FRotator& outRotator, float& outFov)
{
	if (IsFinished())
		return;

	float anitimer = (_timer += DeltaTime) / _biasInfo.Duration;

	anitimer = anitimer > 1.0f ? 1.0f : anitimer;

	outLocation -= _curLocation;
	outRotator -= _curRotator;
	outFov -= _curFov;

	_curLocation = _biasInfo.LocationBias;
	_curRotator = _biasInfo.RotationBias;
	_curFov = _biasInfo.FovBias;

	for (int32 i = 0; i < _biasInfo.Animations.Num(); ++i)
	{
		UCurveFloat* ani = _biasInfo.Animations[i]._animation;
		if (ani == nullptr)
			continue;

		_logValue = ani->GetFloatValue(anitimer);

		uint8 flags = _biasInfo.Animations[i].flags;

		_curLocation.X *= (_isFlag(flags, (uint8)AnimationBitFlag::X) ? _logValue : 1.0f);
		_curLocation.Y *= (_isFlag(flags, (uint8)AnimationBitFlag::Y) ? _logValue : 1.0f);
		_curLocation.Z *= (_isFlag(flags, (uint8)AnimationBitFlag::Z) ? _logValue : 1.0f);


		_curRotator.Pitch *= (_isFlag(flags, (uint8)AnimationBitFlag::Pitch) ? _logValue : 1.0f);
		_curRotator.Yaw *= (_isFlag(flags, (uint8)AnimationBitFlag::Yaw) ? _logValue : 1.0f);
		_curRotator.Roll *= (_isFlag(flags, (uint8)AnimationBitFlag::Roll) ? _logValue : 1.0f);

		_curFov *= (_isFlag(flags, (uint8)AnimationBitFlag::Fov) ? _logValue : 1.0f);
	}

	_curLocation *= LocationScale;
	_curRotator *= RotationScale;
	_curFov *= FovScale;

	outLocation += _curLocation;
	outRotator += _curRotator;
	outFov += _curFov;
}

void UxtCameraBias::CleanUp(FVector& outLocation, FRotator& outRotator, float& outFov)
{
	outLocation -= _curLocation;
	outRotator -= _curRotator;
	outFov -= _curFov;

	_reset();
}


bool UxtCameraBias::IsArbitraryTermination(uint64 actorUID, uint64 animUID, FVector& outLocation, FRotator& outRotator, float& outFov, float& outLogValue)
{
	if (_biasInfo.IsMaintain && _biasInfo.ActorUID == actorUID && _biasInfo.AnimUID == animUID)
	{
		if(_biasInfo.IsUseCharge) outLogValue = _logValue;
		CleanUp(outLocation, outRotator, outFov);
		return true;
	}
	return false; 
}
bool UxtCameraBias::IsArbitraryTermination(uint64 actorUID, FVector& outLocation, FRotator& outRotator, float& outFov)
{ 
	if (_biasInfo.IsMaintain && _biasInfo.ActorUID == actorUID)
	{
		CleanUp(outLocation, outRotator, outFov);
		return true;
	}
	return false; 
}

void UxtCameraBias::_reset()
{
	_curLocation.X = 0;
	_curLocation.Y = 0;
	_curLocation.Z = 0;

	_curRotator.Pitch = 0;
	_curRotator.Yaw = 0;
	_curRotator.Roll = 0;

	_curFov = 0;

	_timer = 0;
	_logValue = 0;
}
