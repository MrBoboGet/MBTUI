#pragma once
#include <MBCLI/Window.h>
namespace MBTUI
{
       
    class Hider : public MBCLI::Window
    {
        MBUtility::SmartPtr<MBCLI::Window> m_SubWindow;
        bool m_Visible = true;
        bool m_Redraw = true;

    public:
        void SetSubwindow(MBUtility::SmartPtr<MBCLI::Window> Child)
        {
            m_SubWindow = std::move(Child);
            SetChild(*m_SubWindow);
            SetUpdated(true);
        }

        void SetVisible(bool Visible);

        virtual bool HandleInput(MBCLI::ConsoleInput const& Input);
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions);
        virtual void SetFocus(bool IsFocused);
        virtual MBCLI::CursorInfo GetCursorInfo();
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw);
    };
}
