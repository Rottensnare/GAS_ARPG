// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbility/AuraSummonAbility.h"

#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	/*UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(),
		Location,
		Location + LeftOfSpread * MaxSpawnDistance,
		4.f,
		FLinearColor::Blue,
		5.f,
		1.f);
	
	UKismetSystemLibrary::DrawDebugArrow(
		GetAvatarActorFromActorInfo(),
		Location,
		Location + RightOfSpread * MaxSpawnDistance,
		4.f,
		FLinearColor::Green,
		5.f,
		1.f);
	*/
	TArray<FVector> ReturnVectors;
	ReturnVectors.Add(FVector(Location + RightOfSpread * MaxSpawnDistance));
	ReturnVectors.Add(FVector(Location + LeftOfSpread * MaxSpawnDistance));
	ReturnVectors.Add(FVector(Location + Forward * MaxSpawnDistance));
	
	//TODO: Check if the spawn point is a valid point. Adjust location if possible.
	//FNavLocation ProjectedLocation;
	//UNavigationSystemV1::ProjectPointToNavigation(Location, ProjectedLocation, FVector(100.f));
	//ProjectedLocation.Location;
	
	return ReturnVectors;
}
