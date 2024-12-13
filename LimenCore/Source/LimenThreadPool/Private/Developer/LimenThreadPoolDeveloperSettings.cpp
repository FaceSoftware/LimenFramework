// Fill out your copyright notice in the Description page of Project Settings.


#include "Developer/LimenThreadPoolDeveloperSettings.h"


ULimenThreadPoolDeveloperSettings::ULimenThreadPoolDeveloperSettings()
{
	ThreadsPriority = EBP_ThreadPriority::Normal;
	bAutoDetermineThreadCount = true;
	NumThreadsToUse = 0;
	MemoryPerThreadInBytes = 512;
}

int32 ULimenThreadPoolDeveloperSettings::GetThreadCount() const
{
	if (!bAutoDetermineThreadCount)
	{
		return NumThreadsToUse;
	}

#if PLATFORM_WINDOWS
	return FWindowsPlatformMisc::NumberOfCores();
#else
	return 0;
#endif
}

FName ULimenThreadPoolDeveloperSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName ULimenThreadPoolDeveloperSettings::GetCategoryName() const
{
	return TEXT("Game");
}

FName ULimenThreadPoolDeveloperSettings::GetSectionName() const
{
	return TEXT("Limen - Thread Pool");
}

#if WITH_EDITOR
FText ULimenThreadPoolDeveloperSettings::GetSectionText() const
{
	return FText::FromName(GetSectionName());
}

FText ULimenThreadPoolDeveloperSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("Windows Only!! Thread pool settings"));
}
#endif
