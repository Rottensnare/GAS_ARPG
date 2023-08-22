// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugFunctionLibrary.generated.h"

class ACombatManager;
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
	 *	@param Time			The input value that controls the position along the wave. It can be a value that increments over time.
	 *						Will be reset to 0.f without affecting the output.
	 *	@param Amplitude	The maximum value of the wave. It determines how high or low the wave peaks.
	 *	@param Frequency	The frequency of the wave. It controls how many peaks and troughs occur over a given time period.
	 *	@param Phase		The phase offset of the wave. It shifts the starting position of the wave.
	 *	@param Offset		The vertical offset of the wave. It adds a constant value to the entire wave.
	 *	@returns			a floating-point value that represents the calculated wave value at a given time point.
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
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static void PredictActorPosition(const FVector& CurrentPosition, FVector& OutPosition, const FVector& CurrentVelocity, const FVector& CurrentAcceleration, const float PredictionTime);


	/**	Uses a float curve to get a correct fine tune value based on the distance to target.
	 *	@param DistanceToTarget Distance from projectile spawn location to the target location.
	 *	@param ValueFromDistance Float curve that uses normalized distance [0.f, 1.f] and returns a value [1.f, 0.25f]
	 *	@returns Fine tune value between [1.f, 0.25f]
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static float CalculateFineTuneValue(float DistanceToTarget, const UCurveFloat* ValueFromDistance);


	/**	Predicts the direction for the projectile that it needs in order to hit the target. It also returns the predicted location where the collision will happen.
	 *	@param TargetPosition Current position of the target.
	 *	@param OutDirection Normalized direction vector that gets filled by the function.
	 *	@param TargetVelocity Current velocity of the target.
	 *	@param ProjectileStartLocation Location where the projectile is spawned from.
	 *	@param ProjectileSpeed Initial Speed of the projectile.
	 *	@param FineTuneValue Value that is used to fix the overshoot problem for this function. As distance to the target increases, the value needs to decrease accordingly.
	 *	@returns FVector that corresponds to the predicted target location.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static FVector PredictProjectileInterceptionPoint(const FVector& TargetPosition, FVector& OutDirection,
	const FVector& TargetVelocity, const FVector& ProjectileStartLocation, const float ProjectileSpeed, const float FineTuneValue = 0.75f);


	/**	Predicts the intersection point for the projectile that it needs in order to hit the target that is moving in a circle.
	 *	@param TargetPosition Current position of the target.
	 *	@param CircleCenter The center of the circle that the target is moving around.
	 *	@param CircleRadius The radius of the circle the target is moving around.
	 *	@param bClockwise Movement direction around the circle.
	 *	@param TargetSpeed The speed at which the target is moving around the circle.
	 *	@param ProjectileStartLocation Location where the projectile is spawned from.
	 *	@param ProjectileSpeed Initial Speed of the projectile.
	 *	@param FineTuneValue Value that is used to fix the overshoot problem for this function. As distance to the target increases, the value needs to decrease accordingly.
	 *	@returns FVector that corresponds to the predicted target location.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static FVector PredictProjectileInterceptionPoint_Circle(const FVector& TargetPosition, const FVector& CircleCenter, const float CircleRadius, const bool bClockwise,
	const float TargetSpeed, const FVector& ProjectileStartLocation, const float ProjectileSpeed, const float FineTuneValue = 0.75f );



	
	// NOTE Ballistic functions too advanced for me to understand why they are not working properly.
	// NOTE Would need maybe few days to understand and fix the problems, not currently worth my time since current version works well enough and is much faster to execute.
	
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static int SolveBallisticArc(const FVector& ProjectilePosition, const float ProjectileSpeed, const FVector& TargetPosition, const FVector& TargetVelocity, const float GravityZ, FVector& FiringSolution, FVector& SecondSolution);

	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static bool SolveBallisticArcLateral(const FVector& ProjectilePosition, float LateralSpeed, const FVector& TargetPosition,
	                              float MaxHeight, FVector& FireVelocity, float& Gravity);
	
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Ballistics")
	static bool SolveBallisticArcLateral_Plus(const FVector& ProjectilePosition, float LateralSpeed, const FVector& Target,
	                              const FVector& TargetVelocity, float MaxHeightOffset, FVector& FireVelocity,
	                              float& Gravity, FVector& ImpactPoint);

	/**	Get standard deviation for float data.
	 *
	 *	@param Data		Array containing float data.
	 *	@returns		Standard deviation as a float.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float GetStandardDeviation(const TArray<float>& Data);

	/**	Gets a dot product value for determining if the actor is turning right or left. Positive means right/clockwise
	 *
	 *	@param CurrentForwardVector		Forward vector of the actor for the current frame.
	 *	@param PreviousForwardVector	Forward vector of the actor for the previous frame.
	 *	@returns						Dot product value for determining if actor is turning right or left.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float GetLeanDotProduct(const FVector& CurrentForwardVector, const FVector& PreviousForwardVector);

	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static FVector GetVectorIntersectionPoint(const FVector& V1StartPoint, const FVector& V1EndPoint, const FVector& V2StartPoint, const FVector& V2EndPoint);

	/**	Calculates the center and radius of a circle using a point on the circle arc, two tangents, and the length of the arc.
	 *
	 *	@param CirclePoint1		A point on the circle arc.
	 *	@param Tangent1			The tangent vector at CirclePoint1 for the circle.
	 *	@param Tangent2			A second tangent vector at any position.
	 *	@param ArcLength		The length of the circle arc between the points that Tangent1 and Tangent2 correspond to.
	 *	@param CircleCenter		[Out] The calculated center of the circle.
	 *	@returns				The radius of the circle.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static float CalculateCircleRadiusAndCenter(const FVector& CirclePoint1, const FVector& Tangent1, const FVector& Tangent2, const float ArcLength, FVector& CircleCenter);

	/**	Calculates the future position on a circle arc based on simulation time and speed.
	 *
	 *	@param CurrentLocation  The current location on the circle arc.
	 *	@param CircleCenter		The center of the circle.
	 *	@param CircleRadius		The radius of the circle.
	 *	@param Speed			The speed at which the object is moving along the circle arc.
	 *	@param PredictionTime   The duration in seconds for which the future position is predicted.
	 *	@param bClockwise		Determines whether the movement is clockwise (true) or counterclockwise (false).
	 *	@returns				The predicted future position on the circle arc.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static FVector CalculatePositionOnCircleArc(const FVector& CurrentLocation, const FVector& CircleCenter, const float CircleRadius, const float Speed, const float PredictionTime, const bool bClockwise);

	/**	Calculates the future position on a circle arc based on simulation time and speed.
	 *
	 *	@param CurrentLocation  The current location on the circle arc.
	 *	@param CircleCenter		The center of the circle.
	 *	@param Speed			The speed at which the object is moving along the circle arc.
	 *	@param PredictionTime   The duration in seconds for which the future position is predicted.
	 *	@param bClockwise		Determines whether the movement is clockwise (true) or counterclockwise (false).
	 *	@returns				The predicted future position on the circle arc.
	 */
	UFUNCTION(BlueprintPure, Category = "DebugFunctionLibrary|Math")
	static FVector CalculatePositionOnCircleArc_Ver2(const FVector& CurrentLocation, const FVector& CircleCenter, const float Speed, const float PredictionTime, const bool bClockwise);

	/**	Calls UKismetSystemLibrary::DrawDebugBox with predefined parameters 
	 *	@param WorldContextObject World Context Object
	 *	@param Location	Location for the center of the box.
	 */
	UFUNCTION(BlueprintCallable, Category = "DebugFunctionLibrary|Debug")
	static void DebugBoxSimple_Red(const UObject* WorldContextObject, const FVector& Location);

	UFUNCTION(BlueprintCallable, Category = "DebugFunctionLibrary|Combat")
	static ACombatManager* GetCombatManager(const UObject* WorldContextObject);

	// NOTE: For whatever reason, I couldn't find e or golden ration in Blueprints even though they are valuable constants.
	
	/** Returns the value of e, as in Euler number */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get e", CompactNodeTitle = "e"), Category="DebugFunctionLibrary|Math")
	static float Get_e();
	
	/** Returns the value of the golden ration */
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Golden Ratio", CompactNodeTitle = "Golden Ratio"), Category="DebugFunctionLibrary|Math")
	static float Get_Golden_Ration();

	//TODO: Work in Progress
	static TMap<AActor*, int32> Dbscan(const TArray<AActor*> Actors, const float Eps, const int32 MinPoints);
	static TArray<AActor*> FindNeighbours(const AActor* Actor, const TArray<AActor*>& AllActors, const float Eps);

	//Category: Templates
	template<typename ElementType>
	static void ShuffleArray(TArray<ElementType>& ArrayToShuffle); //UKismetArrayLibrary::Array_Shuffle only seems to work for int32 type
};
//TODO: Find a way to get this to work in blueprints
template <typename ElementType>
void UDebugFunctionLibrary::ShuffleArray(TArray<ElementType>& ArrayToShuffle)
{
	const int32 NumShuffles = ArrayToShuffle.Num() - 1;
	for(int32 i = 0; i < NumShuffles; ++i)
	{
		int32 SwapIndex = FMath::RandRange(i, NumShuffles);
		ArrayToShuffle.Swap(i, SwapIndex);
	}
}
