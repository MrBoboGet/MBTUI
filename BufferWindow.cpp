#include "BufferWindow.h"

namespace MBTUI
{
    void BufferWindow::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //
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
        SetUpdated(true);
    }
    BufferWindow::BufferWindow(MBCLI::TerminalWindowBuffer Buffer)
    {
        SetBuffer(std::move(Buffer));
    }
    void BufferWindow::WriteBuffer(MBCLI::BufferView View,bool Redraw)
    {
        SetUpdated(false);
        View.WriteBuffer(0,0,m_OriginalBuffer);
    }
    MBCLI::Dimensions BufferWindow::PreferedDimensions(MBCLI::Dimensions Dims) 
    {
        return MBCLI::Dimensions(m_OriginalBuffer.GetWidth(),m_OriginalBuffer.GetHeight());
    }
}
