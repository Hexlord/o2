#include "o2Editor/stdafx.h"
#include "SpoilerWithHead.h"

#include "o2/Scene/UI/Widgets/Button.h"

namespace Editor
{

	SpoilerWithHead::SpoilerWithHead() :
		Spoiler()
	{}

	SpoilerWithHead::SpoilerWithHead(const SpoilerWithHead& other) :
		Spoiler(other)
	{
		InitializeControls();
		RetargetStatesAnimations();
	}

	SpoilerWithHead& SpoilerWithHead::operator=(const SpoilerWithHead& other)
	{
		Spoiler::operator=(other);
		return *this;
	}

	Image* SpoilerWithHead::GetIcon() const
	{
		return mIcon;
	}

	String SpoilerWithHead::GetCreateMenuCategory()
	{
		return "UI/Editor";
	}

	void SpoilerWithHead::InitializeControls()
	{
		mOptionsBtn = FindInternalWidgetByType<Button>("options");
		if (mOptionsBtn)
			mOptionsBtn->onClick = [&]() { onOptionsPressed(); };

		mSaveBtn = FindInternalWidgetByType<Button>("save");
		if (mSaveBtn)
			mSaveBtn->onClick = [&]() { onSavePressed(); };

		mIcon = FindInternalWidgetByType<Image>("icon");
	}

	void SpoilerWithHead::CopyData(const Actor& otherActor)
	{
		Spoiler::CopyData(otherActor);
		InitializeControls();

	}
}

DECLARE_CLASS(Editor::SpoilerWithHead);
