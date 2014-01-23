
// CConfigurableIntelligenceGameView.h : CConfigurableIntelligenceGameView ��Ľӿ�
//


#pragma once

#include "Stack.h"
#include "Chessboard.h"
#include "CIGRuleConfig.h"
using namespace CIG;

// CConfigurableIntelligenceGameView ����

class CConfigurableIntelligenceGameView : public CWnd
{
		// ����
	public:
		CConfigurableIntelligenceGameView();

		// ����
	public:
		CWinThread* m_GameThread;
		CEvent workThreadOK;
		CEvent moveComplete;
		CIG::Move moveOfLastRound;
		CIG::Chessboard nowBoard;
		bool getMove;
		int nBoardBaseDC;
		int nChessmanDC[CIGRuleConfig::PLAYER_NUM][CIGRuleConfig::CHESSMAN_TYPE_NUM];
		CBitmap boardBaseBitmap;
		CBitmap chessmanBaseBitmap;

		// ����
	public:
		void DrawBoard(Chessboard* cb = NULL, Move* move = NULL);
		void WrapChessWithFrame(CDC& dc, PointOrVector logicCoo, COLORREF color = RGB(255, 0, 0), bool rectangleNotCircle = true, bool fill = false);

		void GenerateBoardBaseDC(CDC& boardBaseDC, CBitmap* pBoardBaseDCBmpOld);

		void GenerateChessmanDC(CDC& chessmanDC, CBitmap* pChessmanDCBmpOld);

		// ��д
	protected:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

		// ʵ��
	public:
		virtual ~CConfigurableIntelligenceGameView();

		// ���ɵ���Ϣӳ�亯��
	protected:
		afx_msg void OnPaint();
		DECLARE_MESSAGE_MAP()
	public:
		//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnGameNew();
	protected:
		afx_msg LRESULT OnMoveComplete(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT OnGetMove(WPARAM wParam, LPARAM lParam);
	public:
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		//		afx_msg void OnClose();
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	protected:
		afx_msg LRESULT OnOperationText(WPARAM wParam, LPARAM lParam);
};

