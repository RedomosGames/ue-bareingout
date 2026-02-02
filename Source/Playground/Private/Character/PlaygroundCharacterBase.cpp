// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlaygroundCharacterBase.h"
#include "GAS/Attributes/BasicAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
APlaygroundCharacterBase::APlaygroundCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

	BasicAttributeSet = CreateDefaultSubobject<UBasicAttributeSet>(TEXT("BasicAttributeSet"));

}

// Called when the game starts or when spawned
void APlaygroundCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlaygroundCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlaygroundCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		GrantAbilities(StartingAbilities);
	}
}

void APlaygroundCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

TArray<FGameplayAbilitySpecHandle> APlaygroundCharacterBase::GrantAbilities(TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGrant)
{
	if (!AbilitySystemComponent || !HasAuthority()) { return TArray<FGameplayAbilitySpecHandle>(); }

	TArray<FGameplayAbilitySpecHandle> AbilityHandles;

	for (TSubclassOf<UGameplayAbility> Ability: AbilitiesToGrant)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, -1, this));

		AbilityHandles.Add(SpecHandle);
	}
	
	SendAbilitiesChangedEvent();

	return AbilityHandles;
}

void APlaygroundCharacterBase::RemoveAbilities(TArray<FGameplayAbilitySpecHandle> AbilityHandleToRemove)
{
	if (!AbilitySystemComponent || !HasAuthority()) { return; }

	for (FGameplayAbilitySpecHandle AbilityHandle : AbilityHandleToRemove)
	{
		AbilitySystemComponent->ClearAbility(AbilityHandle);
	}

	SendAbilitiesChangedEvent();
}

void APlaygroundCharacterBase::SendAbilitiesChangedEvent()
{
	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Abilities.Changed"));
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventData.EventTag, EventData);
}


UAbilitySystemComponent* APlaygroundCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


// Called to bind functionality to input
void APlaygroundCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

