// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDebugFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 *	Calculate a wave value based on the provided parameters.
	 *
	 *	@param Time The input value that controls the position along the wave. It can be a value that increments over time.
	 *	@param Amplitude The maximum value of the wave. It determines how high or low the wave peaks.
	 *	@param Frequency The frequency of the wave. It controls how many peaks and troughs occur over a given time period.
	 *	@param Phase The phase offset of the wave. It shifts the starting position of the wave.
	 *	@param Offset The vertical offset of the wave. It adds a constant value to the entire wave.
	 *	@returns a floating-point value that represents the calculated wave value at a given time point.
	 *
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float DebugWaveFunction(const float Time, const float Amplitude, const float Frequency, const float Phase, const float Offset);

	/**
	 *	Same as DebugWaveFunction but will subtract frequency from time when time > frequency
	 *	NOTE: Not subtracting 1/Frequency because division is expensive *relatively speaking*
	 *	
	 *	@param Time The input value that controls the position along the wave. It can be a value that increments over time.
	 *	Will be reset to 0.f without affecting the output.
	 *	@param Amplitude The maximum value of the wave. It determines how high or low the wave peaks.
	 *	@param Frequency The frequency of the wave. It controls how many peaks and troughs occur over a given time period.
	 *	@param Phase The phase offset of the wave. It shifts the starting position of the wave.
	 *	@param Offset The vertical offset of the wave. It adds a constant value to the entire wave.
	 *	@returns a floating-point value that represents the calculated wave value at a given time point.
	 *	
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float DebugWaveFunction_TimeReset(UPARAM(ref)float& Time, const float Amplitude, const float Frequency, const float Phase, const float Offset);
};
