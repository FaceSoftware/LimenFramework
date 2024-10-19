// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShaderPipelineCache.h"
#include "Engine/GameInstance.h"
#include "LimenGameInstance.generated.h"


class AInitializerProxyActor;
DECLARE_MULTICAST_DELEGATE(FShaderCompilationDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FShaderCompilationPercentageDelegate, double /* ProgressPercentage */);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLiteralShaderCompilationDelegate, int32 /* RemainingShaders */, int32 /* TotalShaders */);

class LIMEN_API FShaderCompiler
{
public:
	FShaderCompilationPercentageDelegate OnShaderCompilationUpdated;
	FLiteralShaderCompilationDelegate OnShaderCompilationUpdatedWithDetails;
	FShaderCompilationDelegate OnCompilationStarted;
	FShaderCompilationDelegate OnCompilationFinished;

	static TSharedPtr<FShaderCompiler> Get();

	void StartCompiling(UWorld* InWorld, const FShaderPipelineCache::BatchMode InBatchMode);

	bool IsCompiling() const;

	void StopCompiling();

protected:

private:
	static TSharedPtr<FShaderCompiler> ShaderCompiler;

	UWorld* World;
	FShaderPipelineCache::BatchMode BatchMode;

	bool bIsCompiling;
	bool bStopCompiling;
	uint32 RemainingShaders;
	uint32 PreviousRemainingShaders;
	int64 TotalShaders;
	
	FDelegateHandle CheckCompileFinishedDelegateHandle;
	
	void StartShaderBatching();	
	void CheckCompileFinished();
	void CheckCompileFinished_Internal();
	
	FShaderCompiler();
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBlueprintShaderCompilationDelegate, double, ProgressPercentage);

/**
 * 
 */
UCLASS()
class LIMEN_API ULimenGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FBlueprintShaderCompilationDelegate OnShaderCompilationProgressUpdated;

	template<typename GameInstanceClass>
	static GameInstanceClass* GetGameInstance(UWorld* World)
	{
		static_assert(std::is_base_of_v<UGameInstance, GameInstanceClass>);
		GameInstanceClass* GameInstancePtr = Cast<GameInstanceClass>(World->GetGameInstance());
		
		return IsValid(GameInstancePtr) ? GameInstancePtr : nullptr;
	}

	static ULimenGameInstance* GetGameInstance(UWorld* World);

	virtual void Init() override;
	virtual void Shutdown() override;
	
	const TSharedPtr<FShaderCompiler>& GetShaderCompiler();

protected:
	void StartPreGameShaderCompile() const;
	void StartLoadingScreenShaderCompile() const;
	void StopShaderCompilation() const;

	virtual void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues IVS);

private:
	TSharedPtr<FShaderCompiler> PSOBatch;
	UPROPERTY()
	TObjectPtr<AInitializerProxyActor> InitializationProxy;
	
	void SetupInitialization();
	UFUNCTION()
	void BindLoadingScreenEvents();
	FDelegateHandle BindLoadingScreenEventsDelegateHandle;
	UFUNCTION()
	void OpenMainMenu();
	FDelegateHandle OpenMainMenuDelegateHandle;
	
	void PSOBatchStarted();
	void PSOBatchFinished();

	void ShaderCompilationProgressUpdated(const double Percentage);

	uint32 WorldChangeCount;
};
