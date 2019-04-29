#ifndef _CELENGINE_ASTRO_H_
#define _CELENGINE_ASTRO_H_
//************************************************
//ʱ�����
//************************************************
#include <iostream>
#include <string>
#include <cmath>
#include "PublicTypes.h"

#ifdef DLL_CLASS
#else
#define DLL_CLASS __declspec(dllimport)
#endif

using namespace std;
namespace CSTK
{
    class  AeroTime
    {
    public:
		//���캯��
        AeroTime(void);
        AeroTime(int Y, int M, int D);
		AeroTime(int Y,int M,int D,int Hou,int Min,double Sec,string strtz);
        AeroTime(double);

		void double2At(double);

		//���ַ���ת��ΪAeroTime����
		static bool parseStkDate(const std::string&, AeroTime&);
		static bool parseDate(const std::string&, AeroTime&);
		static double TDBcorrection(double tdb);

		// Convert to and from UTC dates
		static double TAItoJDUTC(double tai);
		static double UTCtoTAI(const AeroTime& utc);
		static AeroTime TAItoUTC(double tai);
		static double UTCtoTDB(const AeroTime& utc);
		static AeroTime TDBtoUTC(double tdb);
		static AeroTime TDBtoLocal(double tdb);
 
		//Convert among uniform time scales
		static double TTtoTAI(double tt);
		static double TAItoTT(double tai);
		static double TTtoTDB(double tt);
		static double TDBtoTT(double tdb);
		static double JDUTCtoTAI(double utc);

		static double secondsToJulianDate(double);
		static double julianDateToSeconds(double);

		//time_t���͵�double���͵�ת��
		static double TimeTToJulianDate(time_t ttime);

		//double���͵�time_t���͵�ת��
		static time_t JulianDateToTimeT(double jdtime);

		//���ã��õ�1970��1��1�������ĺ�����
		::int64 GetMSecondsFrom1970();
		void SetTimeFrom1970(::int64 MSeconds);
		
		//�õ�һ����ʱ���double��ʾ
		double GetDoubleHour()
		{
			return hour + minute*1.0/60 + seconds*1.0/3600;
		}

        enum Format
        {
            Locale          = 0,
            TZName          = 1,
            UTCOffset       = 2,
        };
		//��ʱ��ת��Ϊ�ַ�����ʾ
        char* toCStr(Format format = Locale);
		char* toStkTimeStr(Format format = Locale);
		char* toStkTimeStr1(Format format = Locale);
		char* toStkTimeStr1_1(Format format = Locale);
		char* toStkTimeStr_1(Format format = Locale);
		char* toStkTimeStr_2(Format format = Locale);
		char* toShortStr(Format format = Locale);
		char* toStkTimeStrCeil(Format format = Locale);
		char* toStkTimeStrFloor(Format format = Locale);
		char* toStkTimeStr_BL(Format formatt = Locale);
		
		//��AeroTimeʱ��ת��Ϊ�����ձ�ʾ
        operator double() const;


    public:
        int year;
        int month;
        int day;
        int hour;
        int minute;
		double seconds;
		//������һ�������е�λ�ã�������Ϊ0��������Ϊ6
        int wday;
        int utc_offset;
		//ʱ������
        std::string tzname;
    };

	inline double secsToDays(double s)
	{
		return s * (1.0 / 86400.0);
	}

	inline double daysToSecs(double d)
	{
		return d * 86400.0;
	}


};
#endif 


