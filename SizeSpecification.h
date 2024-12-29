#pragma once
#include <MBCLI/MBCLI.h>


namespace MBTUI
{
    struct Percent
    {
        int Value = 0;
    };
    class SizeSpecification
    {
        typedef std::variant<std::monostate,int,Percent> Spec;
        Spec m_WidthSpec;
        Spec m_HeightSpec;

        static int p_GetSize(Spec const& Spec,int OriginalSize);
        Spec p_ParseSpec(std::string_view SpecString);
    public:

        bool Empty() const;
        MBCLI::Dimensions GetDims(MBCLI::Dimensions ContainerSize) const;

        void SetWidth(int CharacterCount);
        void SetWidth(Percent PercentSize);
        void SetWidth(std::string_view String);

        void SetHeight(int CharacterCount);
        void SetHeight(Percent PercentSize);
        void SetHeight(std::string_view String);

        bool HeightSpecified() const
        {
            return !std::holds_alternative<std::monostate>(m_HeightSpec);
        }
        bool WidthSpecified() const
        {
            return !std::holds_alternative<std::monostate>(m_WidthSpec);
        }
    };
}
