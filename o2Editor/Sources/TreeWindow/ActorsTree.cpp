#include "ActorsTree.h"

#include "Assets/ActorAsset.h"
#include "Assets/Assets.h"
#include "Assets/FolderAsset.h"
#include "AssetsWindow/AssetsIconsScroll.h"
#include "AssetsWindow/UIAssetIcon.h"
#include "Core/Actions/EnableAction.h"
#include "Core/Actions/LockAction.h"
#include "Core/Actions/ReparentActors.h"
#include "Core/EditorApplication.h"
#include "Events/EventSystem.h"
#include "PropertiesWindow/Properties/ActorProperty.h"
#include "PropertiesWindow/Properties/ComponentProperty.h"
#include "Scene/Actor.h"
#include "Scene/Scene.h"
#include "UI/Button.h"
#include "UI/EditBox.h"
#include "UI/Toggle.h"

namespace Editor
{
	UIActorsTree::UIActorsTree():
		UITree(), mAttackedToSceneEvents(false), mDragActorPropertyField(nullptr), mDragComponentPropertyField(nullptr)
	{
		delete mNodeWidgetSample;
		mNodeWidgetSample = mnew UIActorsTreeNode();
		mNodeWidgetSample->layout.minHeight = 20;
		mNodeWidgetSample->AddLayer("caption", nullptr);

		Initialize();
	}

	UIActorsTree::UIActorsTree(const UIActorsTree& other):
		UITree(other), mAttackedToSceneEvents(false), mDragActorPropertyField(nullptr), mDragComponentPropertyField(nullptr)
	{
		Initialize();
	}

	UIActorsTree::~UIActorsTree()
	{
		DeattachFromSceneEvents();
	}

	UIActorsTree& UIActorsTree::operator=(const UIActorsTree& other)
	{
		UITree::operator=(other);
		return *this;
	}

	void UIActorsTree::AttachToSceneEvents()
	{
		auto updateActorTreeNode = Function<void(Actor*)>(this, &UIActorsTree::OnActorChanged);

		o2Scene.onActorCreated += Function<void(Actor*)>(this, &UIActorsTree::OnActorCreated);
		o2Scene.onActorDestroying += Function<void(Actor*)>(this, &UIActorsTree::OnActorDestroyed);

		o2Scene.onActorEnableChanged += updateActorTreeNode;
		o2Scene.onActorLockChanged += updateActorTreeNode;
		o2Scene.onActorNameChanged += updateActorTreeNode;
		o2Scene.onActorChildsHierarchyChanged += updateActorTreeNode;

		mAttackedToSceneEvents = true;
	}

	void UIActorsTree::DeattachFromSceneEvents()
	{
		if (Scene::IsSingletonInitialzed())
		{
			o2Scene.onActorCreated -= Function<void(Actor*)>(this, &UIActorsTree::OnActorCreated);
			o2Scene.onActorDestroying -= Function<void(Actor*)>(this, &UIActorsTree::OnActorDestroyed);

			if (mAttackedToSceneEvents)
			{
				auto updateActorTreeNode = Function<void(Actor*)>(this, &UIActorsTree::OnActorChanged);

				o2Scene.onActorCreated -= Function<void(Actor*)>(this, &UIActorsTree::OnActorCreated);
				o2Scene.onActorDestroying -= Function<void(Actor*)>(this, &UIActorsTree::OnActorDestroyed);

				o2Scene.onActorEnableChanged -= updateActorTreeNode;
				o2Scene.onActorLockChanged -= updateActorTreeNode;
				o2Scene.onActorNameChanged -= updateActorTreeNode;
				o2Scene.onActorChildsHierarchyChanged -= updateActorTreeNode;
			}
		}
	}

	UITreeNode* UIActorsTree::GetNode(Actor* object)
	{
		return UITree::GetNode((UnknownPtr)(void*)object);
	}

	void UIActorsTree::ManualBeginDraggingActors(const ActorsVec& actors)
	{
		mIsDraggingNodes = true;
		SetSelectedActors(actors);

		mDragOffset = Vec2F();
		mFakeDragNode->Show(true);

		setupNodeFunc(mFakeDragNode, mSelectedNodes.Last()->object);

		if (mSelectedNodes.Count() > 1)
		{
			if (auto nameLayer = mFakeDragNode->FindLayer<Text>())
				nameLayer->text = String::Format("%i items", mSelectedNodes.Count());
		}

		for (auto sel : mSelectedNodes)
		{
			if (sel->widget)
				sel->widget->SetState("dragging", true);
		}
	}

	void UIActorsTree::ManualUpdateDraggingActors(const Input::Cursor& cursor)
	{
		//UpdateDragging(cursor);
	}

	void UIActorsTree::CompleteManualDraggingActors()
	{}

	void UIActorsTree::BreakDragging()
	{
		mIsDraggingNodes = false;
		mFakeDragNode->Hide(true);

		for (auto node : mVisibleNodes)
			node->widget->SetState("inserting", false);

		for (auto sel : mSelectedNodes)
		{
			if (sel->widget)
				sel->widget->SetState("dragging", false);
		}

		DeselectAllActors();
	}

	UIActorsTree::ActorsVec UIActorsTree::GetSelectedActors() const
	{
		return UITree::GetSelectedObjects().Select<Actor*>([](auto x) { return (Actor*)(void*)x; });
	}

	void UIActorsTree::SetSelectedActors(const ActorsVec& actors)
	{
		UITree::SetSelectedObjects(actors.Select<UnknownPtr>([](auto x) { return (UnknownPtr)(void*)x; }));
	}

	void UIActorsTree::SelectActor(Actor* object)
	{
		UITree::SelectObject((UnknownPtr)(void*)object);
	}

	void UIActorsTree::SelectAndHightlightActor(Actor* object)
	{
		o2Debug.Log("%i: Hightligh actor:%s %i", o2Time.GetCurrentFrame(), object->GetName(), 
					mAllNodes.FindIdx([=](Node* x) { return x->object == object; }));

		UITree::SelectAndHightlightObject((UnknownPtr)(void*)object);
	}

	void UIActorsTree::DeselectActor(Actor* object)
	{
		UITree::DeselectObject((UnknownPtr)(void*)object);
	}

	void UIActorsTree::DeselectAllActors()
	{
		UITree::DeselectAllObjects();
	}

	void UIActorsTree::ScrollTo(Actor* object)
	{
		UITree::ScrollTo((UnknownPtr)(void*)object);
	}

	void UIActorsTree::Initialize()
	{
		mEnableActorsTogglesGroup = mnew UIToggleGroup(UIToggleGroup::Type::VerOneClick);
		mEnableActorsTogglesGroup->onReleased = Function<void(bool)>(this, &UIActorsTree::EnableActorsGroupReleased);

		mLockActorsTogglesGroup = mnew UIToggleGroup(UIToggleGroup::Type::VerOneClick);
		mLockActorsTogglesGroup->onReleased = Function<void(bool)>(this, &UIActorsTree::LockActorsGroupReleased);

		getParentFunc    = Function<UnknownPtr(UnknownPtr)>(this, &UIActorsTree::GetActorsParent);
		getChildsFunc    = Function<Vector<UnknownPtr>(UnknownPtr)>(this, &UIActorsTree::GetActorsChildren);
		setupNodeFunc    = Function<void(UITreeNode*, UnknownPtr)>(this, &UIActorsTree::SetupTreeNodeActor);
		onDraggedObjects = Function<void(Vector<UnknownPtr>, UnknownPtr, UnknownPtr)>(this, &UIActorsTree::RearrangeActors);
		onItemDblClick   = Function<void(UITreeNode*, Actor*)>(this, &UIActorsTree::OnTreeNodeDblClick);

		UIActorsTreeNode* actorNodeWidgetSample = (UIActorsTreeNode*)mNodeWidgetSample;
		actorNodeWidgetSample->InitializeControls();

		UITree::onItemRBClick = [&](UITreeNode* x) {
			Actor* actor = x ? (Actor*)x->GetObject() : nullptr;
			onItemRBClick(x, actor);
		};

		UITree::onItemDblClick = [&](UITreeNode* x) {
			Actor* actor = x ? (Actor*)x->GetObject() : nullptr;
			onItemDblClick(x, actor);
		};

		UITree::onItemsSelectionChanged = [&](Vector<UnknownPtr> x) {
			onItemsSelectionChanged(x.Select<Actor*>([](auto x) { return (Actor*)(void*)x; })); 
		};
	}

	UnknownPtr UIActorsTree::GetActorsParent(UnknownPtr obj)
	{
		Actor* actor = obj;
		return actor->GetParent();
	}

	Vector<UnknownPtr> UIActorsTree::GetActorsChildren(UnknownPtr parentObj)
	{
		if (parentObj)
		{
			Actor* parent = parentObj;
			return parent->GetChilds().Cast<UnknownPtr>();
		}

		return o2Scene.GetRootActors().Cast<UnknownPtr>();
	}

	void UIActorsTree::SetupTreeNodeActor(UITreeNode* node, UnknownPtr actorObj)
	{
		UIActorsTreeNode* actorNode = (UIActorsTreeNode*)node;
		actorNode->SetActor(actorObj);
		actorNode->mLockToggle->SetToggleGroup(mLockActorsTogglesGroup);
		actorNode->mEnableToggle->SetToggleGroup(mEnableActorsTogglesGroup);
	}

	void UIActorsTree::RearrangeActors(Vector<UnknownPtr> objects, UnknownPtr parentObj, UnknownPtr prevObj)
	{
		Actor* parent = parentObj;
		Actor* prevActor = prevObj;
		Vector<Actor*> actors = objects.Cast<Actor*>();

		auto action = mnew ReparentActorsAction(actors);

		o2Scene.ReparentActors(actors, parent, prevActor);

		action->ActorsReparented(parent, prevActor);
		o2EditorApplication.DoneAction(action);
	}

	void UIActorsTree::OnTreeNodeDblClick(UITreeNode* node, Actor* actor)
	{
		((UIActorsTreeNode*)node)->EnableEditName();
	}

	void UIActorsTree::EnableActorsGroupPressed(bool value)
	{}

	void UIActorsTree::EnableActorsGroupReleased(bool value)
	{
		ActorsVec actors = mEnableActorsTogglesGroup->GetToggled().Select<Actor*>(
			[](UIToggle* x) { return ((UITreeNode*)x->GetParent())->GetObject(); });

		auto action = mnew EnableAction(actors, value);
		o2EditorApplication.DoneAction(action);
	}

	void UIActorsTree::LockActorsGroupPressed(bool value)
	{}

	void UIActorsTree::LockActorsGroupReleased(bool value)
	{
		ActorsVec actors = mLockActorsTogglesGroup->GetToggled().Select<Actor*>(
			[](UIToggle* x) { return ((UITreeNode*)x->GetParent())->GetObject(); });

		auto action = mnew LockAction(actors, value);
		o2EditorApplication.DoneAction(action);
	}

	void UIActorsTree::OnActorCreated(Actor* actor)
	{
		UITree::OnObjectCreated(actor, actor->GetParent());
	}

	void UIActorsTree::OnActorDestroyed(Actor* actor)
	{
		UITree::OnObjectRemoved(actor);
	}

	void UIActorsTree::OnActorChanged(Actor* actor)
	{
		UITree::OnObjectsChanged({ actor });
	}

		// 	void UIActorsTree::OnDraggedAbove(SelectableDragableObjectsGroup* group)
	// 	{
	// 		UITree::OnDraggedAbove(group);
	// 
	// 		auto listenerUnderCursor = o2Events.GetCursorListenerUnderCursor(0);
	// 		ActorProperty* actorProperty = dynamic_cast<ActorProperty*>(listenerUnderCursor);
	// 		ComponentProperty* componentProperty = dynamic_cast<ComponentProperty*>(listenerUnderCursor);
	// 
	// 		if (actorProperty && !mDragActorPropertyField)
	// 		{
	// 			if (mSelectedItems.Count() == 1)
	// 			{
	// 				mDragActorPropertyField = actorProperty;
	// 				mDragActorPropertyField->GetWidget()->SetState("selected", true);
	// 				o2Application.SetCursor(CursorType::Hand);
	// 			}
	// 		}
	// 
	// 		if (componentProperty && !mDragComponentPropertyField)
	// 		{
	// 			if (mSelectedItems.Count() == 1)
	// 			{
	// 				Actor* actor = (Actor*)(void*)mSelectedItems[0].object;
	// 				if (actor->GetComponent(componentProperty->GetSpecializedType()->GetUnpointedType()))
	// 				{
	// 					mDragComponentPropertyField = componentProperty;
	// 					mDragComponentPropertyField->GetWidget()->SetState("selected", true);
	// 					o2Application.SetCursor(CursorType::Hand);
	// 				}
	// 			}
	// 		}
	// 
	// 		if (!actorProperty && mDragActorPropertyField)
	// 		{
	// 			mDragActorPropertyField->GetWidget()->SetState("selected", false);
	// 			mDragActorPropertyField = nullptr;
	// 			o2Application.SetCursor(CursorType::Arrow);
	// 		}
	// 
	// 		if (!componentProperty && mDragComponentPropertyField)
	// 		{
	// 			mDragComponentPropertyField->GetWidget()->SetState("selected", false);
	// 			mDragComponentPropertyField = nullptr;
	// 			o2Application.SetCursor(CursorType::Arrow);
	// 		}
	// 	}
	// 
	// 	void UIActorsTree::OnDropped(SelectableDragableObjectsGroup* group)
	// 	{
	// 		auto listenerUnderCursor = o2Events.GetCursorListenerUnderCursor(0);
	// 		UIAssetsIconsScrollArea* assetsScrollArea = dynamic_cast<UIAssetsIconsScrollArea*>(listenerUnderCursor);
	// 		if (assetsScrollArea)
	// 		{
	// 			String targetPath = assetsScrollArea->GetViewingPath();
	// 
	// 			auto iconUnderCursor = assetsScrollArea->GetIconUnderPoint(o2Input.GetCursorPos());
	// 			if (iconUnderCursor && iconUnderCursor->GetAssetInfo().mType == TypeOf(FolderAsset).ID())
	// 				targetPath = iconUnderCursor->GetAssetInfo().mPath;
	// 
	// 			for (auto& sel : mSelectedItems)
	// 			{
	// 				ActorAsset newAsset;
	// 				newAsset.actor = *(Actor*)(void*)sel.object;
	// 				String path = targetPath.IsEmpty() ? newAsset.actor.name + ".prefab" : targetPath + "/" + newAsset.actor.name + ".prefab";
	// 				newAsset.Save(o2Assets.MakeUniqueAssetName(path));
	// 			}
	// 
	// 			BreakDragging();
	// 		}
	// 		else if (mDragActorPropertyField)
	// 		{
	// 			mDragActorPropertyField->SetValue((Actor*)(void*)(mSelectedItems[0].object));
	// 			mDragActorPropertyField->GetWidget()->SetState("selected", false);
	// 			o2Application.SetCursor(CursorType::Arrow);
	// 
	// 			mDragActorPropertyField = nullptr;
	// 
	// 			mDraggingNodes = false;
	// 			mDragNode->Hide(true);
	// 
	// 			for (auto node : mAllNodes)
	// 				node->SetState("inserting", false);
	// 
	// 			for (auto sel : mSelectedItems)
	// 			{
	// 				if (sel.node)
	// 					sel.node->SetState("dragging", false);
	// 			}
	// 
	// 			for (auto sel : mSelectedItems)
	// 				FreeSelectionSprite(sel.selectionSprite);
	// 
	// 			mSelectedItems.Clear();
	// 
	// 			for (auto obj : mBeforeDragSelected)
	// 			{
	// 				SelectedNode selectionNode;
	// 				selectionNode.object = obj.object;
	// 				selectionNode.selectionSprite = CreateSelectionSprite();
	// 				selectionNode.node = UITree::GetNode(obj.object);
	// 
	// 				mSelectedItems.Add(selectionNode);
	// 			}
	// 
	// 			UpdateLayout();
	// 		}
	// 		else if (mDragComponentPropertyField)
	// 		{
	// 			Actor* actor = (Actor*)(void*)mSelectedItems[0].object;
	// 			auto component = actor->GetComponent(mDragComponentPropertyField->GetSpecializedType()->GetUnpointedType());
	// 
	// 			mDragComponentPropertyField->SetValue(component);
	// 			mDragComponentPropertyField->GetWidget()->SetState("selected", false);
	// 			o2Application.SetCursor(CursorType::Arrow);
	// 
	// 			mDragComponentPropertyField = nullptr;
	// 
	// 			mDraggingNodes = false;
	// 			mDragNode->Hide(true);
	// 
	// 			for (auto node : mAllNodes)
	// 				node->SetState("inserting", false);
	// 
	// 			for (auto sel : mSelectedItems)
	// 			{
	// 				if (sel.node)
	// 					sel.node->SetState("dragging", false);
	// 			}
	// 
	// 			for (auto sel : mSelectedItems)
	// 				FreeSelectionSprite(sel.selectionSprite);
	// 
	// 			mSelectedItems.Clear();
	// 
	// 			for (auto obj : mBeforeDragSelected)
	// 			{
	// 				SelectedNode selectionNode;
	// 				selectionNode.object = obj.object;
	// 				selectionNode.selectionSprite = CreateSelectionSprite();
	// 				selectionNode.node = UITree::GetNode(obj.object);
	// 
	// 				mSelectedItems.Add(selectionNode);
	// 			}
	// 
	// 			UpdateLayout();
	// 		}
	// 		else UITree::OnDropped(group);
	// 	}

UIActorsTreeNode::UIActorsTreeNode():
	UITreeNode()
{}

UIActorsTreeNode::UIActorsTreeNode(const UIActorsTreeNode& other):
	UITreeNode(other)
{
	InitializeControls();
}

Editor::UIActorsTreeNode& UIActorsTreeNode::operator=(const UIActorsTreeNode& other)
{
	UITreeNode::operator=(other);
	InitializeControls();

	return *this;
}

void UIActorsTreeNode::InitializeControls()
{
	mNameDrawable = GetLayerDrawable<Text>("name");
	mLockToggle   = (UIToggle*)GetChild("lockToggle");
	mEnableToggle = (UIToggle*)GetChild("enableToggle");
	mLinkBtn      = (UIButton*)GetChild("linkBtn");
	mNameEditBox  = (UIEditBox*)GetChild("nameEditBox");
	mEditState    = GetStateObject("edit");

	if (mLockToggle)
		mLockToggle->onClick = Function<void()>(this, &UIActorsTreeNode::OnLockClicked);

	if (mEnableToggle)
		mEnableToggle->onClick = Function<void()>(this, &UIActorsTreeNode::OnEnableCkicked);

	if (mNameEditBox)
	{
		mNameEditBox->onChangeCompleted = [=](const WString& text) {
			mTargetActor->SetName(text);
			mEditState->SetState(false);
			((UIActorsTree*)mOwnerTree)->OnActorChanged(mTargetActor);
		};
	}
}

void UIActorsTreeNode::SetActor(Actor* actor)
{
	mTargetActor = actor;
	mName = actor->GetName();

	float alpha = actor->IsEnabledInHierarchy() ? 1.0f : 0.5f;

	mNameDrawable->SetText(mName);
	if (!Math::Equals(alpha, mNameDrawable->GetTransparency()))
	{
		mNameDrawable->SetTransparency(alpha);
		mEnableToggle->SetTransparency(alpha);
		mLinkBtn->SetTransparency(alpha);
	}

	mEnableToggle->SetValue(actor->IsEnabled());

	mLockToggle->SetValue(actor->IsLocked());
	mLockToggle->SetStateForcible("locked", actor->IsLockedInHierarchy());
	mLockToggle->SetStateForcible("halfHide", actor->IsLockedInHierarchy() && !actor->IsLocked());
}

void UIActorsTreeNode::EnableEditName()
{
	mEditState->SetState(true);

	mNameEditBox->text = (String)mTargetActor->name;
	mNameEditBox->SelectAll();
	mNameEditBox->UIWidget::Focus();
	mNameEditBox->ResetSroll();
}

void UIActorsTreeNode::OnLockClicked()
{
	mTargetActor->SetLocked(mLockToggle->GetValue());
}

void UIActorsTreeNode::OnEnableCkicked()
{
	mTargetActor->SetEnabled(mEnableToggle->GetValue());
}

}