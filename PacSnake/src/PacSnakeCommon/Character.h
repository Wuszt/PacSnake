#pragma once
#include "Grid.h"
#include "Actor.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Character : public pacsnake::Actor
	{
		RTTI_DECLARE_ABSTRACT_CLASS( Character, pacsnake::Actor );

	public:
		enum class Action
		{
			None,
			TurnLeft,
			TurnRight,
		};

		void ScheduleAction( Action action )
		{
			m_scheduledAction = action;
		}

		const Vector2& GetDir() const;

	protected:
		virtual void OnBeforeSimUpdated() override;

	private:
		void TurnLeft();
		void TurnRight();

		Action m_scheduledAction;
	};
}