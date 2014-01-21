#include "stdafx.h"
#include "MotionGenerator.h"
#include "Array.h"
#include "Chessboard.h"
#include "Player.h"
#include "Chessman.h"
#include "ChessmanIndex.h"

void CIG::MotionGenerator::generateMoves( bool guiInput )
{
	if (chessboard.loose[chessboard.nowTurn]||chessboard.win[chessboard.nowTurn])
	{
		return;								//����Ѿ����˻�Ӯ��, �Ͳ������߷�, �߷�ջ�ǿյ�. 
	}

	bool result;
	do
	{
		if (guiInput)
		{
			GUI::inform("�����壡");
		}
		Move logMotionStack;
		OperationStack operationStack;

		operationStack.push(CIGRuleConfig::BEGIN);
		result = generateRecursively(logMotionStack, operationStack, guiInput);
		//operationStack.popNoReturn();
	} while (!result);
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

		GUI::drawBoard(&chessboard,&logMotionStack);
		GUI::getInput(dist, msg);

		switch (msg)
		{
		case CIG_END:
			if (chessboard.onChangeTurn())
			{
				chessboard.undoChangeTurn();
				moveStack.push(logMotionStack);
				return true;			//�Ѿ���moveStack����. 
			}
			else
			{
				result = false;
			}
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
				for (int j=0; j<tempMotionStack.size; ++j)
				{
					if (tempMotionStack[j].distination == dist )
					{
						tmpMotion = tempMotionStack[j];
						count++;
					}
				}

				if (count==0)
				{
					result = false;
					break;
				}
				else if (count>1)
				{
					;//������Լ�ѡ��
					//optemp = 
				}

				result = false;
				for (; i > 0; --i)
				{
					generateMotionsForOneOperation(operationStack, logMotionStack, runningMotionStack, guiInput);
					while (runningMotionStack.size > 0)
					{
						if (runningMotionStack.top()==tmpMotion)
						{
							Motion& nowOperation = tmpMotion;
							logMotionStack.push(nowOperation);
							chessboard.onOperationIntent(nowOperation);

							result = generateRecursively(logMotionStack, operationStack, guiInput);

							chessboard.undoOperation(nowOperation);
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
				chessboard.onOperationIntent(nowOperation);

				generateRecursively(logMotionStack, operationStack);

				chessboard.undoOperation(nowOperation);
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
	CIGRuleConfig::OPERATIONS s = operationStack.top();							// TO-DO ֱ��ʹ��s��Ϊ��������. 

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

				if (logMotionStack.size==0)
				{
					for (unsigned i = 0; i < cg.size; ++i)
					{
						Chessman* c = const_cast<Chessman*> (&(cg.at(i)));
						testAndSave(s, c, c->coordinate,runningMotionStack);
					}
				}
				else
				{
					Chessman* c = const_cast<Chessman*> (&(cg.at(logMotionStack.top().chessmanIndex.index)));
					
					testAndSave(s, c, c->coordinate,runningMotionStack);
				}
			}
			break;

		case CIGRuleConfig::PUT:
			//{
			//	ChessmanIndex& cl = chessboard.pickedChessmanByIndex[-1];
			//	Chessman* c = &chessboard.players[cl.player].ownedChessmans[cl.index];

			//	if(logMotionStack.size>=3)
			//	{
			//		PointOrVector diff = (logMotionStack[-3].distination-logMotionStack[-2].distination);
			//		if (abs(diff[0]|diff[1])==1)			//����
			//		{
			//			return;
			//		}
			//	}

			//	for (int i=GUI::LINE_DIRECTION.size()-1; i>=0; --i)
			//	{
			//		for (int j=-1; j<2; j+=2)
			//		{
			//			PointOrVector& delt = j*GUI::LINE_DIRECTION.at(i);
			//			Chessman* oneStep = chessboard[c->coordinate+delt];
			//			Chessman* twoStep = chessboard[c->coordinate+2*delt];
			//			PointOrVector dist;

			//			if (logMotionStack.size==1)			//��δȷ���Ƿ�����
			//			{
			//				if (oneStep&&twoStep)
			//				{
			//					continue;
			//				}
			//				else if (oneStep)
			//				{
			//					dist = c->coordinate+2*delt;
			//				}
			//				else
			//				{
			//					dist = c->coordinate+delt;
			//				}
			//			}
			//			else														//�Ѿ�ȷ��Ϊ����
			//			{
			//				if (oneStep&&!twoStep)
			//				{
			//					dist = c->coordinate + 2*delt;
			//				}
			//				else
			//				{
			//					continue;
			//				}

			//				bool flag = false;
			//				for (int j=logMotionStack.size-1;j>=0;--j)
			//				{
			//					if (logMotionStack[j].operation==CIGRuleConfig::PICK&&logMotionStack[j].distination==dist)
			//					{
			//						flag = true;
			//						break;
			//					}
			//				}
			//				if (flag)
			//				{
			//					continue;
			//				}
			//			}

			//			testAndSave(s, c, dist, runningMotionStack);
			//		}
			//	}
			//}
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

bool CIG::MotionGenerator::testAndSave( CIGRuleConfig::OPERATIONS s, Chessman* c, PointOrVector dist, Move &runningMotionStack )
{
	switch (s)
	{
	case CIG::CIGRuleConfig::BEGIN:
		break;
	case CIG::CIGRuleConfig::ADD:
		{
			Motion optemp(ChessmanIndex(),CIGRuleConfig::ADD,dist);
			runningMotionStack.push(optemp);
			return true;
		}
		break;
	case CIG::CIGRuleConfig::PICK:
		{
			if (chessboard.onPickIntent(c))
			{
				chessboard.undoPick(c,c->coordinate);
				Motion optemp(c->chessmanIndex, CIGRuleConfig::PICK,c->coordinate);
				runningMotionStack.push(optemp);
			}
		}
		break;
	case CIG::CIGRuleConfig::PUT:
		{
			PointOrVector preP(c->coordinate);
			if (chessboard.onPutIntent(c, dist))
			{
				chessboard.undoPut(c);
				c->coordinate = preP;
				Motion optemp(c->chessmanIndex, s, dist);
				runningMotionStack.push(optemp);

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
				Motion optemp(temp->chessmanIndex, s, dist);
				runningMotionStack.push(optemp);

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

