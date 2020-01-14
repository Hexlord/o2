#pragma once

#include "o2/Animation/IAnimation.h"
#include "o2/Utils/Debug/Debug.h"
#include "o2/Utils/Property.h"
#include "o2/Utils/Reflection/Attributes.h"
#include "o2/Utils/Types/String.h"

namespace o2
{
	class IObject;
	class AnimationComponent;
	class AnimationState;

	template<typename _type>
	class AnimatedValue;

	class IAnimatedValue;

	// -----------------------------------------------------------
	// Animation. Can animate anything object derived from IObject
	// -----------------------------------------------------------
	class Animation : public IAnimation
	{
	public:
		struct AnimatedValueDef;

	public:
		Function<void()> onChanged;        // It is called when some animated value has changed
		Function<void()> onDurationChange; // It is called when duration has changed

	public:
		// Default constructor
		Animation(IObject* target = nullptr);

		// Copy-constructor
		Animation(const Animation& other);

		// Destructor
		~Animation();

		// Assign operator
		Animation& operator=(const Animation& other);

		// Sets animation target
		// Bind all animation values to target's child fields (if it possible)
		void SetTarget(IObject* target, bool errors = true);

		// Returns animation's target
		IObject* GetTarget() const;

		// Removes and clears all animated values
		void Clear();

		// Returns animation values
		Vector<AnimatedValueDef>& GetAnimationsValues();

		// Returns animation values
		const Vector<AnimatedValueDef>& GetAnimationsValues() const;

		// Returns is contains animated value by path (some like "path/abc/cde")
		bool ContainsAnimationValue(const String& path) const;

		// Returns animated value by path (some like "path/abc/cde")
		template<typename _type>
		AnimatedValue<_type>* GetAnimationValue(const String& path);

		// Returns animated value by value pointer
		template<typename _type>
		AnimatedValue<_type>* GetAnimationValue(_type* target);

		// Adds animation value with specified path
		template<typename _type>
		AnimatedValue<_type>* AddAnimationValue(const String& path);

		// Add animation value with specified path
		IAnimatedValue* AddAnimationValueNoType(const String& path);

		// Removes animated value for target
		template<typename _type>
		bool RemoveAnimationValue(_type* target);

		// Removes animated value by path
		bool RemoveAnimationValue(const String& path);

		// Adds key in animated value by path and position
		template<typename _type>
		void AddKey(const String& targetPath, float position, const typename AnimatedValue<_type>::Key& key);

		// Adds key in animated value for target by position
		template<typename _type>
		void AddKey(_type* target, float position, const typename AnimatedValue<_type>::Key& key);

		// Sets keys in animated value by path
		template<typename _type>
		void SetKeys(const String& targetPath, const Vector<typename AnimatedValue<_type>::Key>& key);

		// Sets keys in animated value for target 
		template<typename _type>
		void SetKeys(_type* target, const Vector<typename AnimatedValue<_type>::Key>& key);

		// Returns animated value keys by path
		template<typename _type>
		Vector<typename AnimatedValue<_type>::Key> GetKeys(const String& path);

		// Returns animated value keys by target
		template<typename _type>
		Vector<typename AnimatedValue<_type>::Key> GetKeys(_type* target);

		// Removes key in animated value by path and position
		template<typename _type>
		bool Removekey(const String& targetPath, float position);

		// Removes key in animated value for target by position
		template<typename _type>
		bool Removekey(_type* target, float position);

		// Removes all keys in animated value for target
		template<typename _type>
		void RemoveAllKeys(_type* target);

		// Removes all keys in animated value by path
		template<typename _type>
		void RemoveAllKeys(const String& targetPath);

		//insert animation

		// Returns parametric specified animation
		// Sample: Parametric(targetObjectPtr, targetValuePtr, someBegin, someEnd, 1.0f, 0.0f, 0.4f, 1.0f, 0.6f) 
		// Works like css-bezier curves
		template<typename _type>
		static Animation Parametric(IObject* target, const String& animatingValuePath,
									const _type& begin, const _type& end, float duration,
									float beginCoef, float beginCoefPosition,
									float endCoef, float endCoefPosition);

		// Returns animation from begin to end in duration with ease in
		template<typename _type>
		static Animation EaseIn(IObject* target, const String& animatingValuePath,
								const _type& begin, const _type& end, float duration = 1.0f);

		// Returns animation from begin to end in duration with ease out
		template<typename _type>
		static Animation EaseOut(IObject* target, const String& animatingValuePath,
								 const _type& begin, const _type& end, float duration = 1.0f);

		// Returns animation from begin to end in duration with ease in-out
		template<typename _type>
		static Animation EaseInOut(IObject* target, const String& animatingValuePath,
								   const _type& begin, const _type& end, float duration = 1.0f);

		// Returns animation from begin to end in duration with linear transition
		template<typename _type>
		static Animation Linear(IObject* target, const String& animatingValuePath,
								const _type& begin, const _type& end, float duration = 1.0f);

		SERIALIZABLE(Animation);

	public:
		// -----------------------------------
		// Animated value definition structure
		// -----------------------------------
		struct AnimatedValueDef : public ISerializable
		{
			String          targetPath;              // Target path @SERIALIZABLE
			void*           targetPtr = nullptr;     // Target pointer
			IAnimatedValue* animatedValue = nullptr; // Animated value @SERIALIZABLE

			// Check equals operator
			bool operator==(const AnimatedValueDef& other) const;

			SERIALIZABLE(AnimatedValueDef);
		};

	protected:
		Vector<AnimatedValueDef> mAnimatedValues; // Animated value @SERIALIZABLE
		IObject*          mTarget;         // Target object
		AnimationState*   mAnimationState; // Animation state owner

	protected:
		// Evaluates all animated values by time
		void Evaluate() override;

		// Returns animated value by path
		template<typename _type>
		AnimatedValue<_type>* FindValue(const String& path);

		// Returns animated value by target
		template<typename _type>
		AnimatedValue<_type>* FindValue(_type* target);

		// It is called when some animated value has changed
		void OnAnimatedValueChanged();

		// Recalculates maximum duration by animated values
		void RecalculateDuration();

		// Completion deserialization callback
		void OnDeserialized(const DataNode& node) override;

		// It is called when animated value was added. Need to register value agent in animatable target
		void OnAnimatedValueAdded(AnimatedValueDef& valueDef);

		friend class AnimationComponent;
		friend class Animate;
	};
}

#include "o2/Animation/AnimatedValue.h"

namespace o2
{
	template<typename _type>
	AnimatedValue<_type>* Animation::FindValue(_type* target)
	{
		for (auto val : mAnimatedValues)
		{
			if (val.targetPtr == target)
				return (AnimatedValue<_type>*)val.animatedValue;
		}

		return nullptr;
	}

	template<typename _type>
	AnimatedValue<_type>* Animation::FindValue(const String& path)
	{
		for (auto val : mAnimatedValues)
		{
			if (val.targetPath == path)
				return (AnimatedValue<_type>*)val.animatedValue;
		}

		return nullptr;
	}

	template<typename _type>
	void Animation::RemoveAllKeys(_type* target)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(target);
		if (animVal)
			animVal->RemoveAllKeys();
	}

	template<typename _type>
	void Animation::RemoveAllKeys(const String& targetPath)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(targetPath);
		if (animVal)
			animVal->RemoveAllKeys();
	}

	template<typename _type>
	Animation Animation::Parametric(IObject* target, const String& animatingValuePath,
									const _type& begin, const _type& end, float duration,
									float beginCoef, float beginCoefPosition, float endCoef, float endCoefPosition)
	{
		Animation res(target);
		*res.AddAnimationValue<_type>(animatingValuePath) = AnimatedValue<_type>::Parametric(begin, end, duration,
																							 beginCoef, beginCoefPosition,
																							 endCoef, endCoefPosition);
		return res;
	}

	template<typename _type>
	Animation Animation::EaseIn(IObject* target, const String& animatingValuePath,
								const _type& begin, const _type& end, float duration /*= 1.0f*/)
	{
		Animation res(target);
		*res.AddAnimationValue<_type>(animatingValuePath) = AnimatedValue<_type>::EaseIn(begin, end, duration);
		return res;
	}

	template<typename _type>
	Animation Animation::EaseOut(IObject* target, const String& animatingValuePath,
								 const _type& begin, const _type& end, float duration /*= 1.0f*/)
	{
		Animation res(target);
		*res.AddAnimationValue<_type>(animatingValuePath) = AnimatedValue<_type>::EaseOut(begin, end, duration);
		return res;
	}

	template<typename _type>
	Animation Animation::EaseInOut(IObject* target, const String& animatingValuePath,
								   const _type& begin, const _type& end, float duration /*= 1.0f*/)
	{
		Animation res(target);
		*res.AddAnimationValue<_type>(animatingValuePath) = AnimatedValue<_type>::EaseInOut(begin, end, duration);
		return res;
	}

	template<typename _type>
	Animation Animation::Linear(IObject* target, const String& animatingValuePath,
								const _type& begin, const _type& end, float duration /*= 1.0f*/)
	{
		Animation res(target);
		*res.AddAnimationValue<_type>(animatingValuePath) = AnimatedValue<_type>::Linear(begin, end, duration);
		return res;
	}

	template<typename _type>
	bool Animation::Removekey(_type* target, float position)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(target);
		if (animVal)
			return animVal->RemoveKey(position);

		return false;
	}

	template<typename _type>
	bool Animation::Removekey(const String& targetPath, float position)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(targetPath);
		if (animVal)
			return animVal->RemoveKey(position);

		return false;
	}

	template<typename _type>
	Vector<typename AnimatedValue<_type>::Key> Animation::GetKeys(_type* target)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(target);
		if (animVal)
			return animVal->GetKeys();

		return AnimatedValue<_type>::Vector<Key>();
	}

	template<typename _type>
	Vector<typename AnimatedValue<_type>::Key> Animation::GetKeys(const String& path)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(path);
		if (animVal)
			return animVal->GetKeys();

		return AnimatedValue<_type>::Vector<Key>();
	}

	template<typename _type>
	void Animation::SetKeys(_type* target, const Vector<typename AnimatedValue<_type>::Key>& key)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(target);
		if (!animVal)
			animVal = AddAnimationValue(target);

		animVal->SetKeys(key);
	}

	template<typename _type>
	void Animation::SetKeys(const String& targetPath, const Vector<typename AnimatedValue<_type>::Key>& key)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(targetPath);
		if (!animVal)
			animVal = AddAnimationValue<_type>(targetPath);

		animVal->SetKeys(key);
	}

	template<typename _type>
	void Animation::AddKey(_type* target, float position, const typename AnimatedValue<_type>::Key& key)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(target);
		if (!animVal)
			animVal = AddAnimationValue(target);

		animVal->AddKey(key, position);
	}

	template<typename _type>
	void Animation::AddKey(const String& targetPath, float position, const typename AnimatedValue<_type>::Key& key)
	{
		AnimatedValue<_type> animVal = FindValue<_type*>(targetPath);
		if (!animVal)
			animVal = AddAnimationValue<_type>(targetPath);

		animVal->InsertKey(key, position);
	}

	template<typename _type>
	bool Animation::RemoveAnimationValue(_type* target)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.targetPtr == target)
			{
				def.mAnimatedValue->onKeysChanged -= THIS_FUNC(OnAnimatedValueChanged);
				delete val.animatedValue;
				mAnimatedValues.Remove(val);
				onChanged();

				return true;
			}
		}

		return false;
	}

	template<typename _type>
	AnimatedValue<_type>* Animation::AddAnimationValue(const String& path)
	{
		AnimatedValueDef def;
		def.animatedValue = mnew AnimatedValue<_type>();
		def.animatedValue->onKeysChanged += THIS_FUNC(OnAnimatedValueChanged);

		if (mTarget)
		{
			FieldInfo* fieldInfo = nullptr;
			const ObjectType* type = dynamic_cast<const ObjectType*>(&mTarget->GetType());
			void* castedTarget = type->DynamicCastFromIObject(mTarget);
			def.targetPtr = (_type*)mTarget->GetType().GetFieldPtr(castedTarget, path, fieldInfo);

			if (!fieldInfo)
			{
				o2Debug.LogWarning("Can't create animated value: field info not found " + path);
				def.targetPtr = (_type*)mTarget->GetType().GetFieldPtr(castedTarget, path, fieldInfo);
				return nullptr;
			}

			if (fieldInfo->GetType()->GetUsage() == Type::Usage::Property)
				def.animatedValue->SetTargetProxyVoid(fieldInfo->GetType()->GetValueProxy(def.targetPtr));
			else
				def.animatedValue->SetTargetVoid(def.targetPtr);
		}

		def.targetPath = path;
		mAnimatedValues.Add(def);

		OnAnimatedValueAdded(def);

		return (AnimatedValue<_type>*)def.animatedValue;
	}

	template<typename _type>
	AnimatedValue<_type>* Animation::GetAnimationValue(_type* target)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.targetPtr == target)
				return val.animatedValue;
		}

		return nullptr;
	}

	template<typename _type>
	AnimatedValue<_type>* Animation::GetAnimationValue(const String& path)
	{
		for (auto& val : mAnimatedValues)
		{
			if (val.targetPath == path)
				return val.animatedValue;
		}

		return nullptr;
	}
}

CLASS_BASES_META(o2::Animation)
{
	BASE_CLASS(o2::IAnimation);
}
END_META;
CLASS_FIELDS_META(o2::Animation)
{
	PUBLIC_FIELD(onChanged);
	PUBLIC_FIELD(onDurationChange);
	PROTECTED_FIELD(mAnimatedValues).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mTarget);
	PROTECTED_FIELD(mAnimationState);
}
END_META;
CLASS_METHODS_META(o2::Animation)
{

	PUBLIC_FUNCTION(void, SetTarget, IObject*, bool);
	PUBLIC_FUNCTION(IObject*, GetTarget);
	PUBLIC_FUNCTION(void, Clear);
	PUBLIC_FUNCTION(Vector<AnimatedValueDef>&, GetAnimationsValues);
	PUBLIC_FUNCTION(const Vector<AnimatedValueDef>&, GetAnimationsValues);
	PUBLIC_FUNCTION(bool, ContainsAnimationValue, const String&);
	PUBLIC_FUNCTION(IAnimatedValue*, AddAnimationValueNoType, const String&);
	PUBLIC_FUNCTION(bool, RemoveAnimationValue, const String&);
	PROTECTED_FUNCTION(void, Evaluate);
	PROTECTED_FUNCTION(void, OnAnimatedValueChanged);
	PROTECTED_FUNCTION(void, RecalculateDuration);
	PROTECTED_FUNCTION(void, OnDeserialized, const DataNode&);
	PROTECTED_FUNCTION(void, OnAnimatedValueAdded, AnimatedValueDef&);
}
END_META;

CLASS_BASES_META(o2::Animation::AnimatedValueDef)
{
	BASE_CLASS(o2::ISerializable);
}
END_META;
CLASS_FIELDS_META(o2::Animation::AnimatedValueDef)
{
	PUBLIC_FIELD(targetPath).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(targetPtr);
	PUBLIC_FIELD(animatedValue).SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::Animation::AnimatedValueDef)
{
}
END_META;