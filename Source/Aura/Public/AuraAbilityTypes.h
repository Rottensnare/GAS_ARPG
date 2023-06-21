#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FGameplayEffectContext* Duplicate() const
	{
		FGameplayEffectContext* NewContext = new FGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;

	bool IsCriticalHit() const {return EffectModifierBits & (1 << 0);}
	bool IsBlockedHit() const {return EffectModifierBits & (1 << 1);}
	int32 GetEffectModifierBits() const {return EffectModifierBits;}

	void SetIsCriticalHit(const bool IN bCriticalHit) {EffectModifierBits |= bCriticalHit << 0;}
	void SetIsBlockedHit(const bool IN bBlockedHit) {EffectModifierBits |= bBlockedHit << 1;}
	void SetEffectModifierBits(const int32 IN InBits) {EffectModifierBits = InBits;}

protected:

	/**	Contains information as flags. 1st bit is for critical hits, 2nd bit is for blocked hits.	*/
	UPROPERTY()
	int32 EffectModifierBits = 0;

};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};