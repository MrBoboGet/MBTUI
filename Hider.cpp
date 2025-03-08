#include "Hider.h"
namespace MBTUI
{
    void Hider::SetVisible(bool Visible)
    {
        if(m_Visible != Visible)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }
        m_Visible = Visible;
    }
    bool Hider::HandleInput(MBCLI::ConsoleInput const& Input)
    {
        if(m_Visible)
        {
            return m_SubWindow->HandleInput(Input);
        }
        return false;
    }
    MBCLI::Dimensions Hider::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        if(m_Visible)
        {
            return m_SubWindow->PreferedDimensions(SuggestedDimensions);
        }
        MBCLI::Dimensions Dims;
        Dims.Height = 0;
        Dims.Width = 0;
        return Dims;
    }
    void Hider::SetFocus(bool IsFocused)
    {
        if(m_Visible)
        {
            m_SubWindow->SetFocus(IsFocused);
        }
    }
    MBCLI::CursorInfo Hider::GetCursorInfo()
    {
        if(m_Visible)
        {
            return m_SubWindow->GetCursorInfo();
        }
        MBCLI::CursorInfo ReturnValue;
        return ReturnValue;
    }
    void Hider::WriteBuffer(MBCLI::BufferView View,bool Redraw)
    {
        if(m_Visible)
        {
            m_SubWindow->WriteBuffer(std::move(View),Redraw || m_Redraw);
            m_Redraw = false;
        }
    }
}
