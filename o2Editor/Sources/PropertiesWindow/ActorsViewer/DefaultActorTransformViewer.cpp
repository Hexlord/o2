#include "DefaultActorTransformViewer.h"

#include "PropertiesWindow/Properties/FloatProperty.h"
#include "PropertiesWindow/Properties/Vector2FloatProperty.h"
#include "Scene/Actor.h"
#include "Scene/DrawableComponent.h"
#include "UI/Image.h"
#include "UI/Label.h"
#include "UI/UIManager.h"
#include "UI/Widget.h"

namespace Editor
{
	DefaultActorTransformViewer::DefaultActorTransformViewer()
	{
		mNameCaption->text = "Transform";
		mPropertiesLayout->layout.minHeight = 130;

		auto positionIcon = o2UI.CreateImage("ui/UI2_position_icon.png");
		positionIcon->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(5, -5));
		mPropertiesLayout->AddChild(positionIcon);

		mPositionProperty = mnew Vec2FProperty(o2UI.CreateWidget<UIWidget>("colored vector2 property"));
		mPositionProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 20, 12, 20, 5);
		mPropertiesLayout->AddChild(mPositionProperty->GetWidget());

		auto pivotIcon = o2UI.CreateImage("ui/UI2_pivot_icon.png");
		pivotIcon->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(5, -30));
		mPropertiesLayout->AddChild(pivotIcon);

		mPivotProperty = mnew Vec2FProperty(o2UI.CreateWidget<UIWidget>("colored vector2 property"));
		mPivotProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 20, 12, 20, 30);
		mPropertiesLayout->AddChild(mPivotProperty->GetWidget());

		auto sizeIcon = o2UI.CreateImage("ui/UI2_scale_icon.png");
		sizeIcon->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(5, -55));
		mPropertiesLayout->AddChild(sizeIcon);

		mSizeProperty = mnew Vec2FProperty(o2UI.CreateWidget<UIWidget>("colored vector2 property"));
		mSizeProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 20, 12, 20, 55);
		mPropertiesLayout->AddChild(mSizeProperty->GetWidget());

		auto scaleIcon = o2UI.CreateImage("ui/UI2_scale_icon.png");
		scaleIcon->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(5, -80));
		mPropertiesLayout->AddChild(scaleIcon);

		mScaleProperty = mnew Vec2FProperty(o2UI.CreateWidget<UIWidget>("colored vector2 property"));
		mScaleProperty->GetWidget()->layout = UIWidgetLayout::HorStretch(VerAlign::Top, 20, 12, 20, 80);
		mPropertiesLayout->AddChild(mScaleProperty->GetWidget());

		auto rotateIcon = o2UI.CreateImage("ui/UI2_rotate_icon.png");
		rotateIcon->layout = UIWidgetLayout::Based(BaseCorner::LeftTop, Vec2F(20, 20), Vec2F(5, -105));
		mPropertiesLayout->AddChild(rotateIcon);

		mRotationProperty = mnew FloatProperty();
		mRotationProperty->GetWidget()->layout = UIWidgetLayout(Vec2F(0, 1), Vec2F(0.5f, 1), Vec2F(40, -125), Vec2F(4, -105));
		mPropertiesLayout->AddChild(mRotationProperty->GetWidget());

		auto depthIcon = o2UI.CreateImage("ui/UI2_layer_icon_t.png");
		depthIcon->layout = UIWidgetLayout::Based(BaseCorner::Top, Vec2F(20, 20), Vec2F(15, -105));
		mPropertiesLayout->AddChild(depthIcon);

		mDepthProperty = mnew FloatProperty();
		mDepthProperty->GetWidget()->layout = UIWidgetLayout(Vec2F(0.5f, 1), Vec2F(1, 1), Vec2F(23, -125), Vec2F(-12, -105));
		mPropertiesLayout->AddChild(mDepthProperty->GetWidget());
	}

	DefaultActorTransformViewer::~DefaultActorTransformViewer()
	{}

	void DefaultActorTransformViewer::SetTargetActors(const Vector<Actor*>& actors)
	{
		mTargetActors = actors;
		Update();
	}

	void DefaultActorTransformViewer::Refresh()
	{
		Update();
	}

	void DefaultActorTransformViewer::Update()
	{
		Vector<void*> positionTargets = mTargetActors.Select<void*>([](Actor* x) { return &x->transform.position; });
		mPositionProperty->Setup(positionTargets, true);

		Vector<void*> pivotTargets = mTargetActors.Select<void*>([](Actor* x) { return &x->transform.pivot; });
		mPivotProperty->Setup(pivotTargets, true);

		Vector<void*> scaleTargets = mTargetActors.Select<void*>([](Actor* x) { return &x->transform.scale; });
		mScaleProperty->Setup(scaleTargets, true);

		Vector<void*> sizeTargets = mTargetActors.Select<void*>([](Actor* x) { return &x->transform.size; });
		mSizeProperty->Setup(sizeTargets, true);

		Vector<void*> rotateTargets = mTargetActors.Select<void*>([](Actor* x) { return &x->transform.angleDegree; });
		mRotationProperty->Setup(rotateTargets, true);

		Vector<void*> depthTargets;
		for (auto actor : mTargetActors)
		{
			if (auto comp = actor->GetComponent<DrawableComponent>())
				depthTargets.Add(&comp->drawDepth);
		}

		mDepthProperty->Setup(depthTargets, true);
	}

}
 
CLASS_META(Editor::DefaultActorTransformViewer)
{
	BASE_CLASS(Editor::IActorTransformViewer);

	PROTECTED_FIELD(mTargetActors);
	PROTECTED_FIELD(mPositionProperty);
	PROTECTED_FIELD(mPivotProperty);
	PROTECTED_FIELD(mScaleProperty);
	PROTECTED_FIELD(mSizeProperty);
	PROTECTED_FIELD(mRotationProperty);
	PROTECTED_FIELD(mDepthProperty);

	PUBLIC_FUNCTION(void, SetTargetActors, const Vector<Actor*>&);
	PUBLIC_FUNCTION(void, Refresh);
	PROTECTED_FUNCTION(void, Update);
}
END_META;
 