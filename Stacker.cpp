#include "Stacker.h"

namespace MBTUI
{
    bool Stacker::Updated() 
    {
        bool ReturnValue = m_Updated;
        m_Updated = false;
        return m_Updated;
    }
    void Stacker::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //nothing
    }
    void Stacker::p_UpdateBuffer()
    {
        m_Updated = true;
        int RowOffset = 0;
        if(m_Reversed)
        {
            for(auto& SubWindow : m_StackedWindows)
            {
                if(RowOffset > m_Dims.Height)
                {
                    break;
                }
                auto NewBuffer = SubWindow->GetBuffer();
                m_Buffer.WriteBuffer(NewBuffer,RowOffset,0);
                RowOffset += NewBuffer.GetHeight();
            }
        }
        else
        {
            if(m_StackedWindows.size() == 0)
            {
                return;   
            }
            for(int i = m_StackedWindows.size()-1;i >= 0;i--)
            {
                auto& SubWindow = m_StackedWindows[i];
                if(RowOffset > m_Dims.Height)
                {
                    break;
                }
                auto NewBuffer = SubWindow->GetBuffer();
                m_Buffer.WriteBuffer(NewBuffer,RowOffset,0);
                RowOffset += NewBuffer.GetHeight();
            }
        }
    }
    void Stacker::SetDimensions(MBCLI::Dimensions NewDimensions) 
    {
        if(NewDimensions != m_Dims)
        {
            m_Buffer = MBCLI::TerminalWindowBuffer(NewDimensions.Width,NewDimensions.Height);
            m_Updated = true;
            m_Dims = NewDimensions;
            p_UpdateBuffer();
        }
    }
    void Stacker::SetFocus(bool IsFocused)
    {
        //
    }
    MBCLI::CursorInfo Stacker::GetCursorInfo() 
    {
        return MBCLI::CursorInfo();
    }
    MBCLI::TerminalWindowBuffer Stacker::GetBuffer() 
    {
        return m_Buffer;
    }
}
