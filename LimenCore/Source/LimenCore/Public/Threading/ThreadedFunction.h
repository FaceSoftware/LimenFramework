// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * Threading made easy.
 * Just takes a lambda function as an argument and enables the function run on a thread.
 */
class LIMENCORE_API FThreadedFunction final : public FRunnable
{
public:
	explicit FThreadedFunction(const TFunction<void()>& InFunction, const TFunction<void()>& InStopCallback = [] {} );
    virtual ~FThreadedFunction() override = default;
    virtual FSingleThreadRunnable* GetSingleThreadInterface() override;
    virtual uint32 Run() override;
	virtual void Stop() override;

private:
	TFunction<void()> Function;
	TFunction<void()> StopCallback;
};
