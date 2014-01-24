#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanIndex.h"

void CIG::MotionGenerator::generateMoves( bool guiInput )
{
	if (chessboard.gameOver())
	{
		return;								//如果已经输了或赢了, 就不产生走法, 走法栈是空的.
	}

	bool result;

	do
	{
		Move logMotionStack;
		OperationStack operationStack;

		operationStack.push(CIGRuleConfig::BEGIN);
		logMotionStack.push(Motion(ChessmanIndex(), CIGRuleConfig::BEGIN));
		result = generateRecursively(logMotionStack, operationStack, guiInput);
		//operationStack.popNoReturn();
	}
	while (!result);
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), moveStack() {}

// 过程比较复杂:
// 对于运行中搜索的一步, 若全局状态栈非空, 取定栈顶为当前状态,
// 以记录棋盘栈顶为当前环境, 生成与状态栈顶相应的可能的走法和对应的结果棋盘, 入各自运行栈.
// 若当前走法运行栈非空, 取定栈顶的元素入记录栈, 开始下一阶段的搜索.			///这里适合用递归函数做, 借助程序运行栈, 而不是循环.
// 若当前棋盘和走法运行栈空, 说明该状态没有可用走法, 状态栈弹栈, 重新取定栈顶为当前状态.
// 若全局状态栈顶为end, 临时栈入全局栈, 状态栈弹栈, 重新取定栈顶为当前状态.
// 初始条件配置: operationStack.push(CIGRuleConfig::BEGIN);
bool CIG::MotionGenerator::generateRecursively( Move& logMotionStack, OperationStack& operationStack, bool guiInput /*= false*/ )
{
	//根据输入选择当前的操作.
	if (guiInput)
	{
		bool result;

		PointOrVector dist;
		UINT msg;

		GUI::drawBoard(&chessboard, &logMotionStack);
		GUI::getInput(dist, msg);

		switch (msg)
		{
			case CIG_END:
				for (int i = 0; (CIGRuleConfig::operationGraph[logMotionStack.top().operation][i] != CIGRuleConfig::NOMORE) && (i < CIGRuleConfig::END + 1); ++i)
				{
					if (CIGRuleConfig::operationGraph[logMotionStack.top().operation][i] == CIGRuleConfig::END)
					{
						if(chessboard.onChangeTurn())
						{
							chessboard.undoChangeTurn();
							moveStack.push(logMotionStack);
							return true;
						}
						else
						{
							return false;
						}
					}
				}

				return false;
				break;

			case CIG_UNDO:
				return false;
				break;

			case CIG_POINT:
			{
				Move runningMotionStack;

				CIGRuleConfig::OPERATIONS op = operationStack.top();
				int i = 0;

				for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
				{
					operationStack.push(CIGRuleConfig::operationGraph[op][i]);
				}

				Motion tmpMotion;

				Move tempMotionStack = runningMotionStack;
				OperationStack tempOperationStack = operationStack;
				int j = i;

				for (; j > 0; --j)
				{
					generateMotionsForOneOperation(tempOperationStack, logMotionStack, tempMotionStack, guiInput);			//在runnningMotionStack和MoveStack中保存了下一步所有的可能结果.
					tempOperationStack.popNoReturn();
				}

				int count = 0;

				for (int j = 0; j < tempMotionStack.size; ++j)
				{
					if (tempMotionStack[j].distination == dist )
					{
						tmpMotion = tempMotionStack[j];
						count++;
					}
				}

				if (count == 0)
				{
					return false;
				}
				else if (count > 1)
				{
					;//让玩家自己选择
					//tmpMotion =
				}

				result = false;

				for (; i > 0; --i)
				{
					generateMotionsForOneOperation(operationStack, logMotionStack, runningMotionStack, guiInput);

					while (runningMotionStack.size > 0)
					{
						if (runningMotionStack.top() == tmpMotion)
						{
							Motion& nowOperation = tmpMotion;
							logMotionStack.push(nowOperation);
							chessboard.onMotionIntent(nowOperation);

							result = generateRecursively(logMotionStack, operationStack, guiInput);

							chessboard.undoMotion(nowOperation);
							logMotionStack.popNoReturn();
						}

						runningMotionStack.popNoReturn();
					}

					operationStack.popNoReturn();
				}
			}

			break;

			default:
				break;
		}

		return result;
	}
	else
	{
		Move runningMotionStack;

		CIGRuleConfig::OPERATIONS op = operationStack.top();
		int i = 0;

		for (; CIGRuleConfig::operationGraph[op][i] != CIGRuleConfig::NOMORE; ++i)
		{
			operationStack.push(CIGRuleConfig::operationGraph[op][i]);
		}

		for (; i > 0; --i)
		{
			generateMotionsForOneOperation(operationStack, logMotionStack, runningMotionStack);

			while (runningMotionStack.size > 0)
			{
				Motion& nowOperation = runningMotionStack.top();
				logMotionStack.push(nowOperation);
				chessboard.onMotionIntent(nowOperation);

				generateRecursively(logMotionStack, operationStack, guiInput);

				chessboard.undoMotion(nowOperation);
				logMotionStack.popNoReturn();

				runningMotionStack.popNoReturn();
			}

			operationStack.popNoReturn();
		}

		return true;
	}
}

void CIG::MotionGenerator::generateMotionsForOneOperation( OperationStack& operationStack, Move& logMotionStack, Move& runningMotionStack, bool guiInput /*= false */ )
{
	CIGRuleConfig::OPERATIONS s = operationStack.top();

	switch (s)
	{
		case CIGRuleConfig::BEGIN:
			break;

		case CIGRuleConfig::ADD:
			//for (int i=0; i< (1<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) ;++i)
			//{
			//	for (int j=0;j< (1<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2);++j)
			//	{
			//		PointOrVector dist =  PointOrVector(i,j);
			//		if (chessboard[dist]||!CIGRuleConfig::BOARD_RANGE[j][i])
			//		{
			//			continue;
			//		}
			//
			//		testAndSave(s, NULL, dist, runningMotionStack);
			//	}
			//}
			break;

		case CIGRuleConfig::PICK:
		{
			const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

			if (logMotionStack.size <= 1)
			{
				for (unsigned i = 0; i < cg.size; ++i)
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
					testAndSave(s, c, c->coordinate, runningMotionStack);
				}
			}
			else				//除了跳棋, 都只可能满足上边的分支.
			{
				Chessman* c = const_cast<Chessman*> (&(cg.at(logMotionStack.top().chessmanIndex.index)));

				testAndSave(s, c, c->coordinate, runningMotionStack);
			}
		}
		break;

		case CIGRuleConfig::CAPTURE:
		case CIGRuleConfig::PUT:
		{
			ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
			Chessman* c = &chessboard.players[cl.player].ownedChessmans[cl.index];

			if (logMotionStack.top().operation == CIGRuleConfig::CAPTURE)
			{
				testAndSave(s, c, logMotionStack.top().distination, runningMotionStack);

				break;
			}

			switch (c->chessmanType)				//生成象棋走法, 改变棋子的坐标.
			{
				case CIGRuleConfig::KING:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -1 : 1) : 0, (i & 1) ? ((i & 2) ? -1 : 1) : 0 );
						PointOrVector offset = dist - PointOrVector(7, (c->chessmanIndex.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset.x[0]) & (-2) || abs(offset.x[1]) & (-2))
						{
							continue;
						}

						testAndSave(s, c, dist, runningMotionStack);
					}

					if (s == CIGRuleConfig::CAPTURE)
					{
						for (int j = 1;; ++j)
						{
							PointOrVector dist =  c->coordinate + PointOrVector(0,  (c->chessmanIndex.player == CIGRuleConfig::COMPUTER) ? j : -j);

							if (chessboard.beyondBoardRange(dist))
							{
								break;
							}
							else if ((chessboard[dist]) && (chessboard[dist]->chessmanType != CIGRuleConfig::KING))
							{
								break;
							}
							else
							{
								if(testAndSave(s, c, dist, runningMotionStack))
								{
									break;
								}
							}
						}
					}

					break;

				case CIGRuleConfig::ADVISOR:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );
						PointOrVector offset = dist - PointOrVector(7, (c->chessmanIndex.player == CIGRuleConfig::COMPUTER) ? 4 : 11);

						if (abs(offset[0]) & (-2) || abs(offset[1]) & (-2))
						{
							continue;
						}

						testAndSave(s, c, dist, runningMotionStack);
					}

					break;

				case CIGRuleConfig::ELEPHANT:
					for (int i = 0; i < 4; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( (i & 1) ? -2 : 2, (i & 2) ? -2 : 2 );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 1) ? -1 : 1, (i & 2) ? -1 : 1 );

						if (!chessboard.onSelfHalfOfBoard(dist) || chessboard[eye] != NULL)
						{
							continue;
						}

						testAndSave(s, c, dist, runningMotionStack);
					}

					break;

				case CIGRuleConfig::HORSE:
					for (int i = 0; i < 8; ++i)
					{
						PointOrVector dist = c->coordinate + PointOrVector( ((i & 1) ? -1 : 1) << ((i & 4) ? 1 : 0), ((i & 2) ? -1 : 1) << ((i & 4) ? 0 : 1) );
						PointOrVector eye = c->coordinate + PointOrVector( (i & 4) ? ((i & 1) ? -1 : 1) : 0, (i & 4) ? 0 : ((i & 2) ? -1 : 1) );

						if (chessboard.beyondBoardRange(dist) || chessboard[eye] != NULL)
						{
							continue;
						}

						testAndSave(s, c, dist, runningMotionStack);
					}

					break;

				case CIGRuleConfig::CHAROIT:
					for (int i = 0; i < 4; ++i)
					{
						for (int j = 1;; ++j)
						{
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessboard.beyondBoardRange(dist))
							{
								break;
							}
							else if ((chessboard[dist]) && (chessboard[dist]->chessmanIndex.player == c->chessmanIndex.player))
							{
								break;
							}
							else
							{
								if (s == CIGRuleConfig::CAPTURE)
								{
									if(testAndSave(s, c, dist, runningMotionStack))
									{
										break;
									}
								}
								else
								{
									if(!testAndSave(s, c, dist, runningMotionStack))
									{
										break;
									}
								}
							}
						}
					}

					break;

				case CIGRuleConfig::CANNON:
					for (int i = 0; i < 4; ++i)
					{
						bool haveEmplacement = false;

						for (int j = 1;; ++j)
						{
							PointOrVector dist =  c->coordinate + PointOrVector( ((i & 1) == 0) ? ((i & 2) ? -j : j) : 0, (i & 1) ? ((i & 2) ? -j : j) : 0 );

							if (chessboard.beyondBoardRange(dist))
							{
								break;
							}
							else if (chessboard[dist] == NULL)			//dist在棋盘上, 没有棋子
							{
								if ((s == CIGRuleConfig::PUT) && (!haveEmplacement))				//沿途没有 "炮架"
								{
									testAndSave(s, c, dist, runningMotionStack);
								}
							}
							else if (haveEmplacement)							//有棋子, 有炮架
							{
								testAndSave(s, c, dist, runningMotionStack);

								break;
							}
							else   															  //有棋子, 没炮架
							{
								if (s == CIGRuleConfig::CAPTURE)
								{
									haveEmplacement = true;
								}
								else
								{
									break;										//put状态到头了, 退出本方向
								}
							}
						}
					}

					break;

				case CIGRuleConfig::PAWN:
				{
					if (chessboard.onSelfHalfOfBoard(c->coordinate))
					{
						PointOrVector dist = c->coordinate + PointOrVector(0, (c->chessmanIndex.player == CIGRuleConfig::COMPUTER) ? 1 : -1);
						testAndSave(s, c, dist, runningMotionStack);
					}
					else
					{
						for (int i = -1; i < 2; i += 1)
						{
							PointOrVector dist = c->coordinate + PointOrVector( i, (c->chessmanIndex.player == CIGRuleConfig::COMPUTER) ? (!i) : (-!i) );

							testAndSave(s, c, dist, runningMotionStack);
						}
					}
				}
				break;

				default:
					break;
			}
		}
		break;

		case CIGRuleConfig::END:
			if (!guiInput)
			{
				if (chessboard.onChangeTurn())
				{
					chessboard.undoChangeTurn();
					moveStack.push(logMotionStack);
				}
			}

			break;

		default:
			break;
	}
}

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, Move& runningMotionStack )
{
	switch (s)
	{
		case CIG::CIGRuleConfig::BEGIN:
			break;

		case CIG::CIGRuleConfig::ADD:
		{
			Motion tmpMotion(ChessmanIndex(), CIGRuleConfig::ADD, dist);
			runningMotionStack.push(tmpMotion);
			return true;
		}
		break;

		case CIG::CIGRuleConfig::PICK:
		{
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c, c->coordinate);
				Motion tmpMotion(c->chessmanIndex, CIGRuleConfig::PICK, c->coordinate);
				runningMotionStack.push(tmpMotion);
			}
		}
		break;

		case CIG::CIGRuleConfig::PUT:
		{
			PointOrVector preP(c->coordinate);

			if (chessboard.onPutIntent(c, dist))
			{
				chessboard.undoPut(c, preP);
				Motion tmpMotion(c->chessmanIndex, s, dist, preP);
				runningMotionStack.push(tmpMotion);

				return true;
			}
		}
		break;

		case CIG::CIGRuleConfig::CAPTURE:
		{
			Chessman* temp = chessboard[dist];

			if (chessboard.onCaptureIntent(c, dist))
			{
				chessboard.undoCaptured(temp);
				Motion tmpMotion(temp->chessmanIndex, s, dist);
				runningMotionStack.push(tmpMotion);

				return true;
			}
		}
		break;

		case CIG::CIGRuleConfig::PROMOTION:
			break;

		case CIG::CIGRuleConfig::DECOVER:
			break;

		case CIG::CIGRuleConfig::END:
			break;

		default:
			break;
	}

	return false;
}

