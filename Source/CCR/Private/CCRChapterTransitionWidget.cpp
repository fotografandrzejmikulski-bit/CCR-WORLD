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
