#include "marketcode.h"
#include "config.h"
#include "comfun.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
using namespace BackCom;

static const string EMPTY_STRING = "";

enum ConfigScaleFlag
{
    DATA_SCALE_SET = 0x1,
    DISPLAY_SCALE_SET = 0x2,
};

DigitScale::DigitScale()
    : data_(0),
      display_(0),
      flag_(0)
{
}

void DigitScale::setDataScale(int d)
{
    data_ = d;
    flag_ |= DATA_SCALE_SET;
}

void DigitScale::setDisplayScale(int d)
{
    display_ = d;
    flag_ |= DISPLAY_SCALE_SET;
}

bool DigitScale::isDataScaleSet() const
{
    return (flag_ & DATA_SCALE_SET) != 0;
}

bool DigitScale::isDisplayScaleSet() const
{
    return (flag_ & DISPLAY_SCALE_SET) != 0;
}

bool CMarketCode::Init(const string & strFileName)
{
    using boost::property_tree::ptree;
    try
    {
        ptree pt;
        read_xml(strFileName, pt);
        {
            ptree marketlist = pt.get_child("marketlist");
            for(ptree::iterator itr = marketlist.begin(); itr!=marketlist.end(); itr++)
            {
                ptree market = itr->second;
                string strCode = market.get<string>("<xmlattr>.code");
                int intID = market.get<int>("<xmlattr>.id");
                m_mapIDToCode[intID] = strCode;
                m_mapCodeToID[strCode] = intID;
                m_mapIDToExtraInfo[intID] = market.get<int>("<xmlattr>.extrainfo", ExtraInitial);
                int usAssociatedID = market.get<int>("<xmlattr>.associatedid", 0);
                //Log(CLog::Error, this, __FUNCTION__, "intID=%d,usAssociatedID=%d, m_mapIDToExtraInfo[intID]=%d", intID, usAssociatedID,m_mapIDToExtraInfo[intID]);
                if(usAssociatedID > 0)
                {
                    m_mapIDToAssociatedID[intID] = usAssociatedID;
                    m_mapAssociatedIDToID[usAssociatedID] = intID;
                }

                string strBlockCode = market.get<string>("<xmlattr>.relativemarket", "");
                if (!strBlockCode.empty()) {
                    m_mapBlockRelativeMarket[strCode] = multicommon::split_string(strBlockCode, ',');
                }
            }

            for(map<unsigned short, unsigned short>::iterator itA = m_mapIDToAssociatedID.begin(); itA != m_mapIDToAssociatedID.end(); itA++)
            {
                m_mapCorrespondingMarketID[itA->first] = itA->second;
                m_mapCorrespondingMarketID[itA->second] = itA->first;
            }
            //Log(CLog::Error, this, __FUNCTION__, "m_mapIDToAssociatedID.size()=%d,m_mapIDToAssociatedID.size()=%d", m_mapIDToAssociatedID.size(), m_mapIDToAssociatedID.size());
        }

        {
            ptree typelist = pt.get_child("typelist");
            for(ptree::iterator itr = typelist.begin(); itr!=typelist.end(); itr++)
            {
                ptree typeitem = itr->second;
                string strMarketCode = typeitem.get<string>("<xmlattr>.market");
                if(m_mapCodeToID.find(strMarketCode) == m_mapCodeToID.end())
                {
                    continue;
                }

                unsigned short wMarketID = m_mapCodeToID[strMarketCode];
                string strTypeCode = typeitem.get<string>("<xmlattr>.code");
                int intTypeID = typeitem.get<int>("<xmlattr>.id");
                SMarketTypeMap & typemap = m_mapMarketTypeToID[wMarketID];
                if(typemap.m_intMaxLen == 0 || strTypeCode.size() > typemap.m_intMaxLen)
                {
                    typemap.m_intMaxLen = strTypeCode.size();
                }

                if(typemap.m_intMinLen == 0 || strTypeCode.size() < typemap.m_intMinLen)
                {
                    typemap.m_intMinLen = strTypeCode.size();
                }

                DigitScale& ds = typemap.m_scale[intTypeID];
                ds.setDataScale(typeitem.get<int>("<xmlattr>.scale", 3));
                boost::optional<int> displayScale = typeitem.get_optional<int>("<xmlattr>.display");
                if (displayScale) {
                    ds.setDisplayScale(displayScale.get());
                }
                string strExtra = typeitem.get<string>("<xmlattr>.extra", "");
                typemap.m_data[strTypeCode] = intTypeID;
                if (strExtra != "")
                {
                    typemap.m_extra[intTypeID] = strExtra;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        return false;
    }

    return true;
}

const string& CMarketCode::GetMarketCode(const unsigned short wMarketID) const
{
    map<unsigned short, string>::const_iterator it = m_mapIDToCode.find(wMarketID);
    if(it != m_mapIDToCode.end())
    {
        return it->second;
    }

    return EMPTY_STRING;
}

const map<string, vector<string> >& CMarketCode::GetBlockRelatvieMarket() const {
    return m_mapBlockRelativeMarket;
}

unsigned short CMarketCode::GetMarketID(const string & strCode)
{
    map<string, unsigned short>::iterator it = m_mapCodeToID.find(strCode);
    unsigned short wMarketID = 0;
    if(it != m_mapCodeToID.end())
    {
        wMarketID = it->second;
    }

    return wMarketID;
}

bool CMarketCode::GetTypeID(const unsigned short wMarketID, const string & strCode, const string& strExtra, unsigned short & wTypeID)
{
    char pCode[32];
    memset(pCode, 0, sizeof(pCode));
    int intCodeLen = strCode.size();
    strncpy(pCode, strCode.c_str(), intCodeLen);
    upperStr(pCode);
    string strTmpCode = pCode;
    map<unsigned short, SMarketTypeMap >::iterator it = m_mapMarketTypeToID.find(wMarketID);
    if(it == m_mapMarketTypeToID.end())
    {
        return false;
    }
    map<unsigned short, int>::iterator it2 = m_mapIDToExtraInfo.find(wMarketID);
    if (it2 == m_mapIDToExtraInfo.end())
    {
        return false;
    }

    SMarketTypeMap & typemap = it->second;
    bool blnFind = false;

    if (it2->second == ExtraCodeAnalysis)
    {
        map<string, unsigned short>::iterator itType = typemap.m_data.begin();
        for (; itType != typemap.m_data.end(); ++itType)
        {
            if (itType->first.substr(0, 4) == strExtra)
            {
                map<unsigned short, string>::iterator iter = typemap.m_extra.find(itType->second);
                if (iter != typemap.m_extra.end())
                {
                    try
                    {
                        string strMinCode = iter->second.substr(1, 5);
                        string strMaxCode = iter->second.substr(7, 5);
                        if (strMinCode <= strTmpCode && strTmpCode <= strMaxCode)
                        {
                            if (iter->second.at(0) == '.')
                            {
                                wTypeID = itType->second;
                                blnFind = true;
                                break;
                            }
                        }
                        else
                        {
                            if (iter->second.at(0) == '!')
                            {
                                wTypeID = itType->second;
                                blnFind = true;
                                break;
                            }
                        }
                    }
                    catch (std::out_of_range& ex)
                    {
                        continue;
                    }
                }
                else
                {
                    wTypeID = itType->second;
                    blnFind = true;
                    break;
                }
            }
        }
    }
    else if (it2->second == ExtraDirect)
    {
        map<string, unsigned short>::iterator itType = typemap.m_data.begin();
        for (; itType != typemap.m_data.end(); ++itType)
        {
            if (strcmp(itType->first.c_str(), strExtra.c_str()) == 0)
            {
                wTypeID = itType->second;
                blnFind = true;
                break;
            }
        }
    }
    else
    {
        for(int i = typemap.m_intMaxLen; i >= typemap.m_intMinLen; i--)
        {
            if(strTmpCode.size() >= i)
            {
                string strTypeCode = strTmpCode.substr(0, i);
                map<string, unsigned short>::iterator itType = typemap.m_data.find(strTypeCode);
                if(itType != typemap.m_data.end())
                {
                    wTypeID = itType->second;
                    blnFind = true;
                    break;
                }
            }
        }
    }

    return blnFind;
}

bool CMarketCode::GetTypeScale(const unsigned short wMarketID, const unsigned short wTypeID, DigitScale& scale)
{
    map<unsigned short, SMarketTypeMap >::iterator it = m_mapMarketTypeToID.find(wMarketID);
    if (it == m_mapMarketTypeToID.end()) {
        return false;
    }

    SMarketTypeMap & typemap = it->second;
    bool blnFind = false;
    SMarketTypeMap::ScaleMap::iterator itScale = typemap.m_scale.find(wTypeID);
    if (itScale != typemap.m_scale.end()) {
        scale = itScale->second;
        blnFind = true;
    }

    return blnFind;

}

bool CMarketCode::GetTypeScale(const unsigned short wMarketID, const unsigned short wTypeID, int& intScale)
{
    DigitScale ds;
    bool found = GetTypeScale(wMarketID, wTypeID, ds);
    if (found) {
        if (ds.isDataScaleSet()) {
            intScale = ds.getDataScale();
        } else {
            found = false;
        }
    }

    return found;
}

unsigned short CMarketCode::GetAssociatedID(const unsigned short wMarketID)
{
    map<unsigned short, unsigned short>::iterator it = m_mapIDToAssociatedID.find(wMarketID);
    unsigned short wRetMarketID = 0;
    if(it != m_mapIDToAssociatedID.end())
    {
        wRetMarketID = it->second;
    }

    return wRetMarketID;
}

unsigned short CMarketCode::GetSourceID(const unsigned short wMarketID)
{
    map<unsigned short, unsigned short>::iterator it = m_mapAssociatedIDToID.find(wMarketID);
    unsigned short wRetMarketID = 0;
    if(it != m_mapAssociatedIDToID.end())
    {
        wRetMarketID = it->second;
    }

    return wRetMarketID;
}

unsigned short CMarketCode::GetCorrespondingMarketID(const unsigned short wMarketID)
{
    map<unsigned short, unsigned short>::iterator it = m_mapCorrespondingMarketID.find(wMarketID);
    unsigned short wRetMarketID = 0;
    if(it != m_mapCorrespondingMarketID.end())
    {
        wRetMarketID = it->second;
    }

    return wRetMarketID;
}

