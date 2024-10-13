#pragma once

#include <MBCLI/MBCLI.h>
#include <MBCLI/Window.h>
#include <MBUtility/SmartPtr.h>
namespace MBTUI
{
    class Stacker : public MBCLI::Window
    {
        bool m_Updated = true;
        std::vector<MBUtility::SmartPtr<MBCLI::Window>> m_StackedWindows;
        MBCLI::TerminalWindowBuffer m_Buffer;
        MBCLI::Dimensions m_Dims;

        void p_UpdateBuffer();
        bool m_Reversed = false;
    public:

        void AddElement(MBUtility::SmartPtr<MBCLI::Window> NewWindow)
        {
            m_StackedWindows.emplace_back(std::move(NewWindow));
            p_UpdateBuffer();
        }
        void SetReversed(bool Reversed)
        {
            m_Reversed = true;
            p_UpdateBuffer();
        }
        virtual bool Updated() override;
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetDimensions(MBCLI::Dimensions NewDimensions) override;
        virtual void SetFocus(bool IsFocused)  override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual MBCLI::TerminalWindowBuffer GetBuffer() override;
    };
}
