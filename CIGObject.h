#ifndef __CIGOBJECT_H__
#define __CIGOBJECT_H__

#include "utilities.h"
#include "CIGRuleConfig.h"
#include "GUI.h"
/*************************************************************************
  可配置智能系统各部件的基类, 管理类型名和类型成员的个数.
  要求类名字符串生命周期长于该类.
**************************************************************************/
namespace CIG
{
	class CIGObject
	{
		public:
			inline CIGObject() {};
			inline CIGObject(const CIGObject& o) {};
			inline virtual ~CIGObject() {};

			inline virtual void informError(const string& str)const
			{
				ostringstream oss;
				oss << *this;
				GUI::inform(str + oss.str(), true);		///C++静态函数成员的引用用域符号.
			}

			friend ostream& operator << (ostream& os, const CIGObject& o)
			{
				ostringstream oss;
				oss << o;
				os << oss.str();
				return os;
			}

			friend ostringstream& operator<<(ostringstream& oss, const CIGObject& o)						///不加引用符号, 就会调用拷贝构造函数, id管理得乱七八糟.
			{
				oss << "CIGObject::\n*******************************************************************************\n";				// TO-DO 输出字符串
				return oss;
			}
	};
}
#endif /*__CIGOBJECT_H_*/

