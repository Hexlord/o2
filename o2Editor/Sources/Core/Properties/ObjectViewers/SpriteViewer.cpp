#include "stdafx.h"
#include "SpriteViewer.h"

#include "Core/Properties/Basic/AssetProperty.h"
#include "Core/Properties/Basic/BorderIntProperty.h"
#include "Core/Properties/Basic/ColorProperty.h"
#include "Core/Properties/Basic/EnumProperty.h"
#include "Core/Properties/Basic/FloatProperty.h"
#include "Core/Properties/Properties.h"
#include "UI/Spoiler.h"
#include "UI/UIManager.h"

namespace Editor
{
	SpriteViewer::SpriteViewer()
	{
		auto layout = mnew UIVerticalLayout();
		layout->spacing = 5;
		layout->fitByChildren = true;

		mViewWidget = layout;

		const Type& spriteType = TypeOf(Sprite);

		mImageProperty = dynamic_cast<ImageAssetProperty*>(
			o2EditorProperties.BuildField(layout, spriteType, "image", path, mFieldProperties, onChangeCompleted, onChanged));

		mColorProperty = dynamic_cast<ColorProperty*>(
			o2EditorProperties.BuildField(layout, spriteType, "color", path, mFieldProperties, onChangeCompleted, onChanged));

		mAlphaProperty = dynamic_cast<FloatProperty*>(
			o2EditorProperties.BuildField(layout, spriteType, "transparency", path, mFieldProperties, onChangeCompleted, onChanged));

		mModeProperty = dynamic_cast<EnumProperty*>(
			o2EditorProperties.BuildField(layout, spriteType, "mode", path, mFieldProperties, onChangeCompleted, onChanged));

		mModeProperty->onChanged += [&](IPropertyField* x) { OnModeSelected(); };

		mHiddenProperties = o2UI.CreateWidget<UIVerticalLayout>();
		mHiddenProperties->expandWidth = true;
		mHiddenProperties->expandHeight = false;
		mHiddenProperties->fitByChildren = true;
		layout->AddChild(mHiddenProperties);

		// Fill properties
		mFillPropertiesSpoiler = o2UI.CreateWidget<UISpoiler>();
		mHiddenProperties->AddChild(mFillPropertiesSpoiler);

		mFillProperty = dynamic_cast<FloatProperty*>(
			o2EditorProperties.BuildField(mFillPropertiesSpoiler, spriteType, "fill", path, mFieldProperties, onChangeCompleted, onChanged));

		auto fillSpace = mnew UIWidget();
		fillSpace->layout->minHeight = 5;
		mFillPropertiesSpoiler->AddChildWidget(fillSpace);

		// Slice properties
		mSlicedPropertiesSpoiler = o2UI.CreateWidget<UISpoiler>();
		mHiddenProperties->AddChild(mSlicedPropertiesSpoiler);

		mSliceBorderProperty = dynamic_cast<BorderIProperty*>(
			o2EditorProperties.BuildField(mSlicedPropertiesSpoiler, spriteType, "sliceBorder", path, mFieldProperties, onChangeCompleted, onChanged));

		// Slice properties
		mTiledPropertiesSpoiler = o2UI.CreateWidget<UISpoiler>();
		mHiddenProperties->AddChild(mTiledPropertiesSpoiler);

		mTileScaleProperty = dynamic_cast<FloatProperty*>(
			o2EditorProperties.BuildField(mTiledPropertiesSpoiler, spriteType, "tileScale", path, mFieldProperties, onChangeCompleted, onChanged));
	}

	void SpriteViewer::Refresh(const TargetsVec& targetObjets, const String& path, const OnChangeCompletedFunc& onChangeCompleted,
							   const OnChangedFunc& onChanged)
	{
		mFieldProperties.Set(targetObjets);
	}

	const Type* SpriteViewer::GetViewingObjectType() const
	{
		return &TypeOf(Sprite);
	}

	void SpriteViewer::OnModeSelected()
	{
		SpriteMode mode = (SpriteMode)(mModeProperty->GetCommonValue());

		mFillPropertiesSpoiler->SetExpanded(mode == SpriteMode::Fill360CCW ||
											mode == SpriteMode::Fill360CW ||
											mode == SpriteMode::FillDownToUp ||
											mode == SpriteMode::FillLeftToRight ||
											mode == SpriteMode::FillRightToLeft ||
											mode == SpriteMode::FillUpToDown ||
											mode == SpriteMode::Sliced);

		mSlicedPropertiesSpoiler->SetExpanded(mode == SpriteMode::Sliced);

		mTiledPropertiesSpoiler->SetExpanded(mode == SpriteMode::Tiled);
	}

}

DECLARE_CLASS(Editor::SpriteViewer);
