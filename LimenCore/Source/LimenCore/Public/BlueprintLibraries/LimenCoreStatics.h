// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "LimenCoreStatics.generated.h"


class ULimenGlobalRandomStreamSubsystem;
class APostProcessVolume;

USTRUCT(BlueprintType)
struct FPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float A;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float B;
	
	FPair() : A(0.f), B(0.f) {}
};

UENUM(BlueprintType)
enum class ELogType : uint8
{
	Log,
	Warning,
	Error,
	None UMETA(Hidden),
};

UENUM(BlueprintType)
enum class EMinimalWorldTypes : uint8
{
	/** An untyped world, in most cases this will be the vestigial worlds of streamed in sub-levels */
	Untyped,

	/** The game world */
	Game,

	/** A world being edited in the editor */
	Editor,

	/** A Play In Editor world */
	PIE,

	/** A preview world for an editor tool */
	EditorPreview,

	/** A preview world for a game */
	GamePreview,

	/** A minimal RPC world for a game */
	GameRPC,

	/** An editor world that was loaded but not currently being edited in the level editor */
	Inactive
};

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenCoreStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Limen|Debug", meta=(WorldContext=Caller, Development))
	static void LimenLog(const UObject* Caller, const FString LogText, const ELogType Verbosity = ELogType::Log, const bool bPrintToScreen = true);

	UFUNCTION(BlueprintCallable, Category="Limen|Debug", meta=(WorldContext=Caller))
	static void StaticLimenLog(const FString FunctionName, const FString LogText, const ELogType Verbosity = ELogType::Log, const bool bPrintToScreen = true);

	UFUNCTION(BlueprintCallable, Category="Limen|Debug", meta=(WorldContext=Caller, ExpandBoolAsExecs="bIsEditorGame"))
	static void IsGamePlayingInEditor(UObject* Caller, bool& bIsEditorGame);

	UFUNCTION(BlueprintCallable, Category="Limen|Post Process")
	static void SetDynamicGlobalIlluminationMethod(APostProcessVolume* PostProcessVolume, EDynamicGlobalIlluminationMethod::Type NewMethod);

	UFUNCTION(BlueprintCallable, Category="Limen|Post Process")
	static void SetReflectionMethod(APostProcessVolume* PostProcessVolume, EReflectionMethod::Type NewMethod);
	
	/**
	 * @brief Acts as a coin toss
	 * @param ChanceToWin The probability of winning.
	 * @param bWon True if won, false if lost
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Math", meta=(WorldContext="WorldContext", ExpandBoolAsExecs="bWon"))
	static void RandomPercentage(const double ChanceToWin, UPARAM(DisplayName="Has Won")bool& bWon);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Time")
	static FText GetCurrentTime();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Time")
	static FText GetCurrentDate();

	UFUNCTION(BlueprintCallable, Category="Limen|Time", meta=(WorldContext=Caller))
	static AActor* GetActorWithName(const UObject* Caller, const FString& ActorName);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Time", meta=(WorldContext=Caller))
	static AActor* GetActorWithTag(const UObject* Caller, const FName& Tag);

	UFUNCTION(BlueprintCallable, Category="Limen|Debug", meta=(WorldContext=Caller, ExpandEnumAsExecs="ReturnValue"))
	static EMinimalWorldTypes GetWorldType(UObject* Caller);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Navigation", meta=(WorldContext=WorldContext), BlueprintPure)
	static bool AreLocationsReachableByNavigation(UObject* WorldContext, const FVector& Start, const FVector& End);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Navigation", BlueprintPure)
	static FString GetGameDefaultMap();

	UFUNCTION(BlueprintCallable, Category="Limen|Navigation", BlueprintPure)
	static FString GetGameTransitionMap();

	template <typename T>
	static TArray<T> ShuffleArray(const TArray<T>& In);
	
	UFUNCTION(BlueprintCallable, Category="Limen|String", meta=(ExpandEnumAsExecs="ReturnValue"))
	static bool SetCharacterAtIndex(const FString& String, const int32 Index, const FString& Char, FString& OutString);

	UFUNCTION(BlueprintCallable, Category = "Game Config", BlueprintPure)
	static FString GetProjectTitle();
	
	UFUNCTION(BlueprintCallable, Category = "Game Config", BlueprintPure)
	static FString GetProjectVersion();

	UFUNCTION(BlueprintCallable, Category = "Game Config", BlueprintPure)
	static FString GetCopyrightNotice();

	UFUNCTION(BlueprintCallable, Category = "Network")
	static void GetPacketLoss(APlayerController* PC, float& InLoss, float& OutLoss);
};

template <typename T>
TArray<T> ULimenCoreStatics::ShuffleArray(const TArray<T>& In)
{
	TArray<T> Out = In;
	ULimenGlobalRandomStreamSubsystem* RandomStream = ULimenGlobalRandomStreamSubsystem::Get();
	
	if (Out.Num() > 1)
	{
		for (int32 i = 0; i < Out.Num(); i++)
		{
			const int32 j = RandomStream->RandomIntRange(Out.Num() - 1, i);
			if (i != j)
			{
				Out.Swap(i, j);
			}
		}
	}

	return Out;
}
