#include "CCRChapterTransitionWidget.h"

void UCCRChapterTransitionWidget::ShowChapterTitle(const FText& Title, const FText& Subtitle)
{
	SetVisibility(ESlateVisibility::Visible);
	OnShowChapterTitle(Title, Subtitle);
}

void UCCRChapterTransitionWidget::DismissTransition()
{
	OnDismissTransition();
	SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// BlueprintNativeEvent default implementations
// ---------------------------------------------------------------------------

void UCCRChapterTransitionWidget::OnShowChapterTitle_Implementation(const FText& Title, const FText& Subtitle)
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRChapterTransitionWidget::OnShowChapterTitle has no C++ or Blueprint implementation."))
#endif
}

void UCCRChapterTransitionWidget::OnDismissTransition_Implementation()
{
	// Default C++ implementation: no-op.
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Verbose, TEXT("[CCR] UCCRChapterTransitionWidget::OnDismissTransition has no C++ or Blueprint implementation."))
#endif
}
