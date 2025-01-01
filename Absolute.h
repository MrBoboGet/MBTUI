#pragma once
#include <MBCLI/Window.h>




namespace MBTUI
{
    class Absolute : public MBCLI::Window
    {
        MBCLI::OverlayHandle m_Handle;

        MBUtility::SmartPtr<MBCLI::Window> m_SubWindow;
        MBCLI::Dimensions m_SubDims;

        bool m_Visible = true;
        bool m_Center = false;
        bool m_Redraw = false;
    public:
        void SetSubwindow(MBUtility::SmartPtr<MBCLI::Window> Child)
        {
            m_SubWindow = std::move(Child);
            SetChild(*m_SubWindow);
            SetUpdated(true);
        }

        void SetVisible(bool Visible);
        void SetCenter(bool Centered);

        virtual void HandleInput(MBCLI::ConsoleInput const& Input);
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions);
        virtual void SetFocus(bool IsFocused);
        virtual MBCLI::CursorInfo GetCursorInfo();
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw);
    };
}
