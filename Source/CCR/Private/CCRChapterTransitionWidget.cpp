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
}

void UCCRChapterTransitionWidget::OnDismissTransition_Implementation()
{
// Default C++ implementation: no-op.
}
