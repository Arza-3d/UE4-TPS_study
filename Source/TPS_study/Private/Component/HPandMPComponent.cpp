#include "Component/HPandMPComponent.h"

//===========================================================================
// public:
//===========================================================================

UHPandMPComponent::UHPandMPComponent()
{
}

float UHPandMPComponent::AddHealth(const float AddHealth)
{
	return AddStat(&HealthAndMana.Health.Current, AddHealth, HealthAndMana.Health.Max);
}

float UHPandMPComponent::SetHealth(float NewHealth)
{
	return SetStatClamped(NewHealth, HealthAndMana.Health.Max);
}

float UHPandMPComponent::SetMana(float NewMana)
{
	return SetStatClamped(NewMana, HealthAndMana.Mana.Max);
}

FCeiledFloat UHPandMPComponent::SetMaxHealth(const float NewMaxHealth)
{
	if (NewMaxHealth <= 0.0f) return HealthAndMana.Health;

	if (NewMaxHealth > HealthAndMana.Health.Max)
	{
		HealthAndMana.Health.Max = NewMaxHealth;

		return HealthAndMana.Health;
	}
	else if (NewMaxHealth < HealthAndMana.Health.Current)
	{
		HealthAndMana.Health.Current = NewMaxHealth;

		HealthAndMana.Health.Max = NewMaxHealth;

		return HealthAndMana.Health;
	}

	HealthAndMana.Health.Max = NewMaxHealth;

	return HealthAndMana.Health;
}

float UHPandMPComponent::AddMana(const float AddMana)
{
	return AddStat(&HealthAndMana.Mana.Current, AddMana, HealthAndMana.Mana.Max);
}

//===========================================================================
// protected:
//===========================================================================

void UHPandMPComponent::BeginPlay()
{
	Super::BeginPlay();
}

float UHPandMPComponent::AddStat(float* CurrentStat, const float AddStat, const float MaxStat)
{
	float oldStat = *CurrentStat;
	float newStat = oldStat + AddStat;

	newStat = SetStatClamped(newStat, MaxStat);

	*CurrentStat = newStat;
	return newStat;
}

float UHPandMPComponent::SetStatClamped(float NewStat, const float MaxStat)
{
	if (NewStat <= 0.0f)
	{
		NewStat = 0.0f;
	}
	else if (NewStat >= MaxStat)
	{
		NewStat = MaxStat;
	}
	return NewStat;
}
