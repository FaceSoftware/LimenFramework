// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenObjective.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenObjectivesManager.generated.h"


struct FObjectiveData;

UCLASS()
class LIMENOBJECTIVES_API ALimenObjectivesManager : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FObjectivesManagerInit, ALimenObjectivesManager* /* ObjectivesManager */);

	FObjectivesManagerInit OnInitialized;
	
	ALimenObjectivesManager();
	virtual void Start() override;

	TArray<ALimenObjective*> LoadObjectivesForPlayer(APlayerController* Controller, APawn* Pawn, const TArray<TSubclassOf<ALimenObjective>>& InObjectives) const;
	TArray<ALimenObjective*> GetObjectivesForPlayer(const APlayerController* Controller, const APawn* Pawn) const;
	const TArray<ALimenObjective*>& GetObjectivesInstances() const;

	template<typename ObjectiveClass>
	ObjectiveClass* FindObjective() const
	{
		static_assert(std::is_base_of_v<ALimenObjective, ObjectiveClass>);
		for (ALimenObjective* Objective : ObjectiveInstances)
		{
			if (Objective->IsA<ObjectiveClass>())
			{
				return Objective;
			}
		}
		return nullptr;
	}

	ALimenObjective* FindObjective(const TSubclassOf<ALimenObjective>& ObjectiveClass) const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Setup")
	TArray<TSubclassOf<ALimenObjective>> ObjectivesClassList;
	UPROPERTY()
	TArray<ALimenObjective*> ObjectiveInstances;

	UFUNCTION()
	void ObjectiveAdded(ALimenObjective* Objective, const FObjectiveData& Data);
};
