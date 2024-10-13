#include "BufferWindow.h"

namespace MBTUI
{
    bool BufferWindow::Updated() 
    {
        bool ReturnValue = false;
        if(m_Updated)
        {
            ReturnValue = true;
            m_Updated = false;
        }
        return ReturnValue;
    }
    void BufferWindow::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //
    }
    void BufferWindow::SetDimensions(MBCLI::Dimensions NewDimensions) 
    {
        m_Updated = true;
        m_CroppedBuffer = MBCLI::TerminalWindowBuffer(NewDimensions.Width,NewDimensions.Height);
        m_CroppedBuffer.WriteBuffer(m_OriginalBuffer,0,0);
    }
    void BufferWindow::SetFocus(bool IsFocused) 
    {
        //
    }
    MBCLI::CursorInfo BufferWindow::GetCursorInfo() 
    {
        MBCLI::CursorInfo Cursor;
        Cursor.Hidden = true;
        return Cursor;
    }
    void BufferWindow::SetBuffer(MBCLI::TerminalWindowBuffer Buffer)
    {
        m_OriginalBuffer = Buffer;
        m_CroppedBuffer = Buffer;
    }
    BufferWindow::BufferWindow(MBCLI::TerminalWindowBuffer Buffer)
    {
        SetBuffer(std::move(Buffer));
    }
    MBCLI::TerminalWindowBuffer BufferWindow::GetBuffer() 
    {
        return m_CroppedBuffer;
    }
}
