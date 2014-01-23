#include "stdafx.h"
#include "Chessboard.h"
#include "Player.h"
#include "Array.h"
#include "Chessman.h"
#include "ChessmanIndex.h"
#include "GraphSearchEngine.h"

namespace CIG
{
	// �����ó�ʼ������ִ��, ���Կ����ٶ���һ��.
	CIG::Chessboard::Chessboard() : nowRound(0), nowTurn((CIGRuleConfig::PLAYER_NAMES)0), pickedChessmanByIndex(), currentBannedMotions(), chessmanIndexBoard()
	{
		players[0] = Player(CIGRuleConfig::HUMAN, GUI::askForMove, this);
		players[1] = Player(CIGRuleConfig::COMPUTER, GraphSearchEngine::makeBestMove, this);			// TO-DO  Ӧ������������ʵ��������player��ʵ��.

		memset(loose, 0, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memset(win, 0, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);

		for (int k = 0; k < CIGRuleConfig::PLAYER_NUM; ++k)
		{
			evaluations[k] = 0;

			for (int i = 0; i < (1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2); ++i)
			{
				for (int j = 0 ; j < (1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2); ++j)
				{
					CIGRuleConfig::CHESSMAN_TYPES t = CIGRuleConfig::INI_BOARD[k][i][j];

					if ((t != CIGRuleConfig::NOCHESSMAN) && (!beyondBoardRange(PointOrVector(j, i))))
					{
						Chessman c = Chessman(t, PointOrVector(j, i), (CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size, CIGRuleConfig::ON_BOARD, CIGRuleConfig::ALL);
						players[k].ownedChessmans.push(c);

						chessmanIndexBoard[PointOrVector(j, i)] = ChessmanIndex((CIGRuleConfig::PLAYER_NAMES)k, players[k].ownedChessmans.size - 1);
					}
				}
			}
		}

		refreshEvaluations();
	}

	CIG::Chessboard::Chessboard( const Chessboard& cb )
		: nowRound(cb.nowRound),
		  nowTurn(cb.nowTurn),
		  chessmanIndexBoard(cb.chessmanIndexBoard)
	{
		currentBannedMotions.forceCopyFrom(cb.currentBannedMotions);
		pickedChessmanByIndex.forceCopyFrom(cb.pickedChessmanByIndex);

		memcpy(loose, cb.loose, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(win, cb.win, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
	}

	void CIG::Chessboard::operator=( const Chessboard& cb )
	{
		currentBannedMotions.forceCopyFrom(cb.currentBannedMotions);
		pickedChessmanByIndex.forceCopyFrom(cb.pickedChessmanByIndex);
		nowRound = cb.nowRound;
		nowTurn = cb.nowTurn;

		this->chessmanIndexBoard = cb.chessmanIndexBoard;
		memcpy(win, cb.win, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(loose, cb.loose, sizeof(bool)*CIGRuleConfig::PLAYER_NUM);
		memcpy(this->evaluations, cb.evaluations, sizeof(evaluations));

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			players[i] = cb.players[i];
			players[i].chessboard = this;
		}
	}

	//************************************
	// Method:    operator[]
	// FullName:  CIG::Chessboard::operator[]
	// Access:    public
	// Returns:   Chessman*
	// Qualifier: const
	// Parameter: PointOrVector p
	// ע���ڲ�����, Ĭ���Ѿ�������Χ���.
	//************************************
	CIG::Chessman* CIG::Chessboard::operator []( PointOrVector p ) const			// TO-DO �����߼������Ż�һ��, ������ô���ȽϺ�
	{
		if (beyondBoardRange(p))
		{
			return (Chessman*)(void*) - 1;
		}

		ChessmanIndex& ci = chessmanIndexBoard.content[(p[1] << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) + p[0]];
		return (ci.player == -1) ? NULL : &(players[ci.player].ownedChessmans[ci.index]);
	}

	int CIG::Chessboard::getEvaluation( CIGRuleConfig::PLAYER_NAMES p ) const
	{
		//���Լ���ʤ��, ����ƽ��, �и�
		int result = 0;

		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			if (i != p)
			{
				result -= evaluations[i];
			}
		}

		result = evaluations[p] + (result) / (CIGRuleConfig::PLAYER_NUM - 1);

		return result;
	}

	int CIG::Chessboard::getEvaluation() const
	{
		return getEvaluation(nowTurn);
	}

	bool CIG::Chessboard::beyondBoardRange( PointOrVector& p )const
	{
		static const unsigned mask = (~(((unsigned short)1 << CIGRuleConfig::INI_BOARD_WIDTH_LOG2) - 1) << sizeof(unsigned short) * 8) | (unsigned short) - 1 & (~(((unsigned short)1 << CIGRuleConfig::INI_BOARD_HEIGHT_LOG2) - 1));

		if (mask&*((unsigned*)p.x))
		{
			return true;
		}
		else if (!CIGRuleConfig::BOARD_RANGE[p.x[1]][p.x[0]])
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	Chessman* Chessboard::onAddIntent(PointOrVector p, bool refreshEvaluations)
	{
		Chessman* c = players[nowTurn].ownedChessmans.add(Chessman(CIGRuleConfig::CHESS, p, nowTurn, players[nowTurn].ownedChessmans.size, CIGRuleConfig::OFF_BOARD, CIGRuleConfig::ALL));
		pickedChessmanByIndex.add(c->chessmanIndex);
		return c;
	}

	//����ԭ����, ����������ô��
	void Chessboard::undoAdd(bool refreshEvaluations)
	{
		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size - 1);
		players[nowTurn].ownedChessmans.popNoReturn();
	}

	bool CIG::Chessboard::onPutIntent( Chessman* c, PointOrVector p , bool refreshEvaluations)
	{
		if ((*this)[p] != NULL)			//beyondBoardʱ����-1(ffff), ����Ҳ�᷵��false
		{
			return false;
		}
		else if ((pickedChessmanByIndex.size > 0) && (pickedChessmanByIndex[0] == c->chessmanIndex))
		{
			if (refreshEvaluations)
			{
				static const int DIRECTION_NUM = 8;
				static const PointOrVector direction[DIRECTION_NUM] = 
				{
					PointOrVector(1,0),
					PointOrVector(1,1),
					PointOrVector(0,1),
					PointOrVector(-1,1),
					PointOrVector(-1,0),
					PointOrVector(-1,-1),
					PointOrVector(0,-1),
					PointOrVector(1,-1),
				};
				bool enemyOnTheEnd[DIRECTION_NUM];
				unsigned char numOfChessman[DIRECTION_NUM];
				unsigned char totalnumOfChessman[DIRECTION_NUM>>1];

				for (int d = 0;d<DIRECTION_NUM;++d)
				{
					enemyOnTheEnd[d] = false;
					numOfChessman[d] = 0;
					PointOrVector testPoint = c->coordinate;

					for (Chessman* testc = (*this)[testPoint+=direction[d]]; testc; testc = (*this)[testPoint+=direction[d]])
					{
						if (testc==(void*)-1)
						{
							enemyOnTheEnd[d] = true;
							break;
						}
						else if (!testc)
						{
							enemyOnTheEnd[d] = false;
							break;
						}
						else if (testc->chessmanIndex.player!=nowTurn)
						{
							enemyOnTheEnd[d] = true;
							break;
						}
						else
						{
							++numOfChessman[d];
						}
					}
				}

				for (int d=0;d<(DIRECTION_NUM>>1);++d)
				{
					totalnumOfChessman[d]= numOfChessman[d]+numOfChessman[d+4]+1;
					if (totalnumOfChessman[d]>=5)
					{
						loose[1-nowTurn] = true;
					}
					evaluations[nowTurn] -= GRADES[enemyOnTheEnd[d]+0][numOfChessman[d]];
					evaluations[nowTurn] -= GRADES[enemyOnTheEnd[d+4]+0][numOfChessman[d+4]];
					evaluations[nowTurn] +=GRADES[enemyOnTheEnd[d]+enemyOnTheEnd[d+4]][totalnumOfChessman[d]>5?5:totalnumOfChessman[d]];
				}
			}

			chessmanIndexBoard[p] = c->chessmanIndex;

			assert(c->onPutIntent(p));    //�������������, ˵���߷����������.

			pickedChessmanByIndex.deleteAtNoReturn(0);
			return true;
		}

		return false;
	}

	void CIG::Chessboard::undoPut(Chessman* c, PointOrVector previousP, bool refreshEvaluations /*= false*/)
	{
		undoPut(c, refreshEvaluations);
		c->coordinate = previousP;
	}

	void CIG::Chessboard::undoPut(Chessman* c , bool refreshEvaluations /*= false*/)
	{
		chessmanIndexBoard[c->coordinate].clear();
		c->undoPut();
		pickedChessmanByIndex.add(c->chessmanIndex);

		if (refreshEvaluations)
		{
			static const int DIRECTION_NUM = 8;
			static const PointOrVector direction[DIRECTION_NUM] = 
			{
				PointOrVector(1,0),
				PointOrVector(1,1),
				PointOrVector(0,1),
				PointOrVector(-1,1),
				PointOrVector(-1,0),
				PointOrVector(-1,-1),
				PointOrVector(0,-1),
				PointOrVector(1,-1),
			};
			bool enemyOnTheEnd[DIRECTION_NUM];
			unsigned char numOfChessman[DIRECTION_NUM];
			unsigned char totalnumOfChessman[DIRECTION_NUM>>1];

			for (int d = 0;d<DIRECTION_NUM;++d)
			{
				enemyOnTheEnd[d] = false;
				numOfChessman[d] = 0;
				PointOrVector testPoint = c->coordinate;

				for (Chessman* testc = (*this)[testPoint+=direction[d]]; testc; testc = (*this)[testPoint+=direction[d]])
				{
					if (testc==(void*)-1)
					{
						enemyOnTheEnd[d] = true;
						break;
					}
					else if (!testc)
					{
						enemyOnTheEnd[d] = false;
						break;
					}
					else if (testc->chessmanIndex.player!=nowTurn)
					{
						enemyOnTheEnd[d] = true;
						break;
					}
					else
					{
						++numOfChessman[d];
					}
				}
			}

			for (int d=0;d<(DIRECTION_NUM>>1);++d)
			{
				totalnumOfChessman[d]= numOfChessman[d]+numOfChessman[d+4]+1;
					loose[1-nowTurn] = false;			//�ر�ע������, û��, ����ʱ�Ż����undo, �������looose. 
				evaluations[nowTurn] += GRADES[enemyOnTheEnd[d]+0][numOfChessman[d]];
				evaluations[nowTurn] += GRADES[enemyOnTheEnd[d+4]+0][numOfChessman[d+4]];
				evaluations[nowTurn] -=GRADES[enemyOnTheEnd[d]+enemyOnTheEnd[d+4]][totalnumOfChessman[d]>5?5:totalnumOfChessman[d]];
			}
		}
	}

	bool CIG::Chessboard::onChangeTurn()
	{
		nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn + 1) % (CIGRuleConfig::PLAYER_NUM));
		nowRound++;
		return true;
	}

	void Chessboard::undoChangeTurn()
	{
		nowTurn = (CIGRuleConfig::PLAYER_NAMES)((nowTurn - 1 + CIGRuleConfig::PLAYER_NUM) % (CIGRuleConfig::PLAYER_NUM));
		nowRound--;
	}

	bool CIG::Chessboard::onMoveIntent(Move& move, bool refreshEvaluations)
	{
		bool result = true;

		for (int i = 0; i < move.size; i++)
		{
			result  &= onMotionIntent(move[i], refreshEvaluations);
		}

		result &= (this->pickedChessmanByIndex.size == 0);
		return result;
	}

	bool Chessboard::onMotionIntent( Motion& op , bool refreshEvaluations)
	{
		switch (op.operation)
		{
			case CIGRuleConfig::ADD:
				if (!onAddIntent(op.distination , refreshEvaluations))
				{
					return false;
				}

				break;

			case CIGRuleConfig::PICK:
				if (!onPickIntent(&(this->players[op.chessmanIndex.player].ownedChessmans[op.chessmanIndex.index]), refreshEvaluations))
				{
					return false;
				}

				break;

			case CIGRuleConfig::PUT:
				if(!onPutIntent(&(this->players[op.chessmanIndex.player].ownedChessmans[op.chessmanIndex.index]), op.distination, refreshEvaluations))
				{
					return false;
				}

				break;

			case CIGRuleConfig::CAPTURE:
				if(!onCaptureIntent(&(this->players[pickedChessmanByIndex[0].player].ownedChessmans[pickedChessmanByIndex[0].index]), op.distination, refreshEvaluations))
				{
					return false;
				}

				break;

			case CIGRuleConfig::PROMOTION:
				return false;
				break;

			default:
				return false;
				break;
		}

		return true;
	}

	bool CIG::Chessboard::canMakeWholeMove(Move& move, bool refreshEvaluations)
	{
		Chessboard cb(*this);
		bool result = cb.onWholeMoveIntent(move, refreshEvaluations);
		return result;
	}

	void Chessboard::undoMove( Move& move , bool refreshEvaluations)
	{
		for (int i = move.size - 1; i >= 0; --i)
		{
			undoMotion(move[i], refreshEvaluations);
		}
	}

	void Chessboard::undoMotion( Motion& operation , bool refreshEvaluations)
	{
		switch (operation.operation)
		{
			case CIGRuleConfig::ADD:
				undoAdd(refreshEvaluations);
				break;

			case CIGRuleConfig::PICK:
				undoPick(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), operation.distination, refreshEvaluations);
				break;

			case CIGRuleConfig::PUT:
				undoPut(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), operation.savedCoodinate, refreshEvaluations);
				break;

			case CIGRuleConfig::CAPTURE:
				undoCaptured(&(this->players[operation.chessmanIndex.player].ownedChessmans[operation.chessmanIndex.index]), refreshEvaluations);
				break;

			default:
				break;
		}
	}

	bool Chessboard::onWholeMoveIntent( Move& move , bool refreshEvaluations)
	{
		return onMoveIntent(move, refreshEvaluations) && onChangeTurn();
	}

	void Chessboard::undoWholeMove( Move& move , bool refreshEvaluations)
	{
		undoChangeTurn();
		undoMove(move, refreshEvaluations);
	}

	bool CIG::Chessboard::onPickIntent( PointOrVector p , bool refreshEvaluations)
	{
		return onPickIntent((*this)[p], refreshEvaluations);
	}

	void CIG::Chessboard::undoPick(Chessman* c , PointOrVector p, bool refreshEvaluations)
	{
		if (refreshEvaluations)
		{
			// TO-DO

		}

		pickedChessmanByIndex.deleteAtNoReturn(pickedChessmanByIndex.size - 1);
		chessmanIndexBoard[p] = c->chessmanIndex;
		c->coordinate = p;
		c->undoPick();
	}

	void CIG::Chessboard::undoPromotion(Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t, bool refreshEvaluations)
	{
		//TODO
	}
	void CIG::Chessboard::undoPromotion(PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t, bool refreshEvaluations)
	{
		//TODO
	}

	bool CIG::Chessboard::onCaptureIntent( Chessman* c, PointOrVector p , bool refreshEvaluations)
	{
		if (beyondBoardRange(p))
		{
			return false;
		}
		else
		{
			Chessman* testC = (*this)[p];

			if ((testC == NULL) || (testC->chessmanIndex.player == nowTurn))
			{
				return false;
			}
			else if (pickedChessmanByIndex[0] == c->chessmanIndex)
			{
				if (testC->onCapturedIntent())
				{
					c->onCaptureIntent(testC);
					chessmanIndexBoard[p].clear();
					//loose[testC->chessmanIndex.player] = (testC->chessmanType == CIGRuleConfig::KING);

					if (refreshEvaluations)
					{
						//evaluations[testC->chessmanIndex.player] -= CIGRuleConfig::EVALUATIONS[testC->chessmanIndex.player][testC->chessmanType][p.x[1]][p.x[0]];
					}

					return true;
				}
				else
				{
					return false;
				}
			}
		}

		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( Chessman* c, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return false;
	}

	bool CIG::Chessboard::onPickIntent( Chessman* c , bool refreshEvaluations)
	{
		if ((c->chessmanIndex.player == nowTurn) && (pickedChessmanByIndex.size == 0) && (c->onPickIntent()))
		{
			PointOrVector& p = c->coordinate;
			pickedChessmanByIndex.add(c->chessmanIndex);
			chessmanIndexBoard[c->coordinate].clear();

			if (refreshEvaluations)
			{
				//evaluations[nowTurn] -= CIGRuleConfig::EVALUATIONS[nowTurn][c->chessmanType][p.x[1]][p.x[0]];
			}

			return true;
		}

		return false;
	}

	bool CIG::Chessboard::onPromotionIntent( PointOrVector p, CIGRuleConfig::CHESSMAN_TYPES t , bool refreshEvaluations)
	{
		return onPromotionIntent((*this)[p], t, refreshEvaluations);
	}

	void Chessboard::undoCaptured( Chessman* c , bool refreshEvaluations)
	{
		c->undoCaptured();
		PointOrVector p(c->coordinate);
		chessmanIndexBoard[p] = c->chessmanIndex;

		//if (c->chessmanType == CIGRuleConfig::KING)
		//{
		//	loose[c->chessmanIndex.player] = false;
		//}

		if (refreshEvaluations)
		{
			// TO-DO
		}
	}

	void Chessboard::refreshEvaluations()
	{
		// TO-DO
	}

	bool Chessboard::gameOver()
	{
		// TO-DO
		for (int i = 0; i < CIGRuleConfig::PLAYER_NUM; ++i)
		{
			if (loose[i])
			{
				return true;
			}
		}

		return false;
	}
	const int Chessboard::GRADES[3][6]= 
	{
		{0,30,100,1000,MATE_VALUE>>2,MATE_VALUE},
		{0,10,50,200,1000,MATE_VALUE},
		{0,0,0,0,0,MATE_VALUE},
	};
}