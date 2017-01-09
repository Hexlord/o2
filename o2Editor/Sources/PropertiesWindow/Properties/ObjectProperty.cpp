#include "ObjectProperty.h"

#include "PropertiesWindow/PropertiesWindow.h"
#include "UI/Button.h"
#include "UI/Label.h"
#include "UI/UIManager.h"
#include "UI/VerticalLayout.h"
#include "UI/Spoiler.h"

using namespace o2;

namespace Editor
{
	ObjectProperty::ObjectProperty()
	{
		mLayout = mnew UIVerticalLayout();
		mLayout->spacing = 0.0f;
		mLayout->border = RectF(0, 0, 0, 0);
		mLayout->expandHeight = false;
		mLayout->expandWidth = true;
		mLayout->fitByChildren = true;
		mLayout->baseCorner = BaseCorner::RightTop;
		mLayout->layout = UIWidgetLayout::BothStretch();

		auto header = mnew UIWidget();
		header->layout = UIWidgetLayout::BothStretch();
		header->layout.minHeight = 20;

		mExpandButton = o2UI.CreateWidget<UIButton>("expand");
		mExpandButton->layout = UIWidgetLayout::Based(BaseCorner::Left, Vec2F(20, 20), Vec2F(-7, 0));
		mExpandButton->onClick = [&]() { SetExpanded(!IsExpanded()); };
		header->AddChild(mExpandButton, false);

		mNameLabel = o2UI.CreateWidget<UILabel>();
		mNameLabel->layout = UIWidgetLayout::HorStretch(VerAlign::Middle, 10, 0, 20, 0);
		mNameLabel->horAlign = HorAlign::Left;
		header->AddChild(mNameLabel, false);

		mSpoiler = mnew UISpoiler();
		mPropertiesLayout = mnew UIVerticalLayout();
		mPropertiesLayout->spacing = 5.0f;
		mPropertiesLayout->borderLeft = 10;
		mPropertiesLayout->expandHeight = false;
		mPropertiesLayout->expandWidth = true;
		mPropertiesLayout->fitByChildren = true;
		mPropertiesLayout->baseCorner = BaseCorner::RightTop;
		mPropertiesLayout->layout = UIWidgetLayout::BothStretch();
		mSpoiler->AddChild(mPropertiesLayout);

		mLayout->AddChild(header, false);
		mLayout->AddChild(mSpoiler, false);
	}

	ObjectProperty::~ObjectProperty()
	{
		delete mLayout;
	}

	void ObjectProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mTargetObjects.Clear();
			return;
		}

		mTargetObjects = targets.Select<IObject*>([](auto x) { return (IObject*)x; });
		Refresh();
	}

	void ObjectProperty::Refresh()
	{
		mFieldProperties.Set(mTargetObjects);
	}

	UIWidget* ObjectProperty::GetWidget() const
	{
		return mLayout;
	}

	const Type* ObjectProperty::GetFieldType() const
	{
		return &TypeOf(IObject);
	}

	void ObjectProperty::SpecializeType(const Type* type)
	{
		mObjectType = type;

		o2EditorProperties.BuildTypeViewer(mPropertiesLayout, mObjectType, mFieldProperties);

		auto onChangedProperty = [&]() { onChanged(); };
		for (auto prop : mFieldProperties.properties)
			prop.Value()->onChanged += onChangedProperty;
	}

	void ObjectProperty::Expand()
	{
		SetExpanded(true);
	}

	void ObjectProperty::Collapse()
	{
		SetExpanded(false);
	}

	void ObjectProperty::SetExpanded(bool expanded)
	{
		if (!expanded)
		{
			mSpoiler->Collapse();
			mExpandButton->SetState("expanded", false);
		}
		else
		{
			mSpoiler->Expand();
			mExpandButton->SetState("expanded", true);
		}
	}

	bool ObjectProperty::IsExpanded() const
	{
		return mSpoiler->IsExpanded();
	}

}
 
CLASS_META(Editor::ObjectProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mObjectType);
	PROTECTED_FIELD(mTargetObjects);
	PROTECTED_FIELD(mFieldProperties);
	PROTECTED_FIELD(mLayout);
	PROTECTED_FIELD(mNameLabel);
	PROTECTED_FIELD(mExpandButton);
	PROTECTED_FIELD(mSpoiler);
	PROTECTED_FIELD(mPropertiesLayout);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Refresh);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PUBLIC_FUNCTION(void, SpecializeType, const Type*);
	PUBLIC_FUNCTION(void, Expand);
	PUBLIC_FUNCTION(void, Collapse);
	PUBLIC_FUNCTION(void, SetExpanded, bool);
	PUBLIC_FUNCTION(bool, IsExpanded);
}
END_META;
 