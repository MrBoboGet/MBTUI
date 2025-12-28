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
            if(m_SubWindow != nullptr && m_SubWindow->NeedsCleanup())
            {
                m_SubWindow->RemoveFromTree();
            }
            m_SubWindow = std::move(Child);
            SetChild(*m_SubWindow);
            SetUpdated(true);
        }

        void SetVisible(bool Visible);

        virtual bool HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) override;
        virtual void SetFocus(bool IsFocused) override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual void RemoveFromTree() override
        {
            if(m_SubWindow != nullptr && m_SubWindow->NeedsCleanup())
            {
                m_SubWindow->RemoveFromTree();
            }
        }
    };
}
