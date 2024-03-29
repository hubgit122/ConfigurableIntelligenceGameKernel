#include "stdafx.h"
#include "GUI.h"
#include "Player.h"
#include "CConfigurableIntelligenceGameView.h"
#include "utilities.h"

namespace CIG
{
	Player::Player( const Player& p, Chessboard* cb)
		: makeBestMove(p.makeBestMove), chessboard(cb), NAME(p.NAME), ownedChessmans(p.ownedChessmans)
	{
	}

	Player::Player(CIGRuleConfig::PLAYER_NAMES p, void (*makeBestMove_)(Chessboard* cb, void*)  , Chessboard* cb)
		: chessboard(cb), NAME(p), ownedChessmans(), makeBestMove(makeBestMove_)
	{
	}

	Player::Player(): chessboard(NULL), NAME((CIGRuleConfig::PLAYER_NAMES) - 1), ownedChessmans(), makeBestMove(GUI::askForMove)
	{
	}

	Player::~Player()
	{
	}

	void Player::operator=( const Player& p )
	{
		makeBestMove = p.makeBestMove;
		chessboard = p.chessboard;
		//ownedChessmans = p.ownedChessmans;
		ownedChessmans.forceCopyFrom(p.ownedChessmans);
		NAME = p.NAME;
	}
}