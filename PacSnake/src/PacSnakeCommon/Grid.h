#pragma once

namespace pacsnake
{
	class GridPawnID
	{
	public:
		static GridPawnID GenerateID()
		{
			static Uint32 s_prevID = 0u;
			GridPawnID id;
			id.m_id = ++s_prevID;
			return id;
		}

	Bool operator==( const GridPawnID& ) const = default;

	Bool IsValid() const
	{
		return m_id != c_invalidID;
	}

	private:
		static constexpr Uint32 c_invalidID = 0u;
		Uint32 m_id = c_invalidID;
	};

	struct GridPawn
	{
		GridPawn( const Vector2& pos )
			: m_pos( pos )
			, m_prevPos( pos )
			, m_id( GridPawnID::GenerateID() )
		{}

		Vector2 m_pos;
		Vector2 m_dir;
		Vector2 m_prevPos;
		const GridPawnID m_id;
		GridPawnID m_nextTailID;
		Bool m_growsTail = false;
	};

	class Grid
	{
	public:
		Grid( Uint32 width, Uint32 height )
			: m_width( width )
			, m_height( height )
		{}

		GridPawnID AddPawn( const Vector2& pos );
		GridPawn* GetPawn( GridPawnID id );
		const GridPawn* GetPawn( GridPawnID id ) const;
		const GridPawn* GetPawnAtPos( const Vector2& pos ) const;

		Uint32 GetWidth() const
		{
			return m_width;
		}

		Uint32 GetHeight() const
		{
			return m_height;
		}

		forge::ArraySpan< const GridPawn > GetPawns() const
		{
			return m_pawns;
		}

		void UpdatePawnTail( pacsnake::GridPawn& pawn );

		void Update();

		struct Collision
		{
			GridPawnID m_first;
			GridPawnID m_second;
		};

		std::vector< Collision > FindCollisions();

	private:
		std::vector< GridPawn > m_pawns;
		Uint32 m_width = 0u;
		Uint32 m_height = 0u;
	};
}