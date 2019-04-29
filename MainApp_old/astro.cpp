// astro.cpp

#include <cmath>
#include <iomanip>
#include <cstdio>
#include <time.h>
#define DLL_CLASS __declspec(dllexport)

#include "astro.h"
#include "PublicTypes.h"

// Atomic Time
const double dTA = 32.184;
const double J2000D = 2451545.0;
//using namespace CSTK::BASELINE;
using namespace CSTK;
static const LeapSecondRecord LeapSeconds[] =
{
    { 10, 2441317.5 }, // 1 Jan 1972
    { 11, 2441499.5 }, // 1 Jul 1972
    { 12, 2441683.5 }, // 1 Jan 1973
    { 13, 2442048.5 }, // 1 Jan 1974
    { 14, 2442413.5 }, // 1 Jan 1975
    { 15, 2442778.5 }, // 1 Jan 1976
    { 16, 2443144.5 }, // 1 Jan 1977
    { 17, 2443509.5 }, // 1 Jan 1978
    { 18, 2443874.5 }, // 1 Jan 1979
    { 19, 2444239.5 }, // 1 Jan 1980
    { 20, 2444786.5 }, // 1 Jul 1981
    { 21, 2445151.5 }, // 1 Jul 1982
    { 22, 2445516.5 }, // 1 Jul 1983
    { 23, 2446247.5 }, // 1 Jul 1985
    { 24, 2447161.5 }, // 1 Jan 1988
    { 25, 2447892.5 }, // 1 Jan 1990
    { 26, 2448257.5 }, // 1 Jan 1991
    { 27, 2448804.5 }, // 1 Jul 1992
    { 28, 2449169.5 }, // 1 Jul 1993
    { 29, 2449534.5 }, // 1 Jul 1994
    { 30, 2450083.5 }, // 1 Jan 1996
    { 31, 2450630.5 }, // 1 Jul 1997
    { 32, 2451179.5 }, // 1 Jan 1999
    { 33, 2453736.5 }, // 1 Jan 2006
};


static const char* MonthAbbrList[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

double /*CSTK::BASELINE::*/secondsToJulianDate(double sec)
{
    return sec / 86400.0;
}

double /*CSTK::BASELINE::*/julianDateToSeconds(double jd)
{
    return jd * 86400.0;
}

//角分秒表示的角度和小数表示的角度的互换

AeroTime::AeroTime()
{
    year = 0;
    month = 0;
    day = 0;
    hour = 0;
    minute = 0;
    seconds = 0.0;
    wday = 0;
    utc_offset = 0;
    tzname = "UTC";
}

AeroTime::AeroTime(int Y, int M, int D)
{
    year = Y;
    month = M;
    day = D;
    hour = 0;
    minute = 0;
    seconds = 0.0;
    wday = 0;
    utc_offset = 0;
    tzname = "UTC";
}

AeroTime::AeroTime(int Y,int M,int D,int Hou,int Min,double Sec,string strtz)
{
	year = Y;
	month = M;
	day = D;
	hour = Hou;
	minute = Min;
	seconds = Sec;	
	wday = 0;
	utc_offset = 0;
	tzname = "UTC";
}
//儒勒日转换到日历时间
AeroTime::AeroTime(double jd)
{
    bool error=true;


	::int64 a = (::int64) floor(jd + 0.5);
	wday = (a + 1) % 7;
	if (wday < 0) wday += 7;
	double c;
	if (a < 2299161)
	{
		c = (double) (a + 1524);
	}
	else
	{
		double b = (double) ((::int64) floor((a - 1867216.25) / 36524.25));
		c = a + b - (::int64) floor(b / 4) + 1525;
	}

	::int64 d = (::int64) floor((c - 122.1) / 365.25);
	::int64 e = (::int64) floor(365.25 * d);
	::int64 f = (::int64) floor((c - e) / 30.6001);

	double dday = c - e - (::int64) floor(30.6001 * f) + ((jd + 0.5) - a);

	// This following used to be 14.0, but gcc was computing it incorrectly, so
	// it was changed to 14
	month = (int) (f - 1 - 12 * (::int64) (f / 14));
	year = (int) (d - 4715 - (::int64) ((7.0 + month) / 10.0));
	day = (int) dday;

	double dhour = (dday - day) * 24;
	hour=(int)dhour;
	

	double dminute = (dhour - hour) * 60;
	//if (dminute>59.9999)
	//{
	//	dminute=0;
	//	hour++;
	//}
	//if (dminute<0.00001)
	//{
	//	dminute=0;
	//}
	minute = (int) dminute;

	seconds = (dminute - minute) * 60;
	utc_offset = 0;

	if (month < 1 || month > 12 || hour > 23 || minute > 59 || seconds >= 60.0 || seconds < 0.0)
        error=true;
	else
        error=false;

	
    tzname = "UTC";
}

void AeroTime::double2At(double jd)
{


}

//四舍五入
char* AeroTime::toStkTimeStr1(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];

	/*switch(format)
	{
	case Locale:
	case TZName:
		if((int)floor(seconds+0.5)==60)
		{
			seconds=0;
			minute++;
			if(minute==60)
			{
				minute=0;
				hour++;
				if(hour==24)
				{
					hour=0;
					day++;
					if(day==32)
					{
						day=1;
						month++;
						if(month==13)
						{
							month=1;
							year++;
						}

					}
				}
			}
		}
		else
			seconds=(int)floor(seconds+0.5);
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%02d", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, (int)ceil(seconds));
		
		sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, (int)seconds);
		break;
	}*/
	
	int y = year, m = month;
    if (month <= 2)
    {
        y = year - 1;
        m = month + 12;
    }
    int B = -2;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        B = y / 400 - y / 100;
    }

    double tmpT= (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);
	
	tmpT=tmpT+0.5/86400;
	AeroTime tmpA(tmpT);

	sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
			tmpA.day, MonthAbbrList[tmpA.month-1], tmpA.year, 
			tmpA.hour, tmpA.minute, (int)floor(tmpA.seconds));
	
	
	return date;

}


//四舍五入-1
char* AeroTime::toStkTimeStr1_1(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];

	/*switch(format)
	{
	case Locale:
	case TZName:
		if((int)floor(seconds+0.5)==60)
		{
			seconds=0;
			minute++;
			if(minute==60)
			{
				minute=0;
				hour++;
				if(hour==24)
				{
					hour=0;
					day++;
					if(day==32)
					{
						day=1;
						month++;
						if(month==13)
						{
							month=1;
							year++;
						}

					}
				}
			}
		}
		else
			seconds=(int)floor(seconds+0.5);
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%02d", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, (int)ceil(seconds));
		
		sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, (int)seconds);
		break;
	}*/
	
	int y = year, m = month;
    if (month <= 2)
    {
        y = year - 1;
        m = month + 12;
    }
    int B = -2;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        B = y / 400 - y / 100;
    }

    double tmpT= (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);
	
	tmpT=tmpT+0.5/86400;
	AeroTime tmpA(tmpT);

	sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
			tmpA.day, MonthAbbrList[tmpA.month-1], tmpA.year, 
			tmpA.hour, tmpA.minute, (int)floor(tmpA.seconds));
	
	
	return date;

}
//舍弃小数点
char* AeroTime::toStkTimeStr_2(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	
	int y = year, m = month;
    if (month <= 2)
    {
        y = year - 1;
        m = month + 12;
    }
    int B = -2;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        B = y / 400 - y / 100;
    }

    double tmpT= (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);
	
	//tmpT=tmpT-0.5/86400;
	AeroTime tmpA(tmpT);

	sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
			tmpA.day, MonthAbbrList[tmpA.month-1], tmpA.year, 
			tmpA.hour, tmpA.minute, (int)floor(tmpA.seconds));
	
	
	return date;

}

//入
char* AeroTime::toStkTimeStr_1(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];

	int y = year, m = month;
	if (month <= 2)
	{
		y = year - 1;
		m = month + 12;
	}
	int B = -2;
	if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
	{
		B = y / 400 - y / 100;
	}

	double tmpT= (floor(365.25 * y) +
		floor(30.6001 * (m + 1)) + B + 1720996.5 +
		day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);

	tmpT=tmpT+1.0/86400;
	AeroTime tmpA(tmpT);

	sprintf(date, "%02d %s %04d %02d:%02d:%02d", 
		tmpA.day, MonthAbbrList[tmpA.month-1], tmpA.year, 
		tmpA.hour, tmpA.minute, (int)floor(tmpA.seconds));


	return date;

}

char* AeroTime::toStkTimeStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%f", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, seconds);
		sprintf(date,  "%02d %s %04d %02d:%02d:%f", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, seconds);
		break;
	}
	return date;

}

char* AeroTime::toShortStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%d/%d/%d %d:%d:%0.2f", 
		//	year, month,day,hour, minute, seconds);
		sprintf(date,  "%d/%d/%d %d:%d:%0.2f", 
			year, month,day,hour, minute, seconds);
		break;
	}
	return date;

}


char* AeroTime::toCStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
    switch(format)
    {
    case Locale:
    case TZName:
        //_snprintf(date, sizeof(date), "%04d %s %02d %02d:%02d:%02d %s", 
        //         year, MonthAbbrList[month-1], day, 
         //        hour, minute, (int)ceil(seconds), tzname.c_str());
		sprintf(date,  "%04d %s %02d %02d:%02d:%02d %s", 
			year, MonthAbbrList[month-1], day, 
			hour, minute, (int)ceil(seconds), tzname.c_str());
        break;
    case UTCOffset:
        {
            int sign = utc_offset < 0 ? -1:1;
            int h_offset = sign * utc_offset / 3600;
            int m_offset = (sign * utc_offset - h_offset * 3600) / 60;
            //_snprintf(date, sizeof(date), "%04d %s %02d %02d:%02d:%02d %c%02d%02d", 
            //        year, MonthAbbrList[month-1], day, 
            //        hour, minute, (int)ceil(seconds), (sign==1?'+':'-'), h_offset, m_offset);
			sprintf(date,  "%04d %s %02d %02d:%02d:%02d %c%02d%02d", 
				year, MonthAbbrList[month-1], day, 
				hour, minute, (int)ceil(seconds), (sign==1?'+':'-'), h_offset, m_offset);
        }
        break;
    }
    return date;

}

// 日历时间转换为儒勒日
AeroTime::operator double() const
{
    int y = year, m = month;
    if (month <= 2)
    {
        y = year - 1;
        m = month + 12;
    }
    int B = -2;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        B = y / 400 - y / 100;
    }

    return (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);
}


// 日历时间规整为字符形式表达。
bool AeroTime::parseStkDate(const string& s, AeroTime& date)
{
	int year = 0;
	unsigned int month = 1;
	unsigned int day = 1;
	unsigned int hour = 0;
	unsigned int minute = 0;
	double second = 0.0;
	char a_month[3] = {0};

	//2 Nov 2014 19:45:57.141465445
	if (sscanf(s.c_str(), "%u %s %d %u:%u:%lf",
		&day,&a_month,&year, &hour, &minute, &second) == 6)
	{
		//a_month = MonthAbbrList[1];
		//printf("mon = %s\n",MonthAbbrList[1]);
		for (int i = 1;i<13;i++)
		{
			if (MonthAbbrList[i - 1][0] == a_month[0]&&MonthAbbrList[i - 1][1] == a_month[1]&&MonthAbbrList[i - 1][2] == a_month[2])
			{
				month = i;
			}			
		}

		if (month < 1 || month > 12)
			return false;
		if (hour > 23 || minute > 59 || second >= 60.0 || second < 0.0)
			return false;

		// Days / month calculation . . .
		int maxDay = 31 - ((0xa50 >> month) & 0x1);
		if (month == 2)
		{
			// Check for a leap year
			if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
				maxDay = 29;
			else
				maxDay = 28;
		}
		if (day > (unsigned int) maxDay || day < 1)
			return false;

		date.year = year;
		date.month = month;
		date.day = day;
		date.hour = hour;
		date.minute = minute;
		date.seconds = second;

		return true;
	}

	return false;
}

// 日历时间规整为字符形式表达。
bool AeroTime::parseDate(const string& s, AeroTime& date)
{
    int year = 0;
    unsigned int month = 1;
    unsigned int day = 1;
    unsigned int hour = 0;
    unsigned int minute = 0;
    double second = 0.0;
	char a_month[3] = {0};

	//2 Nov 2014 19:45:57.141465445
    if (sscanf(s.c_str(), "%d %u %u %u:%u:%lf ",
               &year, &month, &day, &hour, &minute, &second) == 6 ||
		sscanf(s.c_str(), "%d %s %u %u:%u:%lf",
		       &year,&a_month,&day, &hour, &minute, &second) == 6 ||
		sscanf(s.c_str(), "%u %s %d %u:%u:%lf",
			   &day,&a_month,&year, &hour, &minute, &second) == 6 ||
        sscanf(s.c_str(), " %d %u %u %u:%u ",
               &year, &month, &day, &hour, &minute) == 5 ||
        sscanf(s.c_str(), " %d %u %u ", &year, &month, &day) == 3)
    {
		//a_month = MonthAbbrList[1];
		//printf("mon = %s\n",MonthAbbrList[1]);
		for (int i = 1;i<13;i++)
		{
			if (MonthAbbrList[i - 1][0] == a_month[0]&&MonthAbbrList[i - 1][1] == a_month[1]&&MonthAbbrList[i - 1][2] == a_month[2])
			{
				month = i;
			}			
		}
		
        if (month < 1 || month > 12)
            return false;
        if (hour > 23 || minute > 59 || second >= 60.0 || second < 0.0)
            return false;

        // Days / month calculation . . .
        int maxDay = 31 - ((0xa50 >> month) & 0x1);
        if (month == 2)
        {
            // Check for a leap year
            if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
                maxDay = 29;
            else
                maxDay = 28;
        }
        if (day > (unsigned int) maxDay || day < 1)
            return false;

        date.year = year;
        date.month = month;
        date.day = day;
        date.hour = hour;
        date.minute = minute;
        date.seconds = second;

        return true;
    }

    return false;
}


// TAI转化为UTC
AeroTime TAItoUTC(double tai)
{
    unsigned int nRecords = sizeof(LeapSeconds) / sizeof(LeapSeconds[0]);
    double dAT = LeapSeconds[0].seconds;
    /*double dD = 0.0;  Unused*/
    int extraSecs = 0;

    for (unsigned int i = nRecords - 1; i > 0; i--)
    {
        if (tai - secsToDays(LeapSeconds[i].seconds) >= LeapSeconds[i].t)
        {
            dAT = LeapSeconds[i].seconds;
            break;
        }
        else if (tai - secsToDays(LeapSeconds[i - 1].seconds) >= LeapSeconds[i].t)
        {
			dAT = LeapSeconds[i].seconds;
			extraSecs = LeapSeconds[i].seconds - LeapSeconds[i - 1].seconds;
			break;
        }
    }

    AeroTime utcDate(tai - secsToDays(dAT));
    utcDate.seconds += extraSecs;

    return utcDate;
}


// UTC转化为TAI
double UTCtoTAI(const AeroTime& utc)
{
    unsigned int nRecords = sizeof(LeapSeconds) / sizeof(LeapSeconds[0]);
    double dAT = LeapSeconds[0].seconds;
    double utcjd = (double) AeroTime(utc.year, utc.month, utc.day);

    for (unsigned int i = nRecords - 1; i > 0; i--)
    {
        if (utcjd >= LeapSeconds[i].t)
        {
            dAT = LeapSeconds[i].seconds;
            break;
        }
    }

    double tai = utcjd + secsToDays(utc.hour * 3600.0 + utc.minute * 60.0 + utc.seconds + dAT);

    return tai;
}


// TDT到TAI
double TTtoTAI(double tt)
{
    return tt - secsToDays(dTA);
}

double TAItoTT(double tai)
{
    return tai + secsToDays(dTA);
}

// Input is a TDB Julian Date; result is in seconds
double TDBcorrection(double tdb)
{
	static const double K  = 1.657e-3;
	static const double EB = 1.671e-2;
	static const double M0 = 6.239996;
	static const double M1 = 1.99096871e-7;
    // t is seconds from J2000.0
    double t = daysToSecs(tdb - J2000D);

    // Approximate calculation of Earth's mean anomaly
    double M = M0 + M1 * t;

    // Compute the eccentric anomaly
    double E = M + EB * sin(M);

    return K * sin(E);
}


// Convert from Terrestrial Time to Barycentric Dynamical Time
double TTtoTDB(double tt)
{
    return tt + secsToDays(TDBcorrection(tt));
}


// Convert from Barycentric Dynamical Time to Terrestrial Time
double TDBtoTT(double tdb)
{
    return tdb - secsToDays(TDBcorrection(tdb));
}


// Convert from Coordinated Universal time to Barycentric Dynamical Time
AeroTime TDBtoUTC(double tdb)
{
	return TAItoUTC(TTtoTAI(TDBtoTT(tdb)));
}

double TAItoJDUTC(double tai)
{
	unsigned int nRecords = sizeof(LeapSeconds) / sizeof(LeapSeconds[0]);
	double dAT = LeapSeconds[0].seconds;

	for (unsigned int i = nRecords - 1; i > 0; i--)
	{
		if (tai - secsToDays(LeapSeconds[i - 1].seconds) > LeapSeconds[i].t)
		{
			dAT = LeapSeconds[i].seconds;
			break;
		}
	}

	return tai - secsToDays(dAT);
}
// Convert from Barycentric Dynamical Time to local calendar if possible
// otherwise convert to UTC
AeroTime TDBtoLocal(double tdb)
{
    double tai = TTtoTAI(TDBtoTT(tdb));
    double jdutc = TAItoJDUTC(tai);
    if (jdutc < 2465442 &&
        jdutc > 2415733)
    {
        time_t time = (int)julianDateToSeconds(jdutc - 2440587.5);
        struct tm *localt = localtime(&time);
        if (localt != NULL)
        {
            AeroTime d;
            d.year = localt->tm_year + 1900;
            d.month = localt->tm_mon + 1;
            d.day = localt->tm_mday;
            d.hour = localt->tm_hour;
            d.minute = localt->tm_min;
            d.seconds = (int) localt->tm_sec;
            d.wday = localt->tm_wday;
                AeroTime utcDate = TAItoUTC(tai);
                int daydiff = d.day - utcDate.day;
                int hourdiff;
                if (daydiff == 0)
                    hourdiff = 0;
                else if (daydiff > 1 || daydiff == -1)
                    hourdiff = -24;
                else
                    hourdiff = 24;
                d.utc_offset = (hourdiff + d.hour - utcDate.hour) * 3600 
                             + (d.minute - utcDate.minute) * 60;
			if (localt->tm_isdst)
			{
				d.tzname = "DST";
			}
			else
				d.tzname = "STD";
			
           return d;
        }
    }
    return TDBtoUTC(tdb);
}

// Convert from Barycentric Dynamical Time to UTC
double UTCtoTDB(const AeroTime& utc)
{
    return TTtoTDB(TAItoTT(UTCtoTAI(utc)));
}


// Convert from TAI to Julian Date UTC. The Julian Date UTC functions should
// generally be avoided because there's no provision for dealing with leap
// seconds.
double JDUTCtoTAI(double utc)
{
    unsigned int nRecords = sizeof(LeapSeconds) / sizeof(LeapSeconds[0]);
    double dAT = LeapSeconds[0].seconds;

    for (unsigned int i = nRecords - 1; i > 0; i--)
    {
        if (utc > LeapSeconds[i].t)
        {
            dAT = LeapSeconds[i].seconds;
            break;
        }
    }

    return utc + secsToDays(dAT);
}

double AeroTime::TimeTToJulianDate(time_t ttime)
{
	tm* t = localtime(&ttime);
 	int year = t->tm_year + 1900;
 	int month = t->tm_mon + 1;
 	int day = t->tm_mday ;
 	int hour = t->tm_hour;
 	int minute = t->tm_min;
 	int seconds = t->tm_sec;
	AeroTime atime = AeroTime(year,month,day,hour,minute,(double)seconds,"UTC");
 	double jdtime = (double)atime;
 	return jdtime;


}


time_t AeroTime::JulianDateToTimeT(double jdtime)
{
    /*AeroTime atime = AeroTime(jdtime);
	CTime cdtime = CTime(atime.year,atime.month,atime.day,atime.hour,atime.minute,(int)atime.seconds);
	time_t ttime = cdtime.GetTime();
    return ttime;*/
}

::int64 AeroTime::GetMSecondsFrom1970()
{
    /*SYSTEMTIME   thisTime;
	SYSTEMTIME   startTime;   
	startTime.wYear   =   1970;   
	startTime.wMonth   =   1;   
	startTime.wDay   =   1;   
	startTime.wHour   =   0;   
	startTime.wMinute   =   0;   
	startTime.wSecond   =   0;   
	startTime.wMilliseconds   =   0;   
	//GetSystemTime(&sysTime);

	thisTime.wYear=year;
	thisTime.wMonth=month;
	thisTime.wDay=day;
	thisTime.wHour=hour;
	thisTime.wMinute=minute;
	thisTime.wSecond=(int)seconds;
	thisTime.wMilliseconds=(int)((seconds-(int)seconds)*1000);

	//单位是千万分之一秒
	FILETIME   fileTime1,   fileTime2;   
	SystemTimeToFileTime(&thisTime,   &fileTime1)   ;   
	SystemTimeToFileTime(&startTime,   &fileTime2)   ;   
	::int64   time1,   time2;
	time1   =   ((::int64)fileTime1.dwHighDateTime<<32)   +   fileTime1.dwLowDateTime; 
	time2   =   ((::int64)fileTime2.dwHighDateTime<<32)   +   fileTime2.dwLowDateTime;   
    return (time1-time2)/10000;*/
}


void AeroTime::SetTimeFrom1970(::int64 MSeconds)
{
    /*SYSTEMTIME   thisTime;
	SYSTEMTIME   startTime;   
	startTime.wYear   =   1970;   
	startTime.wMonth   =   1;   
	startTime.wDay   =   1;   
	startTime.wHour   =   0;   
	startTime.wMinute   =   0;   
	startTime.wSecond   =   0;   
	startTime.wMilliseconds   =   0;   

	FILETIME   fileTime1,   fileTime2;   
	SystemTimeToFileTime(&startTime,   &fileTime2)   ;   
	::int64   time1,   time2;
	time2   =   ((::int64)fileTime2.dwHighDateTime<<32)   +   fileTime2.dwLowDateTime;   
	
	time1   =  time2+MSeconds*(::int64)10000;

	fileTime1.dwLowDateTime = time1&0xFFFFFFFF; 
	fileTime1.dwHighDateTime= time1>>32;
	
	FileTimeToSystemTime(&fileTime1,&thisTime);

	year=thisTime.wYear;
	month=thisTime.wMonth;
	day=thisTime.wDay;
	hour=thisTime.wHour;
	minute=thisTime.wMinute;
    seconds=thisTime.wSecond+thisTime.wMilliseconds/1000.f;*/
}

char* AeroTime::toStkTimeStrCeil(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%f", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, seconds);
		sprintf(date,  "%02d %s %04d %02d:%02d:%02d", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, int(ceil(seconds)));
		break;
	}
	return date;

}

char* AeroTime::toStkTimeStrFloor(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%f", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, seconds);
		sprintf(date,  "%02d %s %04d %02d:%02d:%02d", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, int(floor(seconds)));
		break;
	}
	return date;

}

char* AeroTime::toStkTimeStr_BL(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%f", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, seconds);
		sprintf(date,  "%02d %s %04d %02d:%02d:%09.6f", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, seconds);
		break;
	}
	return date;

}
