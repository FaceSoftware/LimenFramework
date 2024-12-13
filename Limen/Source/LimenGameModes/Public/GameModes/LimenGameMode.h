// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/LimenGameModeBase.h"
#include "LimenGameMode.generated.h"

class ALimenPlayerControllerBase;
class ALimenPlayerController;
enum class EPauseReason : uint8;

UCLASS(Blueprintable)
class LIMENGAMEMODES_API ALimenGameMode : public ALimenGameModeBase
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWorldPause, const bool, bIsPaused);
	
public:
	UPROPERTY(BlueprintAssignable)
	FWorldPause OnWorldPauseStateChanged;
	
	ALimenGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	
protected:
	/**
	 * @brief Handles an incoming pause request from a specific player.
	 * @param Player The player that requested the pause.
	 * @param PauseReason The reason the game is being paused.
	 */
	UFUNCTION()
	virtual void HandlePauseRequest(ALimenPlayerControllerBase* Player, const EPauseReason PauseReason);
	/**
	 * @brief Handles an incoming unpause requests from a specific player.
	 * @param Player The player that requested the unpause.
	 */
	UFUNCTION()
	virtual void HandleUnpauseRequest(ALimenPlayerControllerBase* Player);

	/**
	 * @brief Set whether or not the in game menu should pause the game.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bGameMenuPausesGame;

	/**
	 * @brief Set whether or not the pause menu should pause the game (useful for multiplayer games).
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bPauseMenuPausesGame;

	/**
	 * @brief Whether notifications that require clicking on a button should pause the game.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bNotificationsPauseGame;

private:
	uint8 PauseRequestCounter;
};
