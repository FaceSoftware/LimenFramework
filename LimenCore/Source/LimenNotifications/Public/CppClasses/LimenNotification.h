// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenNotification.generated.h"


// Dummy class for UHT
UCLASS(Abstract, NotBlueprintable, NotBlueprintable, Hidden)
class ULimenNotification : public UObject
{
	GENERATED_BODY()

public:
	
};


USTRUCT(BlueprintType)
struct FNotificationParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseCustomDisplayTime = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DisplayTime = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NotificationTitle = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText NotificationMessage = FText();
};

class LIMENNOTIFICATIONS_API FLimenNotification
{
public:
	FLimenNotification() = default;
	explicit FLimenNotification(const FNotificationParams& InNotificationParams);
	~FLimenNotification();
	
	bool UseCustomDisplayTime() const;
	float GetDisplayTime() const;
	const FText& GetNotificationTitle() const;
	const FText& GetNotificationMessage() const;

	bool operator==(const FLimenNotification* Other) const;

	explicit operator FNotificationParams() const;
	const FNotificationParams& GetParameters() const;

private:
	FNotificationParams Params;

};
