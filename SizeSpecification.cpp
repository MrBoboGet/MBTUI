#include "SizeSpecification.h"
#include <MBParsing/MBParsing.h>
#include <charconv>
namespace MBTUI
{
    bool SizeSpecification::Empty() const
    {
        return !HeightSpecified() && !WidthSpecified();
    }
    MBCLI::Dimensions SizeSpecification::GetDims(MBCLI::Dimensions ContainerSize) const
    {
        auto ReturnValue = ContainerSize;
        ReturnValue.Height = p_GetSize(m_HeightSpec,ReturnValue.Height);
        ReturnValue.Width = p_GetSize(m_WidthSpec,ReturnValue.Width);
        return ReturnValue;
    }
    MBCLI::Dimensions SizeSpecification::ModifyDims(MBCLI::Dimensions OriginalDims,MBCLI::Dimensions ContainerSize) const
    {
        MBCLI::Dimensions ReturnValue = OriginalDims;
        auto SpecificationDims = GetDims(ContainerSize);
        if(HeightSpecified())
        {
            ReturnValue.Height = SpecificationDims.Height;
        }
        if(WidthSpecified())
        {
            ReturnValue.Width = SpecificationDims.Width;
        }
        return ReturnValue;
    }
    void SizeSpecification::SetWidth(int CharacterCount)
    {
        m_WidthSpec = CharacterCount;
    }
    void SizeSpecification::SetWidth(Percent PercentSize)
    {
        m_WidthSpec = PercentSize;
    }
    void SizeSpecification::SetWidth(std::string_view String)
    {
        m_WidthSpec = p_ParseSpec(String);
    }

    int SizeSpecification::p_GetSize(Spec const& Spec,int OriginalSize)
    {
        int ReturnValue = OriginalSize;
        if(std::holds_alternative<int>(Spec))
        {
            ReturnValue = std::get<int>(Spec);
            if(ReturnValue < 0)
            {
                ReturnValue = OriginalSize + ReturnValue;
            }
        }
        else if(std::holds_alternative<Percent>(Spec))
        {
            ReturnValue = OriginalSize * (std::min(100,std::max(0,std::get<Percent>(Spec).Value))/(float)100);
        }
        return ReturnValue;
    }
    SizeSpecification::Spec SizeSpecification::p_ParseSpec(std::string_view SpecString)
    {
        int Result = 0;
        size_t OutOffset = 0;
        MBParsing::SkipWhitespace(SpecString.data(),SpecString.size(),0,&OutOffset);
        auto ParseResult = std::from_chars(SpecString.data(),SpecString.data()+(SpecString.size()-OutOffset),Result);
        if(ParseResult.ptr != SpecString.data())
        {
            if(ParseResult.ptr != SpecString.data()+SpecString.size())
            {
                if(*ParseResult.ptr == '%')
                {
                    return Spec(Percent{Result});
                }
            }
            else
            {
                return Spec(Result);
            }
        }
        return Spec();
    }
    void SizeSpecification::SetHeight(int CharacterCount)
    {
        m_HeightSpec = CharacterCount;
    }
    void SizeSpecification::SetHeight(Percent PercentSize)
    {
        m_HeightSpec = PercentSize;
    }
    void SizeSpecification::SetHeight(std::string_view String)
    {
        m_HeightSpec = p_ParseSpec(String);
    }
}
