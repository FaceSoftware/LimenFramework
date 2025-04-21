// Copyright Face Software. All Rights Reserved.


#include "BlueprintLibraries/LimenCoreStatics.h"

#include "EngineUtils.h"
#include "GameMapsSettings.h"
#include "NavigationSystem.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LogMacros/LimenLogMacros.h"
#include "Misc/ConfigCacheIni.h"
#include "Namespaces/GlobalInfo.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"


void ULimenCoreStatics::LimenLog(const UObject* Caller, const FString LogText, const ELogType Verbosity, const bool bPrintToScreen)
{
	float DisplayTime = 0.f;
	FColor TextColor = FColor();
	
	switch (Verbosity)
	{
	case ELogType::Log:
		LIMEN_LOG(LogLimen, Log, Caller, "%s", *LogText);
		DisplayTime = ErrorInformation::LogDisplayTime;
		TextColor = ErrorInformation::LogTextColor;
		break;

	case ELogType::Warning:
		LIMEN_LOG(LogLimen, Warning, Caller, "%s", *LogText);
		DisplayTime = ErrorInformation::WarningDisplayTime;
		TextColor = ErrorInformation::WarningTextColor;
		break;

	case ELogType::Error:
		LIMEN_LOG(LogLimen, Error, Caller, "%s", *LogText);
		DisplayTime = ErrorInformation::ErrorDisplayTime;
		TextColor = ErrorInformation::ErrorTextColor;
		break;
		
	default:
		checkNoEntry();
	}

#if !UE_BUILD_SHIPPING
	if (bPrintToScreen)
	{
		check(GEngine)
		const FString Message = FString::Printf(TEXT("%s: %s"), *Caller->GetClass()->GetName(), *LogText);
		UKismetSystemLibrary::PrintString(Caller, Message, bPrintToScreen, true, TextColor, DisplayTime);
	}
#endif
}

void ULimenCoreStatics::StaticLimenLog(const FString FunctionName, const FString LogText, const ELogType Verbosity,
	const bool bPrintToScreen)
{
	float DisplayTime = 0.f;
	FColor TextColor = FColor();
	
	switch (Verbosity)
	{
	case ELogType::Log:
		STATIC_LIMEN_LOG(LogLimen, Log, FunctionName, "%s", *LogText)
		DisplayTime = ErrorInformation::LogDisplayTime;
		TextColor = ErrorInformation::LogTextColor;
		break;

	case ELogType::Warning:
		STATIC_LIMEN_LOG(LogLimen, Warning, FunctionName, "%s", *LogText)
		DisplayTime = ErrorInformation::WarningDisplayTime;
		TextColor = ErrorInformation::WarningTextColor;
		break;

	case ELogType::Error:
		STATIC_LIMEN_LOG(LogLimen, Error, FunctionName, "%s", *LogText)
		DisplayTime = ErrorInformation::ErrorDisplayTime;
		TextColor = ErrorInformation::ErrorTextColor;
		break;
		
	default:
		checkNoEntry();
	}

#if !UE_BUILD_SHIPPING
	if (bPrintToScreen)
	{
		check(GEngine)
		const FString Message = FString::Printf(TEXT("Static Call (%s): %s"), *FunctionName, *LogText);
		GEngine->AddOnScreenDebugMessage(-1, DisplayTime, TextColor, Message);
	}
#endif
}

void ULimenCoreStatics::IsGamePlayingInEditor(UObject* Caller, bool& bIsEditorGame)
{
#if WITH_EDITOR
	bIsEditorGame = true;
#else
	bIsEditorGame = false;
#endif
}

void ULimenCoreStatics::SetDynamicGlobalIlluminationMethod(APostProcessVolume* PostProcessVolume,
                                                           const EDynamicGlobalIlluminationMethod::Type NewMethod)
{	
	PostProcessVolume->Settings.DynamicGlobalIlluminationMethod = NewMethod;
}

void ULimenCoreStatics::SetReflectionMethod(APostProcessVolume* PostProcessVolume, const EReflectionMethod::Type NewMethod)
{
	PostProcessVolume->Settings.ReflectionMethod = NewMethod;
}

void ULimenCoreStatics::RandomPercentage(const double ChanceToWin, bool& bWon)
{
	if (FMath::IsNearlyZero(ChanceToWin))
	{
		bWon = false;
		return;
	}

	if (FMath::IsNearlyEqual(ChanceToWin, 100.0))
	{
		bWon = true;
		return;
	}
	
	auto* GlobalStream = ULimenGlobalRandomStreamSubsystem::Get();
	check(GlobalStream)
	
	const double RandomValue = GlobalStream->GetGlobalRandomStream()->GetFraction() * 100.f;
	bWon = RandomValue <= ChanceToWin;
}

FText ULimenCoreStatics::GetCurrentTime()
{
	const auto CurrentTime = FDateTime::Now();
	FString TimeString;
	
	const int Hour = CurrentTime.GetHour();
	if (Hour <= 9) TimeString += TEXT("0");
	TimeString += FString::Printf(TEXT("%d:"), Hour);

	const int Minute = CurrentTime.GetMinute();
	if (Minute <= 9) TimeString += TEXT("0");
	TimeString += FString::Printf(TEXT("%d:"), Minute);
	
	const int Second = CurrentTime.GetSecond();
	if (Second <= 9) TimeString += TEXT("0");
	TimeString += FString::Printf(TEXT("%d"), Second);

	return FText::FromString(TimeString);
}

FText ULimenCoreStatics::GetCurrentDate()
{
	const auto CurrentTime = FDateTime::Now();
	
	FString MonthString;
	switch (CurrentTime.GetMonthOfYear())
	{
	case EMonthOfYear::January:
		MonthString = TEXT("January");
		break;
		
	case EMonthOfYear::February:
		MonthString = TEXT("February");
		break;
		
	case EMonthOfYear::March:
		MonthString = TEXT("March");
		break;
		
	case EMonthOfYear::April:
		MonthString = TEXT("April");
		break;
		
	case EMonthOfYear::May:
		MonthString = TEXT("May");
		break;
		
	case EMonthOfYear::June:
		MonthString = TEXT("June");
		break;
		
	case EMonthOfYear::July:
		MonthString = TEXT("July");
		break;
		
	case EMonthOfYear::August:
		MonthString = TEXT("August");
		break;
		
	case EMonthOfYear::September:
		MonthString = TEXT("September");
		break;
		
	case EMonthOfYear::October:
		MonthString = TEXT("October");
		break;
		
	case EMonthOfYear::November:
		MonthString = TEXT("November");
		break;
		
	case EMonthOfYear::December:
		MonthString = TEXT("December");
		break;
		
	default:
		checkNoEntry();
	}

	const uint8 Day = CurrentTime.GetDay();
	FString DayString = FString::FromInt(Day);
	switch (DayString[DayString.Len() - 1])
	{
	case '1':
		DayString += TEXT("st");
		break;
		
	case '2':
		DayString += TEXT("nd");
		break;
		
	case '3':
		DayString += TEXT("rd");
		break;
		
	default:
		DayString += TEXT("th");
		break;
	}
	
	const FString DateString = FString::Printf(TEXT("%s %s, %d"), *MonthString, *DayString, CurrentTime.GetYear()); 
	return FText::FromString(DateString);
}

AActor* ULimenCoreStatics::GetActorWithName(const UObject* Caller, const FString& ActorName)
{
	check(GEngine)
	const auto* World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetName() == ActorName)
			return *It;
	}

	return nullptr;
}

AActor* ULimenCoreStatics::GetActorWithTag(const UObject* Caller, const FName& Tag)
{
	check(GEngine)
	const auto* World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		for (const FName& ActorTag : It->Tags)
		{
			if (Tag == ActorTag)
			{
				return *It;
			}
		}
	}

	return nullptr;
}

EMinimalWorldTypes ULimenCoreStatics::GetWorldType(UObject* Caller)
{
	check(GEngine)
	const auto* World = GEngine->GetWorldFromContextObject(Caller, EGetWorldErrorMode::Assert);

	EMinimalWorldTypes OutWorldType = static_cast<EMinimalWorldTypes>(0);
	switch (World->WorldType)
	{
	case EWorldType::Type::Game:
		OutWorldType = EMinimalWorldTypes::Game;
		break;

	case EWorldType::Type::None:
		OutWorldType = EMinimalWorldTypes::Untyped;
		break;

	case EWorldType::Type::Editor:
		OutWorldType = EMinimalWorldTypes::Editor;
		break;

	case EWorldType::Type::Inactive:
		OutWorldType = EMinimalWorldTypes::Inactive;
		break;

	case EWorldType::Type::EditorPreview:
		OutWorldType = EMinimalWorldTypes::EditorPreview;
		break;
		
	case EWorldType::Type::GamePreview:
		OutWorldType = EMinimalWorldTypes::GamePreview;
		break;
		
	case EWorldType::Type::PIE:
		OutWorldType = EMinimalWorldTypes::PIE;
		break;
		
	case EWorldType::Type::GameRPC:
		OutWorldType = EMinimalWorldTypes::GameRPC;
		break;
	
	default:
		break;
	}

	return OutWorldType;
}

bool ULimenCoreStatics::AreLocationsReachableByNavigation(UObject* WorldContext, const FVector& Start, const FVector& End)
{
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(WorldContext->GetWorld());
	if (NavSys == nullptr)
	{
		return false;
	}

	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (NavData == nullptr)
	{
		return false;
	}

	// Prepare the pathfinding query using the navigation filter
	const FSharedConstNavQueryFilter NavFilter = NavData->GetDefaultQueryFilter();
	const FPathFindingQuery Query = FPathFindingQuery(nullptr, *NavData, Start, End, NavFilter);

	// Perform the pathfinding query
	const FPathFindingResult Result = NavSys->FindPathSync(Query);
	return Result.IsSuccessful() && !Result.Path->IsPartial();
}

FString ULimenCoreStatics::GetGameDefaultMap()
{
	return UGameMapsSettings::GetGameDefaultMap();
}

FString ULimenCoreStatics::GetGameTransitionMap()
{
	return GetDefault<UGameMapsSettings>()->TransitionMap.GetAssetName();
}

bool ULimenCoreStatics::SetCharacterAtIndex(const FString& String, const int32 Index, const FString& Char,
	FString& OutString)
{
	if (!String.IsValidIndex(Index) || Char.IsEmpty())
	{
		return false;
	}

	OutString.Empty();

	OutString = String;
	OutString[Index] = Char[0];
	return true;
}

FString ULimenCoreStatics::GetProjectTitle()
{
	FString ProjectTitle;
	verify(GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectName"),
		ProjectTitle,
		GGameIni
	));

	return ProjectTitle;
}

FString ULimenCoreStatics::GetProjectVersion()
{
	FString AppVersion;
	verify(GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		AppVersion,
		GGameIni
	));

	return AppVersion;
}

FString ULimenCoreStatics::GetCopyrightNotice()
{
	FString CopyrightNotice;
	verify(GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("CopyrightNotice"),
		CopyrightNotice,
		GGameIni
	));

	return CopyrightNotice;
}
