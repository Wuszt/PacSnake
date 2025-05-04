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

		[[nodiscard]] forge::CallbackToken RegisterOnBeforeActionTaken( std::function< void() > func );
		[[nodiscard]] forge::CallbackToken RegisterOnSimUpdated( std::function< void() > func );
		[[nodiscard]] forge::CallbackToken RegisterOnNewPickup( std::function< void() > func );

		static Action TranslateDirectionToAction( const Vector2& currentDir, const Vector2& newDir );

		static Vector2 GetDirRotatedRight( const Vector2& dir );
		static Vector2 GetDirRotatedLeft( const Vector2& dir );

	protected:
		virtual void OnBeforeSimUpdated() override;
		virtual void OnSimUpdated() override;
		virtual void OnNewPickup() override;

	private:
		void TurnLeft();
		void TurnRight();

		Action m_scheduledAction;
		forge::Callback<> m_onBeforeActionTaken;
		forge::Callback<> m_onSimUpdated;
		forge::Callback<> m_onNewPickup;
	};
}