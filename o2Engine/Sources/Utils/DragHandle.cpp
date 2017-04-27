#include "DragHandle.h"

#include "Render/Sprite.h"

namespace o2
{
	DragHandle::DragHandle()
	{
		screenToLocalTransformFunc = [](const Vec2F& point) { return point; };
		localToScreenTransformFunc = [](const Vec2F& point) { return point; };
		checkPositionFunc = [](const Vec2F& point) { return point; };

		InitializeProperties();
	}

	DragHandle::DragHandle(Sprite* regular, Sprite* hover /*= nullptr*/, Sprite* pressed /*= nullptr*/,
						   Sprite* selected /*= nullptr*/):
		mRegularSprite(regular), mHoverSprite(hover), mPressedSprite(pressed), mSelectedSprite(selected)
	{
		screenToLocalTransformFunc = [](const Vec2F& point) { return point; };
		localToScreenTransformFunc = [](const Vec2F& point) { return point; };
		checkPositionFunc = [](const Vec2F& point) { return point; };

		InitializeProperties();
	}

	DragHandle::DragHandle(const DragHandle& other)
	{
		if (other.mRegularSprite)
			mRegularSprite = other.mRegularSprite->Clone();

		if (other.mHoverSprite)
			mHoverSprite = other.mHoverSprite->Clone();

		if (other.mPressedSprite)
			mPressedSprite = other.mPressedSprite->Clone();

		if (other.mSelectedSprite)
			mSelectedSprite = other.mSelectedSprite->Clone();

		onChangedPos = other.onChangedPos;
		screenToLocalTransformFunc = other.screenToLocalTransformFunc;
		localToScreenTransformFunc = other.localToScreenTransformFunc;
		checkPositionFunc = other.checkPositionFunc;

		SetPosition(other.mPosition);
		InitializeProperties();
	}

	DragHandle::~DragHandle()
	{
		delete mRegularSprite;
		delete mHoverSprite;
		delete mPressedSprite;
		delete mSelectedSprite;
	}

	DragHandle& DragHandle::operator=(const DragHandle& other)
	{
		delete mRegularSprite;
		delete mHoverSprite;
		delete mPressedSprite;
		delete mSelectedSprite;

		if (other.mRegularSprite)
			mRegularSprite = other.mRegularSprite->Clone();

		if (other.mHoverSprite)
			mHoverSprite = other.mHoverSprite->Clone();

		if (other.mPressedSprite)
			mPressedSprite = other.mPressedSprite->Clone();

		if (other.mSelectedSprite)
			mSelectedSprite = other.mSelectedSprite->Clone();

		onChangedPos = other.onChangedPos;
		screenToLocalTransformFunc = other.screenToLocalTransformFunc;
		localToScreenTransformFunc = other.localToScreenTransformFunc;
		checkPositionFunc = other.checkPositionFunc;

		SetPosition(other.mPosition);

		return *this;
	}

	void DragHandle::Draw()
	{
		if (!mEnabled)
			return;

		Vec2F screenPosition = localToScreenTransformFunc(mPosition);
		float alphaChangeCoef = 15.0f;

		if (mRegularSprite)
		{
			mRegularSprite->SetPosition(screenPosition);
			mRegularSprite->Draw();
		}

		if (mHoverSprite)
		{
			mHoverSprite->SetTransparency(Math::Lerp(mHoverSprite->GetTransparency(), mIsHovered ? 1.0f : 0.0f,
										  o2Time.GetDeltaTime()*alphaChangeCoef));

			mHoverSprite->SetPosition(screenPosition);
			mHoverSprite->Draw();
		}

		if (mSelectedSprite)
		{
			mSelectedSprite->SetTransparency(Math::Lerp(mSelectedSprite->GetTransparency(), mIsSelected ? 1.0f : 0.0f,
											 o2Time.GetDeltaTime()*alphaChangeCoef));

			mSelectedSprite->SetPosition(screenPosition);
			mSelectedSprite->Draw();
		}

		if (mPressedSprite)
		{
			mPressedSprite->SetTransparency(Math::Lerp(mPressedSprite->GetTransparency(), mIsPressed ? 1.0f : 0.0f,
											o2Time.GetDeltaTime()*alphaChangeCoef));

			mPressedSprite->SetPosition(screenPosition);
			mPressedSprite->Draw();
		}

		CursorAreaEventsListener::OnDrawn();
		IDrawable::OnDrawn();
	}

	bool DragHandle::IsUnderPoint(const Vec2F& point)
	{
		if (mRegularSprite)
			return mDrawingScissorRect.IsInside(point) && mRegularSprite->IsPointInside(point);

		return false;
	}

	void DragHandle::OnCursorPressed(const Input::Cursor& cursor)
	{
		mIsPressed = true;
		mDragOffset = mPosition - screenToLocalTransformFunc(cursor.position);
		mDragPosition = mPosition;
	}

	void DragHandle::OnCursorReleased(const Input::Cursor& cursor)
	{
		mIsPressed = false;
	}

	void DragHandle::OnCursorPressBreak(const Input::Cursor& cursor)
	{
		mIsPressed = false;
	}

	void DragHandle::OnCursorStillDown(const Input::Cursor& cursor)
	{
		if (mIsPressed && cursor.delta != Vec2F())
		{
			mDragPosition = screenToLocalTransformFunc(cursor.position) + mDragOffset;

			SetPosition(mDragPosition);
			onChangedPos(mPosition);
		}
	}

	void DragHandle::OnCursorEnter(const Input::Cursor& cursor)
	{
		mIsHovered = true;
	}

	void DragHandle::OnCursorExit(const Input::Cursor& cursor)
	{
		mIsHovered = false;
	}

	void DragHandle::SetPosition(const Vec2F& position)
	{
		mPosition = checkPositionFunc(position);
	}

	bool DragHandle::IsSelected() const
	{
		return mIsSelected;
	}

	void DragHandle::SetSelected(bool selected)
	{
		mIsSelected = selected;
	}

	Vec2F DragHandle::GetPosition() const
	{
		return mPosition;
	}

	void DragHandle::SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	bool DragHandle::IsEnabled() const
	{
		return mEnabled;
	}

	void DragHandle::InitializeProperties()
	{
		INITIALIZE_PROPERTY(DragHandle, position, SetPosition, GetPosition);
		INITIALIZE_PROPERTY(DragHandle, enabled, SetEnabled, IsEnabled);
	}
}

CLASS_META(o2::DragHandle)
{
	BASE_CLASS(o2::IDrawable);
	BASE_CLASS(o2::CursorAreaEventsListener);
	BASE_CLASS(o2::ISerializable);

	PUBLIC_FIELD(onChangedPos);
	PUBLIC_FIELD(position);
	PUBLIC_FIELD(enabled);
	PUBLIC_FIELD(screenToLocalTransformFunc);
	PUBLIC_FIELD(localToScreenTransformFunc);
	PUBLIC_FIELD(checkPositionFunc);
	PROTECTED_FIELD(mEnabled);
	PROTECTED_FIELD(mRegularSprite).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mHoverSprite).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mPressedSprite).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mSelectedSprite).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mPosition);
	PROTECTED_FIELD(mDragOffset);
	PROTECTED_FIELD(mDragPosition);
	PROTECTED_FIELD(mIsHovered);
	PROTECTED_FIELD(mIsPressed);
	PROTECTED_FIELD(mIsSelected);

	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(bool, IsUnderPoint, const Vec2F&);
	PUBLIC_FUNCTION(void, SetPosition, const Vec2F&);
	PUBLIC_FUNCTION(bool, IsSelected);
	PUBLIC_FUNCTION(void, SetSelected, bool);
	PUBLIC_FUNCTION(Vec2F, GetPosition);
	PUBLIC_FUNCTION(void, SetEnabled, bool);
	PUBLIC_FUNCTION(bool, IsEnabled);
	PROTECTED_FUNCTION(void, OnCursorPressed, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorReleased, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorPressBreak, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorStillDown, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorEnter, const Input::Cursor&);
	PROTECTED_FUNCTION(void, OnCursorExit, const Input::Cursor&);
	PROTECTED_FUNCTION(void, InitializeProperties);
}
END_META;
 