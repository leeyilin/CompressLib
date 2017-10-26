#pragma once
#include <map>
#include <vector>
#include <string>
#include <vector>
using namespace std;

class DigitScale
{
public:
    DigitScale();
    void setDataScale(int d);
    void setDisplayScale(int d);
    int getDataScale() const { return data_; }
    int getDisplayScale() const { return display_; }
    bool isDataScaleSet() const;
    bool isDisplayScaleSet() const;

private:
    unsigned char data_;
    unsigned char display_;
    unsigned char flag_;
};

class CMarketCode
{
private:
    enum {ExtraDirect = 1, ExtraCodeAnalysis, ExtraInitial};
    struct SMarketTypeMap
    {
        typedef map<unsigned short, DigitScale> ScaleMap;
        int m_intMinLen;
        int m_intMaxLen;
        map<string, unsigned short> m_data;
        ScaleMap m_scale;
        map<unsigned short, string> m_extra;
        SMarketTypeMap()
            :m_intMaxLen(0)
            ,m_intMinLen(0)
        {

        }
    };

    map<string, unsigned short> m_mapCodeToID;
    map<unsigned short, string> m_mapIDToCode;
    map<string, vector<string> > m_mapBlockRelativeMarket;

    map<unsigned short, int> m_mapIDToExtraInfo;
    map<unsigned short, SMarketTypeMap > m_mapMarketTypeToID;
    map<unsigned short, unsigned short> m_mapIDToAssociatedID;
    map<unsigned short, unsigned short> m_mapAssociatedIDToID;
    map<unsigned short, unsigned short> m_mapCorrespondingMarketID;
public:
    bool Init(const string & strFileName);
    const string& GetMarketCode(const unsigned short wMarketID) const;
    const map<string, vector<string> >& GetBlockRelatvieMarket() const;
    unsigned short GetMarketID(const string & strCode);
    bool GetTypeID(const unsigned short wMarketID, const string & strCode, const string& strExtra, unsigned short & wTypeID);
    bool GetTypeScale(const unsigned short wMarketID, const unsigned short wTypeID, DigitScale& scale);

    bool GetTypeScale(const unsigned short wMarketID, const unsigned short wTypeID, int& intScale);
    //�����������ֻ���й����г����г����õ�
    unsigned short GetAssociatedID(const unsigned short wMarketID);   //���ش����г��Ĺ����г�,û���򷵻�0
    unsigned short GetSourceID(const unsigned short wMarketID);           //���ع����г���Ӧ��ԭʼ�г���û���򷵻�0
    unsigned short GetCorrespondingMarketID(const unsigned short wMarketID);
private:

};
