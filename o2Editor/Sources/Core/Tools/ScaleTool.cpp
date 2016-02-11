#include "ScaleTool.h"

#include "Core/Actions/ActorsTransform.h"
#include "Core/EditorApplication.h"
#include "Render/Render.h"
#include "Render/Sprite.h"
#include "Scene/Actor.h"
#include "SceneWindow/SceneEditScreen.h"

EditorScaleTool::EditorScaleTool():
	mHorDragHandle(mnew Sprite("ui/UI2_right_scale_arrow.png"),
				   mnew Sprite("ui/UI2_right_scale_arrow_select.png"),
				   mnew Sprite("ui/UI2_right_scale_arrow_pressed.png")),
	mVerDragHandle(mnew Sprite("ui/UI2_up_scale_arrow.png"),
				   mnew Sprite("ui/UI2_up_scale_arrow_select.png"),
				   mnew Sprite("ui/UI2_up_scale_arrow_pressed.png")),
	mBothDragHandle(mnew Sprite("ui/UI2_scale_both.png"),
					mnew Sprite("ui/UI2_scale_both_select.png"),
					mnew Sprite("ui/UI2_scale_both_pressed.png"))
{
	mHorDragHandle.enabled = false;
	mVerDragHandle.enabled = false;
	mBothDragHandle.enabled = false;

	mHorDragHandle.onChangedPos = Function<void(const Vec2F&)>(this, &EditorScaleTool::OnHorDragHandleMoved);
	mVerDragHandle.onChangedPos = Function<void(const Vec2F&)>(this, &EditorScaleTool::OnVerDragHandleMoved);
	mBothDragHandle.onChangedPos = Function<void(const Vec2F&)>(this, &EditorScaleTool::OnBothDragHandleMoved);

	mHorDragHandle.onPressed = Function<void()>(this, &EditorScaleTool::HandlePressed);
	mVerDragHandle.onPressed = Function<void()>(this, &EditorScaleTool::HandlePressed);
	mBothDragHandle.onPressed = Function<void()>(this, &EditorScaleTool::HandlePressed);

	mHorDragHandle.onReleased = Function<void()>(this, &EditorScaleTool::UpdateHandlesPosition);
	mVerDragHandle.onReleased = Function<void()>(this, &EditorScaleTool::UpdateHandlesPosition);

	mHorDragHandle.onReleased += Function<void()>(this, &EditorScaleTool::HandleReleased);
	mVerDragHandle.onReleased += Function<void()>(this, &EditorScaleTool::HandleReleased);
	mBothDragHandle.onReleased += Function<void()>(this, &EditorScaleTool::HandleReleased);
}

EditorScaleTool::~EditorScaleTool()
{}

void EditorScaleTool::Update(float dt)
{}

void EditorScaleTool::DrawScreen()
{
	EditorSelectionTool::DrawScreen();

	if (!mHorDragHandle.IsPressed() && !mVerDragHandle.IsPressed() && !mBothDragHandle.IsPressed())
		UpdateHandlesPositions();

	Vec2F screenHandlesPos = o2EditorSceneScreen.SceneToScreenPoint(mSceneHandlesPos);
	Vec2F screenHorHandlePos = o2EditorSceneScreen.SceneToScreenPoint(mHorDragHandle.GetPosition());
	Vec2F screenVerHandlePos = o2EditorSceneScreen.SceneToScreenPoint(mVerDragHandle.GetPosition());
	o2Render.DrawLine(screenHandlesPos, screenHorHandlePos, Color4::Green());
	o2Render.DrawLine(screenHandlesPos, screenVerHandlePos, Color4::Red());
}

void EditorScaleTool::OnEnabled()
{
	mHorDragHandle.enabled = true;
	mVerDragHandle.enabled = true;
	mBothDragHandle.enabled = true;
	UpdateHandlesPosition();
}

void EditorScaleTool::OnDisabled()
{
	mHorDragHandle.enabled = false;
	mVerDragHandle.enabled = false;
	mBothDragHandle.enabled = false;
}

void EditorScaleTool::OnSceneChanged(Vector<Actor*> changedActors)
{}

void EditorScaleTool::OnActorsSelectionChanged(Vector<Actor*> actors)
{
	UpdateHandlesPosition();
}

void EditorScaleTool::OnHorDragHandleMoved(const Vec2F& position)
{
	Vec2F axis = Vec2F::Rotated(mHandlesAngle);
	Vec2F handlePos = (position - mSceneHandlesPos).Project(axis) + mSceneHandlesPos;
	float scale = (handlePos - mSceneHandlesPos).Length() / (mLastHorHandlePos - mSceneHandlesPos).Length();

	mLastHorHandlePos = handlePos;
	mHorDragHandle.position = handlePos;

	ScaleSelectedActors(Vec2F(scale, 1.0f));
}

void EditorScaleTool::OnVerDragHandleMoved(const Vec2F& position)
{
	Vec2F axis = Vec2F::Rotated(mHandlesAngle).Perpendicular();
	Vec2F handlePos = (position - mSceneHandlesPos).Project(axis) + mSceneHandlesPos;
	float scale = (handlePos - mSceneHandlesPos).Length() / (mLastVerHandlePos - mSceneHandlesPos).Length();

	mLastVerHandlePos = handlePos;
	mVerDragHandle.position = handlePos;

	ScaleSelectedActors(Vec2F(1.0f, scale));
}

void EditorScaleTool::OnBothDragHandleMoved(const Vec2F& position)
{
	float delta = o2EditorSceneScreen.SceneToScreenPoint(position).x -
		o2EditorSceneScreen.SceneToScreenPoint(mLastBothHandlePos).x;

	float scale = 1.0f + delta*bothScaleSence;
	mLastBothHandlePos = position;

	mBothDragHandle.position = mSceneHandlesPos;

	ScaleSelectedActors(Vec2F(scale, scale));
}

void EditorScaleTool::UpdateHandlesPosition()
{
	auto selectedActors = o2EditorSceneScreen.GetSelectedActors();
	mSceneHandlesPos =
		selectedActors.Sum<Vec2F>([](auto x) { return x->transform.GetWorldPosition(); }) /
		(float)selectedActors.Count();

	if (selectedActors.Count() > 0 && !o2Input.IsKeyDown(VK_CONTROL))
	{
		Actor* lastSelectedActor = selectedActors.Last();
		UpdateHandlesAngleAndPositions(-lastSelectedActor->transform.right->Angle(Vec2F::Right()));
	}
	else UpdateHandlesAngleAndPositions(0.0f);
}

void EditorScaleTool::UpdateHandlesAngleAndPositions(float angle)
{
	mHandlesAngle = angle;

	mVerDragHandle.angle = mHandlesAngle;
	mHorDragHandle.angle = mHandlesAngle;
	mBothDragHandle.angle = mHandlesAngle;

	UpdateHandlesPositions();
}

void EditorScaleTool::UpdateHandlesPositions()
{
	Vec2F handlesAxis = Vec2F::Rotated(mHandlesAngle);
	Vec2F handlesSceneSize = o2EditorSceneScreen.ScreenToScenePoint(mHandlesSize) -
		o2EditorSceneScreen.ScreenToScenePoint(Vec2F());

	mVerDragHandle.position = mSceneHandlesPos + handlesAxis.Perpendicular()*handlesSceneSize.y;
	mHorDragHandle.position = mSceneHandlesPos + handlesAxis*handlesSceneSize.x;
	mBothDragHandle.position = mSceneHandlesPos;

	mLastVerHandlePos = mVerDragHandle.position;
	mLastHorHandlePos = mHorDragHandle.position;
	mLastBothHandlePos = mBothDragHandle.position;
}

void EditorScaleTool::OnKeyPressed(const Input::Key& key)
{
	if (key == VK_CONTROL)
		UpdateHandlesAngleAndPositions(0.0f);

	EditorSelectionTool::OnKeyPressed(key);
}

void EditorScaleTool::OnKeyStayDown(const Input::Key& key)
{}

void EditorScaleTool::OnKeyReleased(const Input::Key& key)
{
	if (key == VK_CONTROL)
	{
		auto selectedActors = o2EditorSceneScreen.GetSelectedActors();
		if (selectedActors.Count() > 0)
		{
			Actor* lastSelectedActor = selectedActors.Last();
			UpdateHandlesAngleAndPositions(-lastSelectedActor->transform.right->Angle(Vec2F::Right()));
		}
	}
}

void EditorScaleTool::ScaleSelectedActors(const Vec2F& scale)
{
	Basis transform =
		Basis::Translated(mSceneHandlesPos*-1.0f)*
		Basis::Rotated(-mHandlesAngle)*
		Basis::Scaled(scale)*
		Basis::Rotated(mHandlesAngle)*
		Basis::Translated(mSceneHandlesPos);

	for (auto actor : o2EditorSceneScreen.GetTopSelectedActors())
		actor->transform.SetWorldNonSizedBasis(actor->transform.GetWorldNonSizedBasis()*transform);
}

void EditorScaleTool::HandlePressed()
{
	mBeforeTransforms = o2EditorSceneScreen.GetTopSelectedActors().Select<ActorTransform>(
		[](Actor* x) { return x->transform; });
}

void EditorScaleTool::HandleReleased()
{
	auto action = mnew EditorActorsTransformAction(o2EditorSceneScreen.GetTopSelectedActors(), mBeforeTransforms);
	o2EditorApplication.DoneAction(action);
}