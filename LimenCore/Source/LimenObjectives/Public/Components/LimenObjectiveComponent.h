// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Managers/LimenObjective.h"
#include "LimenObjectiveComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectiveUpdate, ALimenObjective*, Objective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectiveComponentUpdate, ULimenObjectiveComponent*, ObjectiveComponent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENOBJECTIVES_API ULimenObjectiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FObjectiveUpdate OnNewObjectiveAdded;
	UPROPERTY(BlueprintAssignable)
	FObjectiveUpdate OnObjectiveUpdated;
	UPROPERTY(BlueprintAssignable)
	FObjectiveUpdate OnObjectiveCompleted;
	
	UPROPERTY(BlueprintAssignable)
	FObjectiveComponentUpdate OnObjectivesLoaded;

	UPROPERTY(BlueprintAssignable)
	FObjectiveUpdate OnObjectivesUntracked;
	
	ULimenObjectiveComponent();

	bool TrackObjective(ALimenObjective* NewObjective);
	bool IsTrackingObjective(ALimenObjective* Test) const;

	void LoadObjectives(const TArray<ALimenObjective*>& InObjectives);

	UFUNCTION(BlueprintCallable, Category="Limen|Objective Component")
	const TArray<ALimenObjective*>& GetObjectives() const;

protected:
	void UntrackObjective(ALimenObjective* NewObjective);
	void ResetObjectives();
	
private:
	UPROPERTY()
	TArray<ALimenObjective*> ObjectivesList;

	bool ObjectiveAlreadyExist(const ALimenObjective* InTest) const;

	UFUNCTION()
	void ObjectiveUpdated(ALimenObjective* InObjective, const FObjectiveData& NewData);
	UFUNCTION()
	void ObjectiveCompleted(ALimenObjective* InObjective, const FObjectiveData& NewData);
};
