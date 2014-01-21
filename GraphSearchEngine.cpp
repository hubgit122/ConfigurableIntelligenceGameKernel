#include "stdafx.h"
#include "GraphSearchEngine.h"
#include "ChessBoard.h"
#include "MotionGenerator.h"
#include <time.h>

namespace CIG
{
	const float GraphSearchEngine::MAX_SEARCH_TIME = 10000;		//10s

	//************************************
	// Method:    alphaBetaSearch
	// FullName:  CIG::GraphSearchEngine::alphaBetaSearch
	// Access:    private static 
	// Returns:   int
	// Qualifier:
	// Parameter: int alpha
	// Parameter: int beta
	// Parameter: int depth
	// ��������Ķ��˰�, Ӧ����������:
	// ԭ���ǵ�ǰ��ҿ����޲�, �õ��ڱ�����Ҳ�߶��Լ����������߷�ʱ, �Լ����ܻ�õ��������Ľ��. 
	// ��������С���������ͬ���ڵõ���ǰ�������������һ��֮�����ܵõ�����÷���������ֱ����ǰһ��ȡ�෴���õ���, ��Ӧ��ֱ�����¼��������ر����Ŀ���������. 
	//************************************
	int GraphSearchEngine::alphaBetaSearch( int alpha, int beta, int depth)
	{
		int vl, vlBest;
		Move nowBestMove;
		Chessboard& nowBoard = *pChessboard;
		// һ��Alpha-Beta��ȫ������Ϊ���¼����׶�

		// 1. ����ˮƽ�ߣ��򷵻ؾ�������ֵ
		if (depth == 0)
		{
			return nowBoard.getEvaluation((CIGRuleConfig::PLAYER_NAMES)((nowBoard.nowTurn-1)<0?(CIGRuleConfig::PLAYER_NUM-1):nowBoard.nowTurn-1));
		}

		// 2. ��ʼ�����ֵ������߷�
		vlBest = -Chessboard::MATE_VALUE; // ��������֪�����Ƿ�һ���߷���û�߹�(ɱ��)
		//bestMove.clear();           // ��������֪�����Ƿ���������Beta�߷���PV�߷����Ա㱣�浽��ʷ��

		// 3. ����ȫ���߷�����������ʷ������			���������, û���������.
		MotionGenerator mg(nowBoard);
		mg.generateMoves();

		MoveStack& runningMoveStack = mg.moveStack;

		//qsort(mvs, nGenMoves, sizeof(int), CompareHistory);		//����CompareHistory�Ķ���, �����ɴ�С����.

		// 4. ��һ����Щ�߷��������еݹ�
		for (int i = runningMoveStack.size -1; i >=0 ; --i)
		{
			Move& nowMove = runningMoveStack[i];

			nowBoard.onWholeMoveIntent(nowMove,true);
			vl = alphaBetaSearch(-beta, -alpha, depth - 1);
			nowBoard.undoWholeMove(nowMove,true);

			// 5. ����Alpha-Beta��С�жϺͽض�
			if (vl > vlBest)      // �ҵ����ֵ(������ȷ����Alpha��PV����Beta�߷�)
			{
				vlBest = vl;        // "vlBest"����ĿǰҪ���ص����ֵ�����ܳ���Alpha-Beta�߽�

				if (vl >= beta)   // �ҵ�һ��Beta�߷�
				{
					nowBestMove.forceCopyFrom(runningMoveStack[i]);		// TO-DO���浽��ʷ����ʲô��? Ϊʲô��beta?
					break;            // Beta�ض�
				}
				else if (vl > alpha)   // �ҵ�һ��PV�߷�
				{
					nowBestMove.forceCopyFrom(runningMoveStack[i]);
					alpha = vl;     // ��СAlpha-Beta�߽�
				}
			}
		}

		// 5. �����߷����������ˣ�������߷�(������Alpha�߷�)���浽��ʷ���������ֵ
		if (vlBest <= -Chessboard::WIN_VALUE )
		{
			// �����ɱ�壬�͸���ɱ�岽����������
			return nowBoard.nowRound - Chessboard::MATE_VALUE;
		}

		if (nowBestMove.size != 0)
		{
			//// �������Alpha�߷����ͽ�����߷����浽��ʷ��
			//Search.nHistoryTable[mvBest] += nDepth * nDepth;

			if (depth==rootDepth)
			{
				// �������ڵ�ʱ��������һ������߷�(��Ϊȫ�����������ᳬ���߽�)��������߷���������
				GraphSearchEngine::bestMove.forceCopyFrom(nowBestMove);
			}
		}

		//return vlBest;						//ͨ��vlBestѡ������߷�, ���ǲ����ܷ������ֵ, ���Ƕ�Ӧ��ǰһ��ҵ�����ֵ. 
		nowBoard.onMoveIntent(nowBestMove,true);
		int preBest = nowBoard.getEvaluation((CIGRuleConfig::PLAYER_NAMES)((nowBoard.nowTurn-1)<0?(CIGRuleConfig::PLAYER_NUM-1):nowBoard.nowTurn-1));
		nowBoard.undoMove(nowBestMove,true);
		
		return preBest;
	}

	void GraphSearchEngine::makeBestMove( Chessboard*chessboard, void* move )
	{
		int t = clock();
		bestMove.clear();
		GraphSearchEngine::pChessboard = chessboard;
		// TO-DO ������ʷ��

		//for (int i=1; i<=LIMIT_DEPTH; ++i)
		//{
			rootDepth =/*i*/LIMIT_DEPTH; 

			int vl = alphaBetaSearch( -Chessboard::MATE_VALUE, Chessboard::MATE_VALUE, /*i*/LIMIT_DEPTH);

			//// ������ɱ�壬����ֹ����
			//if (vl > Chessboard::WIN_VALUE || vl < -Chessboard::WIN_VALUE)			//Ϊ�˺ͼ�¼��ȵĻ��Ʊ���һ��, WIN_VALUE = MATE_VALUE - 100
			//{
			//	break;
			//}

			//// ���� MAX_SEARCH_TIME������ֹ����
			//if (clock() - t > MAX_SEARCH_TIME)
			//{
			//	break;
			//}
		//}
		(*((Move*)move)).forceCopyFrom(bestMove);
	}

	int GraphSearchEngine::rootDepth;

	Chessboard* GraphSearchEngine::pChessboard = NULL;

	const int GraphSearchEngine::LIMIT_DEPTH = 3;    // �����������

	CIG::Move GraphSearchEngine::bestMove;
}