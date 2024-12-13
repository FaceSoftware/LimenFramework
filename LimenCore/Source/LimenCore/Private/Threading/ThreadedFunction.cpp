// Copyright Face Software. All Rights Reserved.


#include "Threading/ThreadedFunction.h"

#include "Namespaces/GlobalInfo.h"


FThreadedFunction::FThreadedFunction(const TFunction<void()>& InFunction, const TFunction<void()>& InStopCallback)
{
	Function = InFunction;
	StopCallback = InStopCallback;
}

uint32 FThreadedFunction::Run()
{
	Function();
	return LimenThreading::ExitCodes::Success;
}

void FThreadedFunction::Stop()
{
	StopCallback();
}
