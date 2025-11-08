#pragma once
#include <MBCLI/Window.h>
#include "SizeSpecification.h"



namespace MBTUI
{
    enum class Orientation
    {
        Atop,
        Left,
        Right,
        Above,
        Center,
        Below
    };

    Orientation StringToOrientation(std::string_view View);

    class Absolute : public MBCLI::Window
    {
        MBCLI::OverlayHandle m_Handle;

        MBUtility::SmartPtr<MBCLI::Window> m_SubWindow;
        
        MBCLI::Dimensions m_SubDims;
        SizeSpecification m_SizeSpec;
        //sketchy 
        MBCLI::Dimensions m_ParentContainerSize;

        MBCLI::Dimensions m_PreviousWriteOffsets;
        MBCLI::Dimensions m_PreviousWriteDims;

        bool m_Visible = true;
        bool m_Redraw = false;
        bool m_Relative = false;

        Orientation m_Orientation = Orientation::Atop;
        int  m_RowOffset = 0;
        int  m_ColumnOffset = 0;

        MBCLI::Dimensions p_GetOffsets(MBCLI::BufferView& View);
    public:
        void SetSubwindow(MBUtility::SmartPtr<MBCLI::Window> Child)
        {
            m_SubWindow = std::move(Child);
            SetChild(*m_SubWindow);
            SetUpdated(true);
        }

        void SetVisible(bool Visible);
        //void SetCenter(bool Centered);
        void SetRelative(bool Relative);
        void SetOffsets(int RowOffset,int ColumnOffset);

        MBCLI::Dimensions GetOffsets() const
        {
            return MBCLI::Dimensions(m_ColumnOffset,m_RowOffset);
        }
        void SetSizeSpec(SizeSpecification NewSpec)
        {
            m_SizeSpec = NewSpec;   
            SetUpdated(true);
        }
        SizeSpecification GetSizeSpec()
        {
            return m_SizeSpec;   
        }

        void SetOrientation(Orientation NewOrientation);

        virtual bool HandleInput(MBCLI::ConsoleInput const& Input);
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions);
        virtual void SetFocus(bool IsFocused);
        virtual MBCLI::CursorInfo GetCursorInfo();
        //virtual TerminalWindowBuffer GetBuffer() { return TerminalWindowBuffer();};
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw);
    };
}
