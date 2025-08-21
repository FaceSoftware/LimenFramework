// Copyright FaceSoftware. All Rights Reserved.


#include "BTServices/BTService_CompareAttributeValue.h"

#include "Attributes/LimenAttributeBase.h"
#include "BlueprintLibraries/LimenCoreStatics.h"


UBTService_CompareAttributeValue::UBTService_CompareAttributeValue()
{
	NodeName = TEXT("Compare Attribute Value");
	bCanTickOnSearchStartBeExposed = true;
	ComparisonResult.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, ComparisonResult));
}

void UBTService_CompareAttributeValue::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);
}

void UBTService_CompareAttributeValue::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!AttributeInstance.IsSet()) return;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	UObject* TempAttribute = BlackboardComponent->GetValueAsObject(AttributeInstance.SelectedKeyName);
	const auto* Attribute = CastChecked<ULimenAttributeBase>(TempAttribute, ECastCheckedType::NullAllowed);
	if (!Attribute) return;

	const float CurrentValue = Attribute->GetCurrentValue();
	const float ComparatorValue = ValueToCompareAgainst.GetValue(BlackboardComponent);

	bool bComparisonValue = false;
	switch (ComparisonOperator)
	{
	case EComparisonOperator::GreaterThan:
		bComparisonValue = CurrentValue > ComparatorValue;
		break;

	case EComparisonOperator::LessThan:
		bComparisonValue = CurrentValue < ComparatorValue;
		break;

	case EComparisonOperator::Equal:
		bComparisonValue = CurrentValue == ComparatorValue;
		break;

	case EComparisonOperator::NotEqual:
		bComparisonValue = CurrentValue != ComparatorValue;
		break;

	case EComparisonOperator::GreaterThanOrEqualTo:
		bComparisonValue = CurrentValue >= ComparatorValue;
		break;

	case EComparisonOperator::LessThanOrEqualTo:
		bComparisonValue = CurrentValue <= ComparatorValue;
		break;
	}

	BlackboardComponent->SetValueAsBool(ComparisonResult.SelectedKeyName, bComparisonValue);
}
