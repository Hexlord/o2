#include "TimeStamp.h"

namespace o2
{
    bool TimeStamp::operator!=(const TimeStamp& wt) const
    {
        return !(*this == wt);
    }
    
    bool TimeStamp::operator==(const TimeStamp& wt) const
    {
        return mSecond == wt.mSecond && mMinute == wt.mMinute && mHour == wt.mHour && mDay == wt.mDay && mMonth == wt.mMonth &&
        mYear == wt.mYear;
    }
    
    TimeStamp::TimeStamp(int seconds /*= 0*/, int minutes /*= 0*/, int hours /*= 0*/, int days /*= 0*/, int months /*= 0*/, int years /*= 0*/):
        mYear(years), mMonth(months), mDay(days), mHour(hours), mMinute(minutes), mSecond(seconds)
    {}
}

CLASS_META(o2::TimeStamp)
{
	BASE_CLASS(o2::ISerializable);

	PUBLIC_FIELD(mYear).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mMonth).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mDay).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mHour).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mMinute).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mSecond).SERIALIZABLE_ATTRIBUTE();
}
END_META;
 
