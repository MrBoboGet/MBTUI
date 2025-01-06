#pragma once
#include <MBCLI/Window.h>
#include "SizeSpecification.h"
namespace MBTUI
{
    class Text : public MBCLI::Window
    {
        std::string m_Content;
        SizeSpecification m_SizeSpec;

        MBCLI::TerminalColor m_Color = MBCLI::ANSITerminalColor::BrightWhite;
        MBCLI::TerminalColor m_HighlightColor = MBCLI::ANSITerminalColor::BrightWhite;

        bool m_Focus = false;
    public: 
        std::string_view GetContent() const;
        SizeSpecification GetSizeSpec() const;
        void SetSizeSpec(SizeSpecification NewSpec);

        void SetText(std::string Content);
        void SetColor(MBCLI::TerminalColor Color);
        void SetHighlightColor(MBCLI::TerminalColor Color);

        virtual void HandleInput(MBCLI::ConsoleInput const& Input);
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions);
        virtual void SetFocus(bool IsFocused);
        virtual MBCLI::CursorInfo GetCursorInfo();
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw);
    };
}
