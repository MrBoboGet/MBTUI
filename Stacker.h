#pragma once

#include <MBCLI/MBCLI.h>
#include <MBCLI/Window.h>
#include <MBUtility/SmartPtr.h>
namespace MBTUI
{
    class Stacker : public MBCLI::Window
    {
        bool m_Updated = true;
        struct SubWindow
        {
            MBUtility::SmartPtr<MBCLI::Window> Window;
            MBCLI::Dimensions Dims;
        };
        std::vector<SubWindow> m_StackedWindows;
        MBCLI::TerminalWindowBuffer m_Buffer;
        MBCLI::Dimensions m_Dims;

        void p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw);
        void p_AssignDimensions();
        bool m_Reversed = false;
    public:

        void AddElement(MBUtility::SmartPtr<MBCLI::Window> NewWindow)
        {
            auto& NewSubwindow = m_StackedWindows.emplace_back();
            NewSubwindow.Window = std::move(NewWindow);
            m_Updated = true;
            p_AssignDimensions();
        }
        void SetReversed(bool Reversed)
        {
            m_Reversed = true;
            m_Updated = true;
            p_AssignDimensions();
        }
        virtual bool Updated() override;
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused)  override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
    };
}
