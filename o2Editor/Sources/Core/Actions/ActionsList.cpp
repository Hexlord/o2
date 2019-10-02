#include "stdafx.h"

#include "ActionsList.h"
#include "SceneWindow/SceneEditScreen.h"
#include "PropertyChange.h"

namespace Editor
{
	ActionsList::~ActionsList()
	{
		for (auto action : mActions)
			delete action;

		for (auto action : mForwardActions)
			delete action;
	}

	int ActionsList::GetUndoActionsCount() const
	{
		return mActions.Count();
	}

	int ActionsList::GetRedoActionsCount() const
	{
		return mForwardActions.Count();
	}

	String ActionsList::GetLastActionName() const
	{
		if (mActions.Count() > 0)
			return mActions.Last()->GetName();

		return "";
	}

	String ActionsList::GetNextForwardActionName() const
	{
		if (mForwardActions.Count() > 0)
			return mForwardActions.Last()->GetName();

		return "";
	}

	void ActionsList::UndoAction()
	{
		if (mActions.Count() > 0)
		{
			mActions.Last()->Undo();
			mForwardActions.Add(mActions.PopBack());
		}
	}

	void ActionsList::RedoAction()
	{
		if (mForwardActions.Count() > 0)
		{
			mForwardActions.Last()->Redo();
			mActions.Add(mForwardActions.PopBack());
		}
	}

	void ActionsList::DoneAction(IAction* action)
	{
		mActions.Add(action);

		for (auto action : mForwardActions)
			delete action;

		mForwardActions.Clear();
	}

	void ActionsList::DoneActorPropertyChangeAction(const String& path, const Vector<DataNode>& prevValue, const Vector<DataNode>& newValue)
	{
		PropertyChangeAction* action = mnew PropertyChangeAction(
			o2EditorSceneScreen.GetSelectedObjects(), path, prevValue, newValue);

		DoneAction(action);
	}

	void ActionsList::ResetUndoActions()
	{
		for (auto x : mActions)
			delete x;

		for (auto x : mForwardActions)
			delete x;

		mActions.Clear();
		mForwardActions.Clear();
	}
}