// Fill out your copyright notice in the Description page of Project Settings.


#include "Debug/DebugFunctionLibrary.h"

#include <manipulations.h>

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

FVector UDebugFunctionLibrary::PredictProjectileDirection(const FVector& TargetPosition, FVector& OutDirection,
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

