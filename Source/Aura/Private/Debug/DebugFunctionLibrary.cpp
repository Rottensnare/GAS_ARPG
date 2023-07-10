// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/DebugFunctionLibrary.h"

float UDebugFunctionLibrary::DebugWaveFunction(const float Time, const float Amplitude, const float Frequency, const float Phase, const float Offset)
{
	const float Angle = 2 * PI * Frequency * Time + Phase;
	float WaveValue = FMath::Sin(Angle);
	WaveValue = WaveValue * Amplitude + Offset;
	return WaveValue;
}

//NOTE: Maybe use memoization for some object if this is actually ever used.
float UDebugFunctionLibrary::DebugWaveFunction_TimeReset(float& Time, const float Amplitude, const float Frequency,
	const float Phase, const float Offset)
{
	constexpr float WavePeriod = 2 * PI;
	const float Angle = WavePeriod * Frequency * Time + Phase;
	float WaveValue = FMath::Sin(Angle);
	WaveValue = WaveValue * Amplitude + Offset;
	if(Time > Frequency)
	{
		Time -= Frequency;
	}
	return WaveValue;
}
