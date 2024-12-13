// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "LimenCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMEMODES_API ULimenCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void ForceCrash();

	UFUNCTION(Exec)
	void Die();
	
	UFUNCTION(Exec)
	void ForceGarbageCollection();

	UFUNCTION(Exec)
	void SaveGame();
	
	UFUNCTION(Exec)
	void LoadGame();
	
	virtual void God() override;

	UFUNCTION(Exec)
	void TakeDamage(const float Damage);

};
