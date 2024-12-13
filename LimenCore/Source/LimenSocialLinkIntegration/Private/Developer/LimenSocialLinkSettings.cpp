// Copyright Face Software. All Rights Reserved.


#include "Developer/LimenSocialLinkSettings.h"


FSocialLink::FSocialLink() = default;

bool ULimenSocialLinkSettings::GetSocialLink(const FName& Id, FSocialLink& OutLink)
{
	const ULimenSocialLinkSettings* SocialLinkSettings = GetDefault<ULimenSocialLinkSettings>();
	check(SocialLinkSettings != nullptr);

	const FSocialLink* Link = SocialLinkSettings->SocialLinks.FindByPredicate([&Id] (const FSocialLink& SocialLink)
	{
		return SocialLink.Id == Id;
	});

	if (Link == nullptr)
	{
		return false;
	}

	OutLink = *Link;
	return true;
}

const TArray<FSocialLink>& ULimenSocialLinkSettings::GetSocialLinks()
{
	const ULimenSocialLinkSettings* SocialLinkSettings = GetDefault<ULimenSocialLinkSettings>();
	check(SocialLinkSettings != nullptr);

	return SocialLinkSettings->SocialLinks;
}

FName ULimenSocialLinkSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName ULimenSocialLinkSettings::GetCategoryName() const
{
	return TEXT("Game");
}

FName ULimenSocialLinkSettings::GetSectionName() const
{
	return TEXT("Limen - Social Link Integration");
}

#if WITH_EDITOR
FText ULimenSocialLinkSettings::GetSectionText() const
{
	return FText::FromName(GetSectionName());
}

FText ULimenSocialLinkSettings::GetSectionDescription() const
{
	return FText::FromString(TEXT("Easily setup and access social links to display in-game."));
}
#endif
