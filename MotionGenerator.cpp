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
		return;								//����Ѿ����˻�Ӯ��, �Ͳ������߷�, �߷�ջ�ǿյ�.
	}

	bool result;

	do
	{
		Move logMotionStack;
		OperationStack operationStack;

		operationStack.push(CIGRuleConfig::BEGIN);
		result = generateRecursively(logMotionStack, operationStack, guiInput);
		//operationStack.popNoReturn();
	}
	while (!result);
}

CIG::MotionGenerator::MotionGenerator(Chessboard& cb)
	: chessboard(cb), moveStack() {}

// ���̱Ƚϸ���:
// ����������������һ��, ��ȫ��״̬ջ�ǿ�, ȡ��ջ��Ϊ��ǰ״̬,
// �Լ�¼����ջ��Ϊ��ǰ����, ������״̬ջ����Ӧ�Ŀ��ܵ��߷��Ͷ�Ӧ�Ľ������, ���������ջ.
// ����ǰ�߷�����ջ�ǿ�, ȡ��ջ����Ԫ�����¼ջ, ��ʼ��һ�׶ε�����.			///�����ʺ��õݹ麯����, ������������ջ, ������ѭ��.
// ����ǰ���̺��߷�����ջ��, ˵����״̬û�п����߷�, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ȫ��״̬ջ��Ϊend, ��ʱջ��ȫ��ջ, ״̬ջ��ջ, ����ȡ��ջ��Ϊ��ǰ״̬.
// ��ʼ��������: operationStack.push(CIGRuleConfig::BEGIN);
bool CIG::MotionGenerator::generateRecursively( Move& logMotionStack, OperationStack& operationStack, bool guiInput /*= false*/ )
{
	//��������ѡ��ǰ�Ĳ���.
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
				for (int i=0; (CIGRuleConfig::operationGraph[logMotionStack.top().operation][i]!=CIGRuleConfig::NOMORE)&&(i<CIGRuleConfig::END+1); ++i)
				{
					if (CIGRuleConfig::operationGraph[logMotionStack.top().operation][i]==CIGRuleConfig::END)
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

				//��tempMotionStack���������ж���, �����END, ������moveStack��
				Move tempMotionStack = runningMotionStack;
				OperationStack tempOperationStack = operationStack;
				int j = i;

				for (; j > 0; --j)
				{
					generateMotionsForOneOperation(tempOperationStack, logMotionStack, tempMotionStack, guiInput);			//��runnningMotionStack��MoveStack�б�������һ�����еĿ��ܽ��.
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
					result = false;
					break;
				}
				else if (count > 1)
				{
					;//������Լ�ѡ��
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
			for (int i=0; i< (1<<CIGRuleConfig::INI_BOARD_WIDTH_LOG2) ;++i)
			{
				for (int j=0;j< (1<<CIGRuleConfig::INI_BOARD_HEIGHT_LOG2);++j)
				{
					PointOrVector dist =  PointOrVector(i,j);
					if (chessboard[dist]||!CIGRuleConfig::BOARD_RANGE[j][i])
					{
						continue;
					}
			
					testAndSave(s, NULL, dist, runningMotionStack);
				}
			}
			break;

		case CIGRuleConfig::PICK:
		{
			const Stack<Chessman, CIGRuleConfig::INI_CHESSMAN_GROUP_SIZE, 0>& cg = chessboard.players[chessboard.nowTurn].ownedChessmans;

			if (logMotionStack.size == 0)
			{
				for (unsigned i = 0; i < cg.size; ++i)
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
					testAndSave(s, c, c->coordinate, runningMotionStack);
				}
			}
			else				//��������, ��ֻ���������ϱߵķ�֧.
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

			switch (c->chessmanType)				//���������߷�, �ı����ӵ�����.
			{
			case CIGRuleConfig::CHESS:
				ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
				testAndSave(s, &chessboard.players[cl.player].ownedChessmans[cl.index], logMotionStack.top().distination, runningMotionStack);
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

