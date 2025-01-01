#include "Absolute.h"


namespace MBTUI
{
       
    void Absolute::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        if(m_SubWindow != nullptr)
        {
            m_SubWindow->HandleInput(Input);
        }
    }
    MBCLI::Dimensions Absolute::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        return MBCLI::Dimensions(0,0);
    }
    void Absolute::SetFocus(bool IsFocused) 
    {
        if(m_SubWindow != nullptr)
        {
            m_SubWindow->SetFocus(IsFocused);
        }
    }
    MBCLI::CursorInfo Absolute::GetCursorInfo()
    {
        if(m_SubWindow != nullptr)
        {
            return m_SubWindow->GetCursorInfo();
        }
        return MBCLI::CursorInfo();
    }
    void Absolute::SetVisible(bool Visible)
    {
        if(m_Visible != Visible)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }
        m_Visible = Visible;
    }
    void Absolute::SetCenter(bool Centered)
    {
        if(m_Center != Centered)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }
        m_Center = Centered;

    }
    void Absolute::WriteBuffer(MBCLI::BufferView View,bool Redraw)
    {
        if(m_SubWindow != nullptr)
        {
            if(m_Handle.Empty() && m_Visible)
            {
                m_Handle = View.RegisterOverlay();
            }
            if(!m_Handle.Empty() && !m_Visible)
            {
                View.RemoveOverlay(m_Handle);
                m_Handle = MBCLI::OverlayHandle();
            }
            if( (Updated() || Redraw) && m_Visible)
            {
                if(m_SubWindow->Updated() || m_SubDims == MBCLI::Dimensions())
                {
                    m_SubDims = m_SubWindow->PreferedDimensions(View.GetDimensions());
                }
                MBCLI::Dimensions Dims;
                Dims.Width = 0;
                Dims.Height = 0;
                if(m_Center)
                {
                    auto AbsoluteDims = View.GetScreenDims();
                    Dims.Height = (AbsoluteDims.Height - m_SubDims.Height)/2;
                    Dims.Width = (AbsoluteDims.Width - m_SubDims.Width)/2;
                }
                m_SubWindow->WriteBuffer(View.RelativeOverlay(m_Handle,Dims.Height,Dims.Width,m_SubDims),Redraw||m_Redraw);
            }
            m_Redraw = false;
            SetUpdated(false);   
        }
    }
}

