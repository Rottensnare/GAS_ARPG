// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/DebugFunctionLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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

void UDebugFunctionLibrary::PredictActorPosition(const FVector& CurrentPosition, FVector& OutPosition,
	const FVector& CurrentVelocity, const FVector& CurrentAcceleration, const float PredictionTime)
{
	OutPosition = CurrentPosition + CurrentVelocity * PredictionTime + 0.5f * CurrentAcceleration * FMath::Pow(PredictionTime, 2);
}

float UDebugFunctionLibrary::CalculateFineTuneValue(float DistanceToTarget, const UCurveFloat* ValueFromDistance)
{
	if(ValueFromDistance == nullptr) return 0.75f;
	//NOTE: MaxDistance is currently hardcoded to 2000.f. Probably want to change that based on the projectile/ability
	constexpr float MaxDistance = 2000.f;
	const float NormalizedDistance = FMath::Clamp(DistanceToTarget / MaxDistance, 0.f, 1.f);
	return ValueFromDistance->FloatCurve.Eval(NormalizedDistance);
}

FVector UDebugFunctionLibrary::PredictProjectileInterceptionPoint(const FVector& TargetPosition, FVector& OutDirection,
	const FVector& TargetVelocity, const FVector& ProjectileStartLocation, const float ProjectileSpeed, const float FineTuneValue)
{
	
	// NOTE: CURRENTLY SOMEWHAT WORKING SOLUTION. OVERSHOOTS THE TARGET BY A FEW PERCENT AND THE FURTHER AWAY THE TARGET IS, THE LARGER THE OVERSHOOT
	
	// NOTE: CalculateFineTuneValue to get a somewhat accurate value based on distance to target.
	// NOTE: Should be used in a blueprint and passing the return value as the FineTuneValue for this function
	
	// Calculate the direction vector from the projectile's start location to the target's current position
	const FVector Direction = TargetPosition - ProjectileStartLocation;

	// Calculate the distance between the target and the projectile's start location
	const float Distance = Direction.Size();

	// Calculate the time to reach the target
	const float TimeToReach = Distance / ProjectileSpeed;
	
	// Calculate the predicted future position of the target
	const FVector PredictedTargetPosition = TargetPosition + TargetVelocity * TimeToReach * FineTuneValue;

	// Calculate the direction vector for the projectile to intercept the target
	OutDirection = (PredictedTargetPosition - ProjectileStartLocation).GetSafeNormal();
	
	//UKismetSystemLibrary::DrawDebugBox(WorldContextObject, PredictedTargetPosition, FVector(10.f), FLinearColor::Red, FRotator::ZeroRotator, 2.f);

	return PredictedTargetPosition;
}

FVector UDebugFunctionLibrary::PredictProjectileInterceptionPoint_Circle(const FVector& TargetPosition,
	const FVector& CircleCenter, const float CircleRadius, const bool bClockwise, const float TargetSpeed, const FVector& ProjectileStartLocation,
	const float ProjectileSpeed, const float FineTuneValue)
{
	// NOTE: This is not accurate for some situations. Noticed that it is good enough when the radius of the circle is rather small.
	// NOTE:	If someone else is reading this, please leave a comment on the GitHub page if you know a better, more efficient, more accurate, 
	// NOTE:	or simpler way of achieving the same or even better results.
	
	// NOTE: Hardly any information on how to find the interception point when the target is moving in a circular fashion.
	// NOTE: Had to use my limited memory and knowledge on geometry/trigonometry and then invent how to do it myself.

	
	// Direction to enemy from the circle center the player is running around
	const FVector DirectionalVector = (ProjectileStartLocation - CircleCenter).GetSafeNormal();

	// Closest and farthest positions on the circle from the enemy. Will be used to calculate min and max travel time for the projectile
	const FVector ClosestPositionOnCircle = CircleCenter + DirectionalVector * CircleRadius;
	const FVector FarthestPositionOnCircle = CircleCenter - DirectionalVector * CircleRadius;

	// Calculating min and max projectile travel time
	const float MinDistanceToTarget = FVector::Distance(ProjectileStartLocation, ClosestPositionOnCircle);
	const float MinTimeToTarget = MinDistanceToTarget / ProjectileSpeed;
	const float MaxDistanceToTarget = FVector::Distance(ProjectileStartLocation, FarthestPositionOnCircle);
	const float MaxTimeToTarget = MaxDistanceToTarget / ProjectileSpeed;

	// Predicting target position for the min and max travel times
	const FVector TargetLocAtMinTime = CalculatePositionOnCircleArc(TargetPosition, CircleCenter, CircleRadius, TargetSpeed, MinTimeToTarget, bClockwise);
	const FVector TargetLocAtMaxTime = CalculatePositionOnCircleArc(TargetPosition, CircleCenter, CircleRadius, TargetSpeed, MaxTimeToTarget, bClockwise);

	// Calculating the angle between the 2 previous prediction points
	const float DotProduct = FVector::DotProduct((TargetLocAtMinTime - CircleCenter).GetSafeNormal(), (TargetLocAtMaxTime - CircleCenter).GetSafeNormal());
	const float Angle = FMath::Acos(DotProduct);

	// Interpolating between the 0.f and the Max and Min travel times difference. Using the angle as the alpha.
	// Multiplied by FineTuneValue for trying to account for simplifications in the algorithm.
	// Fine tune value gotten from a float curve that is based on distance. 
	const float InterpolatedTime = (FMath::Lerp(0.f, (MaxTimeToTarget - MinTimeToTarget), (Angle / (2*PI)))) * FineTuneValue;

	// Calculate the predicted location fort the interpolated time.
	const FVector InterpolatedLocation = CalculatePositionOnCircleArc(TargetPosition, CircleCenter, CircleRadius, TargetSpeed, InterpolatedTime+MinTimeToTarget, bClockwise);
	return InterpolatedLocation;
}


static float GetCubicRoot(float value)
{   
	if (value > 0.0) {
		return FMath::Pow(value, 1.0 / 3.0);
	} else if (value < 0) {
		return -FMath::Pow(-value, 1.0 / 3.0);
	} else {
		return 0.0;
	}
}

static int SolveQuadric(float c0, float c1, float c2, float& s0, float& s1) {
	s0 = 0.f;
	s1 = 0.f;

	float p, q, D;

	/* normal form: x^2 + px + q = 0 */
	p = c1 / (2 * c0);
	q = c2 / c0;

	D = p * p - q;

	if (FMath::IsNearlyZero(D)) {
		s0 = -p;
		return 1;
	}
	else if (D < 0) {
		return 0;
	}
	else /* if (D > 0) */ {
		float sqrt_D = FMath::Sqrt(D);

		s0 =   sqrt_D - p;
		s1 = -sqrt_D - p;
		return 2;
	}
}

static int SolveCubic(float c0, float c1, float c2, float c3, float& s0, float& s1, float& s2)
{
	s0 = 0.f;
	s1 = 0.f;
	s2 = 0.f;

	int     num;
	float  sub;
	float  A, B, C;
	float  sq_A, p, q;
	float  cb_p, D;

	/* normal form: x^3 + Ax^2 + Bx + C = 0 */
	A = c1 / c0;
	B = c2 / c0;
	C = c3 / c0;

	/*  substitute x = y - A/3 to eliminate quadric term:  x^3 +px + q = 0 */
	sq_A = A * A;
	p = 1.0/3 * (- 1.0/3 * sq_A + B);
	q = 1.0/2 * (2.0/27 * A * sq_A - 1.0/3 * A * B + C);

	/* use Cardano's formula */
	cb_p = p * p * p;
	D = q * q + cb_p;

	if (FMath::IsNearlyZero(D)) {
		if (FMath::IsNearlyZero(q)) /* one triple solution */ {
			s0 = 0;
			num = 1;
		}
		else /* one single and one float solution */ {
			float u = GetCubicRoot(-q);
			s0 = 2 * u;
			s1 = - u;
			num = 2;
		}
	}
	else if (D < 0) /* Casus irreducibilis: three real solutions */ {
		float phi = 1.0/3 * FMath::Acos(-q / FMath::Sqrt(-cb_p));
		float t = 2 * FMath::Sqrt(-p);

		s0 =   t * FMath::Cos(phi);
		s1 = - t * FMath::Cos(phi + PI / 3);
		s2 = - t * FMath::Cos(phi - PI / 3);
		num = 3;
	}
	else /* one real solution */ {
		float sqrt_D = FMath::Sqrt(D);
		float u = GetCubicRoot(sqrt_D - q);
		float v = -GetCubicRoot(sqrt_D + q);

		s0 = u + v;
		num = 1;
	}

	/* resubstitute */
	sub = 1.0/3 * A;

	if (num > 0)    s0 -= sub;
	if (num > 1)    s1 -= sub;
	if (num > 2)    s2 -= sub;

	return num;
}

static int SolveQuartic(float c0, float c1, float c2, float c3, float c4, float& s0, float& s1, float& s2, float& s3) {
        s0 = 0.f;
        s1 = 0.f;
        s2 = 0.f;
        s3 = 0.f;

        TArray<float> Coeffs;
		Coeffs.Init(0.f, 4);
        float  z, u, v, sub;
        float  A, B, C, D;
        float  sq_A, p, q, r;
        int     num;

        /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
        A = c1 / c0;
        B = c2 / c0;
        C = c3 / c0;
        D = c4 / c0;

        /*  substitute x = y - A/4 to eliminate cubic term: x^4 + px^2 + qx + r = 0 */
        sq_A = A * A;
        p = - 3.0/8 * sq_A + B;
        q = 1.0/8 * sq_A * A - 1.0/2 * A * B + C;
        r = - 3.0/256*sq_A*sq_A + 1.0/16*sq_A*B - 1.0/4*A*C + D;

        if (FMath::IsNearlyZero(r)) {
	        /* no absolute term: y(y^3 + py + q) = 0 */

	        Coeffs[ 3 ] = q;
	        Coeffs[ 2 ] = p;
	        Coeffs[ 1 ] = 0;
	        Coeffs[ 0 ] = 1;

	        num = SolveCubic(Coeffs[0], Coeffs[1], Coeffs[2], Coeffs[3], s0,  s1,  s2);
        }
        else {
	        /* solve the resolvent cubic ... */
	        Coeffs[ 3 ] = 1.0/2 * r * p - 1.0/8 * q * q;
	        Coeffs[ 2 ] = - r;
	        Coeffs[ 1 ] = - 1.0/2 * p;
	        Coeffs[ 0 ] = 1;

            SolveCubic(Coeffs[0], Coeffs[1], Coeffs[2], Coeffs[3], s0, s1, s2);

	        /* ... and take the one real solution ... */
	        z = s0;

	        /* ... to build two quadric equations */
	        u = z * z - r;
	        v = 2 * z - p;

	        if (FMath::IsNearlyZero(u))
	            u = 0;
	        else if (u > 0)
	            u = FMath::Sqrt(u);
	        else
	            return 0;

	        if (FMath::IsNearlyZero(v))
	            v = 0;
	        else if (v > 0)
	            v = FMath::Sqrt(v);
	        else
	            return 0;

	        Coeffs[ 2 ] = z - u;
	        Coeffs[ 1 ] = q < 0 ? -v : v;
	        Coeffs[ 0 ] = 1;

	        num = SolveQuadric(Coeffs[0], Coeffs[1], Coeffs[2], s0, s1);

	        Coeffs[ 2 ]= z + u;
	        Coeffs[ 1 ] = q < 0 ? v : -v;
	        Coeffs[ 0 ] = 1;

            if (num == 0) num += SolveQuadric(Coeffs[0], Coeffs[1], Coeffs[2], s0, s1);
            else if (num == 1) num += SolveQuadric(Coeffs[0], Coeffs[1], Coeffs[2], s1, s2);
            else if (num == 2) num += SolveQuadric(Coeffs[0], Coeffs[1], Coeffs[2], s2, s3);
        }

        /* resubstitute */
        sub = 1.0/4 * A;

        if (num > 0)    s0 -= sub;
        if (num > 1)    s1 -= sub;
        if (num > 2)    s2 -= sub;
        if (num > 3)    s3 -= sub;

        return num;
    }

int UDebugFunctionLibrary::SolveBallisticArc(const FVector& ProjectilePosition, const float ProjectileSpeed,
	const FVector& TargetPosition, const FVector& TargetVelocity, const float GravityZ, FVector& FiringSolution, FVector& SecondSolution)
{
	FiringSolution = FVector::ZeroVector;
	float G = GravityZ;
	float A = ProjectilePosition.X;
	float B = ProjectilePosition.Y;
	float C = ProjectilePosition.Z;
	float M = TargetPosition.X;
	float N = TargetPosition.Y;
	float O = TargetPosition.Z;
	float P = TargetVelocity.X;
	float Q = TargetVelocity.Y;
	float R = TargetVelocity.Z;
	float S = ProjectileSpeed;
	
	float H = M - A;
	float J = O - C;
	float K = N - B;
	float L = -.5f * G;

	float c0 = L*L;
	float c1 = -2*Q*L;
	float c2 = Q*Q - 2*K*L - S*S + P*P + R*R;
	float c3 = 2*K*Q + 2*H*P + 2*J*R;
	float c4 = K*K + H*H + J*J;

	TArray<float> Times;
	Times.Init(0.f, 4);
	int numTimes = SolveQuartic(c0, c1, c2, c3, c4, Times[0], Times[1], Times[2], Times[3]);
	Times.Sort();
	TArray<FVector> Solutions;
	Solutions.Init(FVector::ZeroVector, 2);
	int NumberOfSolutions = 0;
	
	for (int i = 0; i < Times.Num() && NumberOfSolutions < 2; ++i) {
		float t = Times[i];
		if (t <= 0 || FMath::IsNearlyZero(t))
			continue;

		Solutions[NumberOfSolutions].X = (float)((H+P*t)/t);
		Solutions[NumberOfSolutions].Y = (float)((K+Q*t-L*t*t)/ t);
		Solutions[NumberOfSolutions].Z = (float)((J+R*t)/t);
		++NumberOfSolutions;
	}

	// Write out solutions
	if (NumberOfSolutions > 0)   FiringSolution = Solutions[0];
	if (NumberOfSolutions > 1)   SecondSolution = Solutions[1];

	return NumberOfSolutions;
}

bool UDebugFunctionLibrary::SolveBallisticArcLateral(const FVector& ProjectilePosition, const float LateralSpeed,
	const FVector& TargetPosition, const float MaxHeight, FVector& FireVelocity, float& Gravity)
{
	// Handling these cases is up to your project's coding standards
	//check(ProjectilePosition != TargetPosition && LateralSpeed > 0 && MaxHeight > ProjectilePosition.Z);

	FireVelocity = FVector::ZeroVector;
	Gravity = NAN;

	FVector Diff = TargetPosition - ProjectilePosition;
	FVector DiffXZ(Diff.X, 0.f, Diff.Z);
	float LateralDist = DiffXZ.Size();

	if (FMath::IsNearlyZero(LateralDist))
		return false;

	float Time = LateralDist / LateralSpeed;

	FireVelocity = DiffXZ.GetSafeNormal() * LateralSpeed;

	// System of equations. Hit MaxHeight at t=.5*Time. Hit target at t=Time.
	//
	// peak = y0 + vertical_speed*halfTime + .5*gravity*halfTime^2
	// end = y0 + vertical_speed*time + .5*gravity*time^s
	// Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
	float a = ProjectilePosition.Z;     // initial
	float b = MaxHeight;                // peak
	float c = TargetPosition.Z;         // final

	Gravity = -4 * (a - 2 * b + c) / (Time * Time);
	FireVelocity.Z = -(3 * a - 4 * b + c) / Time;

	return true;
}

bool UDebugFunctionLibrary::SolveBallisticArcLateral_Plus(const FVector& ProjectilePosition, const float LateralSpeed,
    const FVector& Target, const FVector& TargetVelocity, const float MaxHeightOffset, FVector& FireVelocity,
    float& Gravity, FVector& ImpactPoint)
{
    // Handling these cases is up to your project's coding standards
    //check(ProjectilePosition != Target && LateralSpeed > 0);

    FireVelocity = FVector::ZeroVector;
    Gravity = 0.f;
    ImpactPoint = FVector::ZeroVector;

    // Ground plane terms
    FVector TargetVelXZ(TargetVelocity.X, 0.f, TargetVelocity.Z);
    FVector DiffXZ = Target - ProjectilePosition;
    DiffXZ.Z = 0.f;

    // Derivation
    //   (1) Base formula: |P + V*t| = S*t
    //   (2) Substitute variables: |diffXZ + targetVelXZ*t| = S*t
    //   (3) Square both sides: Dot(diffXZ,diffXZ) + 2*Dot(diffXZ, targetVelXZ)*t + Dot(targetVelXZ, targetVelXZ)*t^2 = S^2 * t^2
    //   (4) Quadratic: (Dot(targetVelXZ,targetVelXZ) - S^2)t^2 + (2*Dot(diffXZ, targetVelXZ))*t + Dot(diffXZ, diffXZ) = 0
    float C0 = FVector::DotProduct(TargetVelXZ, TargetVelXZ) - LateralSpeed * LateralSpeed;
    float C1 = 2.f * FVector::DotProduct(DiffXZ, TargetVelXZ);
    float C2 = FVector::DotProduct(DiffXZ, DiffXZ);
    float T0, T1;
    int32 NumSolutions = SolveQuadric(C0, C1, C2, T0, T1);

    // Pick the smallest positive time
    bool Valid0 = NumSolutions > 0 && T0 > 0.f;
    bool Valid1 = NumSolutions > 1 && T1 > 0.f;

    float T;
    if (!Valid0 && !Valid1)
        return false;
    else if (Valid0 && Valid1)
        T = FMath::Min(T0, T1);
    else
        T = Valid0 ? T0 : T1;

    // Calculate impact point
    ImpactPoint = Target + (TargetVelocity * T);

    // Calculate fire velocity along XZ plane
    FVector Dir = ImpactPoint - ProjectilePosition;
    FireVelocity = FVector(Dir.X, 0.f, Dir.Z).GetSafeNormal() * LateralSpeed;

    // Solve the system of equations. Hit max_height at t=.5*time. Hit target at t=time.
    //
    // peak = y0 + vertical_speed*halfTime + .5*gravity*halfTime^2
    // end = y0 + vertical_speed*time + .5*gravity*time^s
    // Wolfram Alpha: solve b = a + .5*v*t + .5*g*(.5*t)^2, c = a + vt + .5*g*t^2 for g, v
    float A = ProjectilePosition.Z;                                           // initial
    float B = FMath::Max(ProjectilePosition.Z, ImpactPoint.Z) + MaxHeightOffset;   // peak
    float C = ImpactPoint.Z;                                                  // final

    Gravity = -4 * (A - 2 * B + C) / (T * T);
    FireVelocity.Z = -(3 * A - 4 * B + C) / T;

    return true;
}

float UDebugFunctionLibrary::GetStandardDeviation(const TArray<float>& Data)
{
	float Sum = 0.f;
	float Mean = 0.f;
	float StandardDeviation = 0.f;

	for(const float Value : Data)
	{
		Sum += Value;
	}

	Mean = Sum / Data.Num();

	for(const float Value : Data)
	{
		StandardDeviation += FMath::Pow(Value - Mean, 2);
	}

	return FMath::Sqrt(StandardDeviation / Data.Num());
}

float UDebugFunctionLibrary::GetLeanDotProduct(const FVector& CurrentForwardVector, const FVector& PreviousForwardVector)
{
	return FVector::DotProduct(FVector::CrossProduct(CurrentForwardVector, FVector::UpVector), PreviousForwardVector);
}

FVector UDebugFunctionLibrary::GetVectorIntersectionPoint(const FVector& V1StartPoint, const FVector& V1EndPoint,
	const FVector& V2StartPoint, const FVector& V2EndPoint)
{
	const FVector V1 = V1EndPoint - V1StartPoint;
	const FVector V2 = V2EndPoint - V2StartPoint;
	const FVector V1_X_V2 = V1.Cross(V2);
	
	if(V1_X_V2.IsNearlyZero()) return FVector();

	const FVector V3 = V2StartPoint - V1StartPoint;
	const FVector T1 = V3.Cross(V2) / V1_X_V2;
	return V1StartPoint + V1 * T1;
}

float UDebugFunctionLibrary::CalculateCircleRadiusAndCenter(const FVector& CirclePoint1, const FVector& Tangent1,
	const FVector& Tangent2, const float ArcLength, FVector& CircleCenter)
{
	//Cross products of Tangents 1 and 2, meaning it travels through the circle center. I.E. they are perpendicular to the tangents
	FVector T1_Cross;
	FVector T2_Cross;
	
	// Checks which side of the tangent the circle is
	if(GetLeanDotProduct(Tangent1, Tangent2) < 0.f)
	{
		// Initialize the Perpendicular vectors
		T1_Cross = FVector::CrossProduct(Tangent1, FVector::UpVector);
		T2_Cross = FVector::CrossProduct(Tangent2, FVector::UpVector);
	}
	else
	{
		// Same, but opposite direction
		T1_Cross = -FVector::CrossProduct(Tangent1, FVector::UpVector);
		T2_Cross = -FVector::CrossProduct(Tangent2, FVector::UpVector);
	}
	//Normalize vectors for calculating the dot product
	T1_Cross.Normalize();
	T2_Cross.Normalize();
	const float DotProduct = FVector::DotProduct(T1_Cross, T2_Cross);
	
	// Circle radius calculated using r = b / x, where x is the angle for the circle sector, b is the length of the arc and r is the radius.
	const float CircleRadius = UKismetMathLibrary::SafeDivide(ArcLength, FMath::Acos(DotProduct));
	CircleCenter = T1_Cross * FVector(CircleRadius) + CirclePoint1;
	return CircleRadius;
}

FVector UDebugFunctionLibrary::CalculatePositionOnCircleArc(const FVector& CurrentLocation, const FVector& CircleCenter, const float CircleRadius,
	const float Speed, const float PredictionTime, const bool bClockwise)
{
	// Future position on the circle arc in radians
	float Radians;
	// Circle circumference
	const float Circumference = 2 * PI * CircleRadius;
	// Calculate the number of laps based on the speed and prediction time.
	float NumberOfLaps = (Speed * PredictionTime) / Circumference;
	// Only need a value between [0.f, 1.f)
	NumberOfLaps -= FMath::Floor(NumberOfLaps);

	// Calculate the normalized vector from the current location to the circle center
	const FVector NormalizedVector = UKismetMathLibrary::Normal((CurrentLocation - CircleCenter));
	// Calculate the dot product of the normalized vector with the X-axis and Y-axis
	const float DotProduct = FVector::DotProduct(NormalizedVector, FVector(1.0, 0.f, 0.f));
	const float YDotProduct = FVector::DotProduct(NormalizedVector, FVector(0.f, 1.f, 0.f));

	if(bClockwise)
	{
		// Clockwise movement
		Radians = 2*PI*NumberOfLaps;
		if(YDotProduct > 0.f)
		{
			// Add the angle between the normalized vector and the X-axis
			Radians += FMath::Acos(DotProduct);
		}
		else
		{
			// Add the supplementary angle to account for the Y-axis direction
			Radians += (PI + (PI - FMath::Acos(DotProduct)));
		}
	}
	else
	{
		// Counterclockwise movement
		Radians = -2*PI*NumberOfLaps;
		if(YDotProduct < 0.f)
		{
			// Subtract the angle between the normalized vector and the X-axis
			Radians -= FMath::Acos(DotProduct);
		}
		else
		{
			// Subtract the supplementary angle to account for the Y-axis direction
			Radians -= (PI + (PI - FMath::Acos(DotProduct)));
		}
	}
	
	// Calculate the return vector by rotating the X-axis vector by the calculated angle in radians around the UpVector
	FVector ReturnVector = FVector(1.f, 0.f, 0.f).RotateAngleAxisRad(Radians, FVector::UpVector);

	// Scale the return vector by the circle radius and add the circle center to get the final position
	ReturnVector = (ReturnVector * CircleRadius) + CircleCenter;
	return ReturnVector;

}

FVector UDebugFunctionLibrary::CalculatePositionOnCircleArc_Ver2(const FVector& CurrentLocation, const FVector& CircleCenter,
	const float Speed, const float PredictionTime, const bool bClockwise)
{
	return CalculatePositionOnCircleArc(CurrentLocation, CircleCenter, FVector::Distance(CurrentLocation, CircleCenter), Speed, PredictionTime, bClockwise);
}

void UDebugFunctionLibrary::DebugBoxSimple_Red(const UObject* WorldContextObject, const FVector& Location)
{
	UKismetSystemLibrary::DrawDebugBox(
		WorldContextObject,
		Location,
		FVector(10.f),
		FLinearColor::Red,
		FRotator::ZeroRotator,
		2.f,
		2.f);
}

ACombatManager* UDebugFunctionLibrary::GetCombatManager(const UObject* WorldContextObject)
{
	return Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject))->GetCombatManager();
}


