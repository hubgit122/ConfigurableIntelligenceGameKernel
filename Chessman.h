
#ifndef __CHESSMAN_H__
#define __CHESSMAN_H__

#include "utilities.h"
#include "CIGRuleConfig.h"
#include "ChessmanIndex.h"
#include "CIGObject.h"

namespace CIG
{
	class Chessman: public CIGObject
	{
		private:
			Chessman();
		public:
			inline virtual ~Chessman() {};
			inline Chessman(const Chessman& c)
			{
				memcpy(this, &c, sizeof(Chessman));
			};
			inline Chessman(CIGRuleConfig::CHESSMAN_TYPES t, const PointOrVector& c, CIGRuleConfig::PLAYER_NAMES p, int index, CIGRuleConfig::CHESSMAN_STATUS s, CIGRuleConfig::VISIBILITIES v)
				: chessmanType(t), coordinate(c), chessmanIndex(p, index), status(s)
			{
				for (int i = 0 ; i < CIGRuleConfig::PLAYER_NUM; ++i)
				{
					this->visibility[i] = CIGRuleConfig::VISIBILITIES::ALL;
				}
			}
			CIGRuleConfig::CHESSMAN_TYPES chessmanType;
			struct PointOrVector coordinate;
			ChessmanIndex chessmanIndex;
			CIGRuleConfig::CHESSMAN_STATUS status;
			CIGRuleConfig::VISIBILITIES visibility[CIGRuleConfig::PLAYER_NUM];
			//ChessmanGroup& chessmanGroup;
			void operator = (const Chessman& c);

			friend std::ostringstream& operator<<(std::ostringstream& oss, const Chessman& c)
			{
				oss << "Chessman::\n" << "\tchessmanType: " << c.chessmanType << "\n\tcoordinate: ";
				oss << c.coordinate << "\nChessmanIndex.player: " << c.chessmanIndex.player << '\n';
				return oss;
			}
			//************************************
			// Method:    onXXIntent
			// FullName:  CIG::Chessman::onIntent
			// Access:    virtual public
			// Returns:   bool
			// Qualifier:
			// Parameter: CIGConfig::Operation op, xx
			// ��ѯ���������, ����������״̬.
			// �麯��, Ĭ��һ�в�������ͨ�����. ����������Ҫ, Ӧ�����¶���.
			//************************************
			virtual bool onPickIntent();
			virtual bool onPutIntent(PointOrVector p);
			virtual bool onCapturedIntent();
			virtual bool onCaptureIntent(Chessman* c);
			virtual bool onPromotionIntent(CIGRuleConfig::CHESSMAN_TYPES t);

			virtual void undoPick();
			virtual void undoPut();
			virtual void undoCaptured();
	};
}

#endif /*__CHESSMAN_H_*/

