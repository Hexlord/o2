#include "StringProperty.h"

#include "SceneWindow/SceneEditScreen.h"
#include "UI/EditBox.h"
#include "UI/UIManager.h"

namespace Editor
{
	StringProperty::StringProperty(UIWidget* widget /*= nullptr*/)
	{
		if (widget)
			mEditBox = dynamic_cast<UIEditBox*>(widget);
		else
			mEditBox = o2UI.CreateWidget<UIEditBox>("singleline");

		mEditBox->layout.minHeight = 10;
		mEditBox->onChangeCompleted = Function<void(const WString&)>(this, &StringProperty::OnEdited);
	}

	StringProperty::~StringProperty()
	{
		delete mEditBox;
	}

	void StringProperty::Setup(const Vector<void*>& targets, bool isProperty)
	{
		if (isProperty)
		{
			mAssignFunc = [](void* ptr, const String& value) { *((Property<String>*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return ((Property<String>*)(ptr))->Get(); };
		}
		else
		{
			mAssignFunc = [](void* ptr, const String& value) { *((String*)(ptr)) = value; };
			mGetFunc = [](void* ptr) { return *((String*)(ptr)); };
		}

		mValuesPointers = targets;

		Refresh();
	}

	void StringProperty::Refresh()
	{
		if (mValuesPointers.IsEmpty())
			return;

		auto lastCommonValue = mCommonValue;
		auto lastDifferent = mValuesDifferent;

		auto newCommonValue = mGetFunc(mValuesPointers[0]);
		auto newDifferent = false;

		for (int i = 1; i < mValuesPointers.Count(); i++)
		{
			if (newCommonValue != mGetFunc(mValuesPointers[i]))
			{
				newDifferent = true;
				break;
			}
		}

		if (newDifferent)
		{
			if (!lastDifferent)
				SetUnknownValue();
		}
		else if (lastCommonValue != newCommonValue)
			SetCommonValue(newCommonValue);
	}

	UIWidget* StringProperty::GetWidget() const
	{
		return mEditBox;
	}

	String StringProperty::GetCommonValue() const
	{
		return mCommonValue;
	}

	bool StringProperty::IsValuesDifferent() const
	{
		return mValuesDifferent;
	}

	void StringProperty::SetValue(const String& value)
	{
		for (auto ptr : mValuesPointers)
			mAssignFunc(ptr, value);

		SetCommonValue(value);
	}

	void StringProperty::SetUnknownValue()
	{
		mCommonValue = "";
		mValuesDifferent = true;
		mEditBox->text = "--";

		onChanged();
		o2EditorSceneScreen.OnSceneChanged();
	}

	const Type* StringProperty::GetFieldType() const
	{
		return &TypeOf(String);
	}

	void StringProperty::SetCommonValue(const String& value)
	{
		mCommonValue = value;
		mValuesDifferent = false;
		mEditBox->text = value;

		onChanged();
		o2EditorSceneScreen.OnSceneChanged();
	}

	void StringProperty::OnEdited(const WString& data)
	{
		if (mValuesDifferent && data == "--")
			return;

		SetValue(data);
	}
}
 
CLASS_META(Editor::StringProperty)
{
	BASE_CLASS(Editor::IPropertyField);

	PROTECTED_FIELD(mAssignFunc);
	PROTECTED_FIELD(mGetFunc);
	PROTECTED_FIELD(mValuesPointers);
	PROTECTED_FIELD(mCommonValue);
	PROTECTED_FIELD(mValuesDifferent);
	PROTECTED_FIELD(mEditBox);

	PUBLIC_FUNCTION(void, Setup, const Vector<void*>&, bool);
	PUBLIC_FUNCTION(void, Refresh);
	PUBLIC_FUNCTION(UIWidget*, GetWidget);
	PUBLIC_FUNCTION(String, GetCommonValue);
	PUBLIC_FUNCTION(bool, IsValuesDifferent);
	PUBLIC_FUNCTION(void, SetValue, const String&);
	PUBLIC_FUNCTION(void, SetUnknownValue);
	PUBLIC_FUNCTION(const Type*, GetFieldType);
	PROTECTED_FUNCTION(void, SetCommonValue, const String&);
	PROTECTED_FUNCTION(void, OnEdited, const WString&);
}
END_META;
 