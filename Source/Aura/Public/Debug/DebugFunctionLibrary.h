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

	/**
	 *	Predicts a position in the future at Current time + PredictionTime.
	 *	@param CurrentPosition Current position of the actor.
	 *	@param OutPosition Gets filled with the predicted position.
	 *	@param CurrentVelocity Velocity of the actor.
	 *	@param CurrentAcceleration Acceleration of the actor.
	 *	@param PredictionTime Time in the future we want to predict the position for. (Current time + PredictionTime)
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static void PredictActorPosition(const FVector& CurrentPosition, FVector& OutPosition, const FVector& CurrentVelocity, const FVector& CurrentAcceleration, const float PredictionTime);


	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float CalculateFineTuneValue(float DistanceToTarget, const UCurveFloat* ValueFromDistance);


	/**	Predicts the direction for the projectile that it needs in order to hit the target. It also returns the predicted location where the collision will happen.
	 *	@param WorldContextObject World Context Object.
	 *	@param GravityZ Gravity override of the projectile.
	 *	@param TargetPosition Current position of the target.
	 *	@param OutDirection Normalized direction vector that gets filled by the function.
	 *	@param TargetVelocity Current velocity of the target.
	 *	@param ProjectileStartLocation Location where the projectile is spawned from.
	 *	@param ProjectileSpeed Initial Speed of the projectile.
	 *	@param FineTuneValue Value that is used to fix the overshoot problem for this function. As distance to the target increases, the value needs to decrease accordingly.
	 *	@returns FVector that corresponds to the predicted target location.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static FVector PredictProjectileDirection(const UObject* WorldContextObject, const float GravityZ, const FVector& TargetPosition, FVector& OutDirection,
	const FVector& TargetVelocity, const FVector& ProjectileStartLocation, const float ProjectileSpeed, float FineTuneValue = 0.75f);

	
	// NOTE Ballistic functions too advanced for me to understand why they are not working properly.
	// NOTE Would need maybe few days to understand and fix the problems, not currently worth my time since current version works well enough and is much faster to execute.
	
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static int SolveBallisticArc(const FVector& ProjectilePosition, const float ProjectileSpeed, const FVector& TargetPosition, const FVector& TargetVelocity, const float GravityZ, FVector& FiringSolution, FVector& SecondSolution);

	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static bool SolveBallisticArcLateral(const FVector& ProjectilePosition, float LateralSpeed, const FVector& TargetPosition,
	                              float MaxHeight, FVector& FireVelocity, float& Gravity);
	
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static bool SolveBallisticArcLateral_Plus(const FVector& ProjectilePosition, float LateralSpeed, const FVector& Target,
	                              const FVector& TargetVelocity, float MaxHeightOffset, FVector& FireVelocity,
	                              float& Gravity, FVector& ImpactPoint);
};
