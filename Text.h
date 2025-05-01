#pragma once
#include <MBCLI/Window.h>
#include "SizeSpecification.h"
namespace MBTUI
{
    class Text : public MBCLI::Window
    {
        std::string m_Content;
        SizeSpecification m_SizeSpec;
        MBCLI::Dimensions m_Dims;
        MBCLI::Dimensions m_LastWriteDims;

        MBCLI::TerminalColor m_Color = MBCLI::ANSITerminalColor::BrightWhite;
        MBCLI::TerminalColor m_BGColor = MBCLI::ANSITerminalColor::Black;
        MBCLI::TerminalColor m_HighlightColor = MBCLI::ANSITerminalColor::BrightWhite;
        MBCLI::TerminalColor m_HighlightBGColor = MBCLI::ANSITerminalColor::Black;

        bool m_Focus = false;
        bool m_Multiline = false;
        bool m_Recalculate = false;

        std::vector<std::pair<int_least32_t,int_least32_t>> m_LineOffsets;

        bool p_IsWord(unsigned char const* Begin,unsigned char const* End) const;
        std::vector<std::pair<int_least32_t,int_least32_t>> p_Tokenize(MBCLI::Dimensions Dims,std::string_view Content) const;

    public: 
        std::string_view GetContent() const;
        SizeSpecification GetSizeSpec() const;
        void SetSizeSpec(SizeSpecification NewSpec);

        void SetText(std::string Content);
        void SetColor(MBCLI::TerminalColor Color);
        void SetBGColor(MBCLI::TerminalColor Color);
        void SetHighlightColor(MBCLI::TerminalColor Color);
        void SetHighlightBGColor(MBCLI::TerminalColor Color);
        void SetMultiline(bool Multiline);

        virtual bool HandleInput(MBCLI::ConsoleInput const& Input);
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions);
        virtual void SetFocus(bool IsFocused);
        virtual MBCLI::CursorInfo GetCursorInfo();
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw);
    };
}
