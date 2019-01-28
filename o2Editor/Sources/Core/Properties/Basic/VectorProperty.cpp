#include "stdafx.h"
#include "VectorProperty.h"

#include "UI/UIManager.h"
#include "UI/Spoiler.h"
#include "Core/Properties/Properties.h"
#include "Core/Properties/Basic/IntegerProperty.h"
#include "SceneWindow/SceneEditScreen.h"

namespace Editor
{
	VectorProperty::VectorProperty()
	{
		InitializeControls();
	}

	VectorProperty::VectorProperty(const VectorProperty& other):
		IPropertyField(other)
	{
		InitializeControls();
	}

	VectorProperty& VectorProperty::operator=(const VectorProperty& other)
	{
		IPropertyField::operator=(other);
		return *this;
	}

	void VectorProperty::CopyData(const Actor& otherActor)
	{
		IPropertyField::CopyData(otherActor);
		InitializeControls();		
	}

	void VectorProperty::InitializeControls()
	{
		mSpoiler = FindChildByType<UISpoiler>(false);

		if (!mSpoiler)
		{
			mSpoiler = o2UI.CreateWidget<UISpoiler>("expand with caption");
			AddChild(mSpoiler);
		}

		if (mSpoiler)
			mSpoiler->onExpand = THIS_FUNC(OnExpand);

		mCountProperty = mSpoiler->FindChildByType<IntegerProperty>(false);
		if (!mCountProperty)
		{
			mCountProperty = dynamic_cast<IntegerProperty*>
				(o2EditorProperties.CreateFieldProperty(&TypeOf(int), "Count"));
		}

		if (mCountProperty)
		{
			if (mSpoiler)
				mSpoiler->AddChild(mCountProperty);

			mCountProperty->SetValue(0);
			mCountProperty->onChanged = THIS_FUNC(OnCountChanged);
		}

		expandHeight = true;
		expandWidth = true;
		fitByChildren = true;
	}

	VectorProperty::~VectorProperty()
	{
		for (auto x : mValuePropertiesPool)
			delete x;
	}

	void VectorProperty::SetValueAndPrototypeProxy(const TargetsVec& targets)
	{
		for (auto& pair : mTargetObjects) {
			if (pair.first.isCreated)
				delete pair.first.data;

			if (pair.second.isCreated)
				delete pair.second.data;
		}

		mTargetObjects.Clear();

		for (auto& pair : targets)
			mTargetObjects.Add({ GetObjectFromProxy(pair.first), GetObjectFromProxy(pair.second) });

		Refresh();
	}

	void VectorProperty::Refresh()
	{
		if (mTargetObjects.IsEmpty())
			return;

		if (!IsExpanded())
			return;
		
		mIsRefreshing = true;

		for (auto& pair : mTargetObjects) {
			pair.first.Refresh();
			pair.second.Refresh();
		}

		auto lastCount = mCountOfElements;
		auto lastDifferent = mCountDifferents;

		mCountOfElements = mType->GetObjectVectorSize(mTargetObjects[0].first.data);
		mCountDifferents = false;

		for (auto target : mTargetObjects)
		{
			int targetCount = mType->GetObjectVectorSize(target.first.data);
			if (targetCount != mCountOfElements)
			{
				mCountDifferents = true;
				break;
			}
		}

		if (mCountDifferents)
		{
			if (!lastDifferent)
			{
				mCountProperty->SetUnknownValue();

				for (auto prop : mValueProperties)
				{
					mSpoiler->RemoveChild(prop, false);
					FreeValueProperty(prop);
				}

				mValueProperties.Clear();

				onChanged(this);
				o2EditorSceneScreen.OnSceneChanged();
			}
		}
		else if (lastDifferent || lastCount != mCountOfElements)
		{
			mCountProperty->SetValue(mCountOfElements);

			int i = 0;
			for (; i < mCountOfElements; i++)
			{
				auto itemTargetValues = mTargetObjects.Select<Pair<IAbstractValueProxy*, IAbstractValueProxy*>>(
					[&](const Pair<TargetObjectData, TargetObjectData>& x)
				{
					return Pair<IAbstractValueProxy*, IAbstractValueProxy*>(
						mType->GetObjectVectorElementProxy(x.first.data, i),
						x.second.data ? mType->GetObjectVectorElementProxy(x.second.data, i) : nullptr);
				});

				IPropertyField* propertyDef;
				if (i < mValueProperties.Count())
					propertyDef = mValueProperties[i];
				else
				{
					propertyDef = GetFreeValueProperty();
					mValueProperties.Add(propertyDef);
				}

				mSpoiler->AddChild(propertyDef);
				propertyDef->SetCaption((String)"Element " + (String)i);
				propertyDef->SetValueAndPrototypeProxy(itemTargetValues);
				propertyDef->SetValuePath((String)i);
				propertyDef->onChangeCompleted =
					[&](const String& path, const Vector<DataNode>& before, const Vector<DataNode>& after)
				{
					OnPropertyChanged(mValuesPath + "/" + path, before, after);
				};
			}

			for (; i < mValueProperties.Count(); i++)
			{
				mSpoiler->RemoveChild(mValueProperties[i], false);
				FreeValueProperty(mValueProperties[i]);
			}

			mSpoiler->SetLayoutDirty();

			onChanged(this);
			o2EditorSceneScreen.OnSceneChanged();
		}
		else
		{
			for (int i = 0; i < mCountOfElements; i++)
			{
				auto itemTargetValues = mTargetObjects.Select<Pair<IAbstractValueProxy*, IAbstractValueProxy*>>(
					[&](const Pair<TargetObjectData, TargetObjectData>& x)
				{
					return Pair<IAbstractValueProxy*, IAbstractValueProxy*>(
						mType->GetObjectVectorElementProxy(x.first.data, i),
						x.second.data ? mType->GetObjectVectorElementProxy(x.second.data, i) : nullptr);
				});

				IPropertyField* propertyDef = mValueProperties[i];
				propertyDef->SetValueAndPrototypeProxy(itemTargetValues);
			}
		}

		mIsRefreshing = false;
	}

	const Type* VectorProperty::GetFieldType() const
	{
		return &TypeOf(void);
	}

	void VectorProperty::SpecializeType(const Type* type)
	{
		mType = nullptr;

		if (type->GetUsage() == Type::Usage::Vector)
			mType = dynamic_cast<const VectorType*>(type);
		else if (type->GetUsage() == Type::Usage::Property) 
		{
			auto propertyType = dynamic_cast<const PropertyType*>(type);

			if (propertyType->GetValueType()->GetUsage() == Type::Usage::Vector)
				mType = dynamic_cast<const VectorType*>(propertyType->GetValueType());
		}
	}

	const Type* VectorProperty::GetSpecializedType() const
	{
		return mType;
	}

	void VectorProperty::SetCaption(const WString& text)
	{
		mSpoiler->SetCaption(text);
	}

	WString VectorProperty::GetCaption() const
	{
		return mSpoiler->GetCaption();
	}

	void VectorProperty::Expand()
	{
		SetExpanded(true);
	}

	void VectorProperty::Collapse()
	{
		SetExpanded(false);
	}

	void VectorProperty::SetExpanded(bool expanded)
	{
		mSpoiler->SetExpanded(expanded);
	}

	bool VectorProperty::IsExpanded() const
	{
		return mSpoiler->IsExpanded();
	}

	void* VectorProperty::GetProxyValuePointer(IAbstractValueProxy* proxy) const
	{
		IPointerValueProxy* variableProxy = dynamic_cast<IPointerValueProxy*>(proxy);
		if (variableProxy)
			return variableProxy->GetValueVoidPointer();

		return nullptr;
	}

	VectorProperty::IPropertyField* VectorProperty::GetFreeValueProperty()
	{
		if (!mValuePropertiesPool.IsEmpty())
			return mValuePropertiesPool.PopBack();

		IPropertyField* res = o2EditorProperties.CreateFieldProperty(mType->GetElementType(), "Element", onChangeCompleted, onChanged);

		if (res)
			res->SpecializeType(mType->GetElementType());

		return res;
	}

	void VectorProperty::FreeValueProperty(IPropertyField* def)
	{
		mValuePropertiesPool.Add(def);
	}

	void VectorProperty::OnCountChanged(IPropertyField* def)
	{
		if (mIsRefreshing)
			return;

		Vector<DataNode> prevValues, newValues;

		int newCount = mCountProperty->GetCommonValue();
		auto elementFieldInfo = mType->GetElementFieldInfo();

		auto availableTypes = elementFieldInfo->GetType()->GetDerivedTypes();
		auto elementCreateType = availableTypes.IsEmpty() ? elementFieldInfo->GetType() : availableTypes.First();

		for (auto target : mValuesProxies)
		{
			void* targetPtr = GetProxyValuePointer(target.first);

			prevValues.Add(DataNode());
			prevValues.Last()["Size"].SetValue(mType->GetObjectVectorSize(targetPtr));
			DataNode& elementsData = prevValues.Last()["Elements"];

			int lastCount = mType->GetObjectVectorSize(targetPtr);
			for (int i = newCount; i < lastCount; i++)
			{
				elementFieldInfo->Serialize(mType->GetObjectVectorElementPtr(targetPtr, i),
								   *elementsData.AddNode("Element" + (String)i));
			}

			newValues.Add(DataNode());
			newValues.Last()["Size"].SetValue(newCount);

			mType->SetObjectVectorSize(targetPtr, newCount);

			if (lastCount < newCount)
			{
				for (int i = lastCount; i < newCount; i++)
					*((void**)mType->GetObjectVectorElementPtr(targetPtr, i)) = elementCreateType->CreateSample();
			}
		}

		Refresh();

		if (prevValues != newValues)
			onChangeCompleted(mValuesPath + "/count", prevValues, newValues);

		onChanged(this);
		o2EditorSceneScreen.OnSceneChanged();
	}

	void VectorProperty::OnExpand()
	{
		Refresh();
	}

	VectorProperty::TargetObjectData VectorProperty::GetObjectFromProxy(IAbstractValueProxy* proxy)
	{
		TargetObjectData res;

		if (!proxy)
			return res;

		res.proxy = proxy;

		if (auto pointerProxy = dynamic_cast<IPointerValueProxy*>(proxy)) {
			res.data = pointerProxy->GetValueVoidPointer();
			res.isCreated = false;
		}
		else {
			void* sample = proxy->GetType().CreateSample();
			proxy->GetValuePtr(sample);

			res.data = sample;
			res.isCreated = true;
		}

		return res;
	}

	void VectorProperty::OnPropertyChanged(const String& path, const Vector<DataNode>& before, const Vector<DataNode>& after)
	{
		for (auto& pair : mTargetObjects)
			pair.first.SetValue();

		onChangeCompleted(path, before, after);
	}

	void VectorProperty::TargetObjectData::Refresh()
	{
		if (!isCreated)
			return;

		proxy->GetValuePtr(data);
	}

	void VectorProperty::TargetObjectData::SetValue()
	{
		if (!isCreated)
			return;

		proxy->SetValuePtr(data);
	}

}

DECLARE_CLASS(Editor::VectorProperty);
