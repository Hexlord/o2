#include "UI/VerticalProgress.h"

namespace o2
{
	IOBJECT_CPP(UIVerticalProgress);

	UIVerticalProgress::UIVerticalProgress():
		mValue(0), mMinValue(0), mMaxValue(1), mOrientation(Orientation::Down), mScrollSense(1.0f)
	{
		InitializeProperties();
	}

	UIVerticalProgress::UIVerticalProgress(const UIVerticalProgress& other):
		UIWidget(other), mValue(other.mValue), mMinValue(other.mMinValue), mMaxValue(other.mMaxValue),
		mOrientation(other.mOrientation), mScrollSense(other.mScrollSense)
	{
		mBarLayer = GetLayer("bar");
		mBackLayer = GetLayer("back");

		RetargetStatesAnimations();
		InitializeProperties();
	}

	UIVerticalProgress::~UIVerticalProgress()
	{}

	UIVerticalProgress& UIVerticalProgress::operator=(const UIVerticalProgress& other)
	{
		UIWidget::operator=(other);
		mValue = other.mValue;
		mMinValue = other.mMinValue;
		mMaxValue = other.mMaxValue;
		mOrientation = other.mOrientation;
		mScrollSense = other.mScrollSense;

		RetargetStatesAnimations();
		UpdateLayout();

		return *this;
	}

	void UIVerticalProgress::Update(float dt)
	{
		UIWidget::Update(dt);

		const float threshold = 0.01f;
		const float smoothCoef = 30.0f;

		if (!Math::Equals(mValue, mSmoothValue, threshold))
		{
			mSmoothValue = Math::Clamp(Math::Lerp(mSmoothValue, mValue, dt*smoothCoef), mMinValue, mMaxValue);

			if (Math::Abs(mValue - mSmoothValue) < threshold)
				mSmoothValue = mValue;

			UpdateProgressLayersLayouts();
		}
	}

	void UIVerticalProgress::SetValue(float value)
	{
		mValue = Math::Clamp(value, mMinValue, mMaxValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	void UIVerticalProgress::SetValueForcible(float value)
	{
		mValue = Math::Clamp(value, mMinValue, mMaxValue);
		mSmoothValue = mValue;
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	float UIVerticalProgress::GetValue() const
	{
		return mValue;
	}

	void UIVerticalProgress::SetMinValue(float minValue)
	{
		mMinValue = minValue;
		mValue = Math::Max(mMinValue, mValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	float UIVerticalProgress::GetMinValue() const
	{
		return mMinValue;
	}

	void UIVerticalProgress::SetMaxValue(float maxValue)
	{
		mMaxValue = maxValue;
		mValue = Math::Min(mMaxValue, mValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	float UIVerticalProgress::GetMaxValue() const
	{
		return mMaxValue;
	}

	void UIVerticalProgress::SetValueRange(float minValue, float maxValue)
	{
		mMaxValue = maxValue;
		mValue = Math::Clamp(mValue, mMinValue, mMaxValue);
		UpdateProgressLayersLayouts();

		onChange(value);
	}

	void UIVerticalProgress::SetScrollSense(float coef)
	{
		mScrollSense = coef;
	}

	float UIVerticalProgress::GetScrollSense() const
	{
		return mScrollSense;
	}

	void UIVerticalProgress::SetOrientation(Orientation orientation)
	{
		mOrientation = orientation;
		UpdateLayersLayouts();
	}

	UIVerticalProgress::Orientation UIVerticalProgress::GetOrientation() const
	{
		return mOrientation;
	}

	bool UIVerticalProgress::IsUnderPoint(const Vec2F& point)
	{
		if (mBackLayer)
		{
			return mBackLayer->IsUnderPoint(point);
		}

		return false;
	}

	float UIVerticalProgress::Depth()
	{
		return GetMaxDrawingDepth();
	}

	bool UIVerticalProgress::IsScrollable() const
	{
		return true;
	}

	bool UIVerticalProgress::IsInteractable() const
	{
		return mResVisible && CursorEventsListener::IsInteractable();
	}

	void UIVerticalProgress::OnCursorPressed(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = true;

		GetValueFromCursor(cursor);
	}

	void UIVerticalProgress::OnCursorReleased(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;
	}

	void UIVerticalProgress::OnCursorPressBreak(const Input::Cursor& cursor)
	{
		auto pressedState = state["pressed"];
		if (pressedState)
			*pressedState = false;
	}

	void UIVerticalProgress::OnCursorStillDown(const Input::Cursor& cursor)
	{
		GetValueFromCursor(cursor);
	}

	void UIVerticalProgress::GetValueFromCursor(const Input::Cursor &cursor)
	{
		float height = layout.mAbsoluteRect.Height();
		float d = mMaxValue - mMinValue;
		if (mOrientation == UIVerticalProgress::Orientation::Up)

			SetValue((cursor.mPosition.y - layout.mAbsoluteRect.bottom)/height*d + mMinValue);
		else
			SetValue((height - (cursor.mPosition.y - layout.mAbsoluteRect.bottom))/height*d + mMinValue);
	}

	void UIVerticalProgress::OnCursorEnter(const Input::Cursor& cursor)
	{
		auto selectState = state["select"];
		if (selectState)
			*selectState = true;
	}

	void UIVerticalProgress::OnCursorExit(const Input::Cursor& cursor)
	{
		auto selectState = state["select"];
		if (selectState)
			*selectState = false;
	}

	void UIVerticalProgress::OnScrolled(float scroll)
	{
		SetValue(mValue + scroll*mScrollSense);
	}

	void UIVerticalProgress::UpdateLayout(bool forcible /*= false*/)
	{
		if (layout.mDrivenByParent && !forcible)
		{
			if (mParent)
				mParent->UpdateLayout();

			return;
		}

		RecalculateAbsRect();
		UpdateProgressLayersLayouts();
		UpdateLayersLayouts();

		mChildsAbsRect = layout.mAbsoluteRect;

		for (auto child : mChilds)
			child->UpdateLayout();
	}

	void UIVerticalProgress::UpdateProgressLayersLayouts()
	{
		if (mBarLayer)
		{
			mBarLayer->layout.offsetMin = Vec2F();
			mBarLayer->layout.offsetMax = Vec2F();

			if (mOrientation == Orientation::Down)
			{
				mBarLayer->layout.anchorMin = Vec2F(0, 1.0f - (mSmoothValue - mMinValue)/(mMaxValue - mMinValue));
				mBarLayer->layout.anchorMax = Vec2F(1, 1);
			}
			else if (mOrientation == Orientation::Up)
			{
				mBarLayer->layout.anchorMin = Vec2F(0, 0);
				mBarLayer->layout.anchorMax = Vec2F(1, (mSmoothValue - mMinValue)/(mMaxValue - mMinValue));
			}
		}

		if (mBackLayer)
			mBackLayer->layout = Layout::Both();

		UpdateLayersLayouts();
	}

	void UIVerticalProgress::OnLayerAdded(Ptr<UIWidgetLayer> layer)
	{
		if (layer->name == "back")
			mBackLayer = layer;
		else if (layer->name == "bar")
			mBarLayer = layer;

		UpdateProgressLayersLayouts();
	}

	void UIVerticalProgress::InitializeProperties()
	{
		INITIALIZE_PROPERTY(UIVerticalProgress, value, SetValue, GetValue);
		INITIALIZE_PROPERTY(UIVerticalProgress, minValue, SetMinValue, GetMinValue);
		INITIALIZE_PROPERTY(UIVerticalProgress, maxValue, SetMaxValue, GetMaxValue);
		INITIALIZE_PROPERTY(UIVerticalProgress, scrollSense, SetScrollSense, GetScrollSense);
	}
}