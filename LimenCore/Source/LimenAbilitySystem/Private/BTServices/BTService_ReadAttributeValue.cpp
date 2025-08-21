// Copyright FaceSoftware. All Rights Reserved.


#include "BTServices/BTService_ReadAttributeValue.h"

#include "Attributes/LimenAttributeBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_ReadAttributeValue::UBTService_ReadAttributeValue()
{
	NodeName = TEXT("Read Attribute Value");
	bCanTickOnSearchStartBeExposed = true;
	ReadValueMethod = EReadAttributeMethod::Fraction;
	bNotifyBecomeRelevant = bNotifyTick = bNotifyCeaseRelevant = true;
}

void UBTService_ReadAttributeValue::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	AttributeInstanceKey.ResolveSelectedKey(*Asset.GetBlackboardAsset());
	KeyToStoreValue.ResolveSelectedKey(*Asset.GetBlackboardAsset());
}

void UBTService_ReadAttributeValue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	
}

void UBTService_ReadAttributeValue::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (AttributeInstanceKey.IsNone()) return;
	if (KeyToStoreValue.IsNone()) return;

	UObject* TempAttribute = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AttributeInstanceKey.SelectedKeyName);
	const auto* Attribute = Cast<ULimenAttributeBase>(TempAttribute);
	if (!Attribute) return;

	float ValueToSet = 0.f;
	switch (ReadValueMethod)
	{
	case EReadAttributeMethod::Literal:
		ValueToSet = Attribute->GetCurrentValue();
		break;

	case EReadAttributeMethod::Fraction:
		ValueToSet = Attribute->GetCurrentValueNormalized();
		break;

	case EReadAttributeMethod::Percentage:
		ValueToSet = Attribute->GetCurrentValueAsPercentage();
		break;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(KeyToStoreValue.SelectedKeyName, ValueToSet);
}
