#include "Absolute.h"


namespace MBTUI
{
    Orientation StringToOrientation(std::string_view View)
    {
        Orientation Result = Orientation::Atop;
        if(View == "center")
        {
            Result = Orientation::Center;
        }
        else if(View == "left")
        {
            Result = Orientation::Left;
        }
        else if(View == "right")
        {
            Result = Orientation::Right;   
        }
        else if(View == "above")
        {
            Result = Orientation::Above;
        }
        else if(View == "below")
        {
            Result = Orientation::Below;
        }
        return Result;
    }

    //
       
    bool Absolute::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        if(m_SubWindow != nullptr)
        {
            return m_SubWindow->HandleInput(Input);
        }
        return false;
    }
    MBCLI::Dimensions Absolute::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        m_ParentContainerSize = SuggestedDimensions;
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
            if(!m_Handle.Empty() && !Visible)
            {
                //View.RemoveOverlay(m_Handle);
                m_Handle.Remove();
                m_Handle = MBCLI::OverlayHandle();
            }
        }
        m_Visible = Visible;
    }
    //void Absolute::SetCenter(bool Centered)
    //{
    //    if(m_Center != Centered)
    //    {
    //        SetUpdated(true);   
    //        m_Redraw = true;
    //    }
    //    m_Center = Centered;

    //}
    void Absolute::SetRelative(bool Relative)
    {
        if(m_Relative != Relative)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }
        m_Relative = Relative;
    }
    void Absolute::SetOffsets(int RowOffset,int ColumnOffset)
    {
        if(std::tie(m_RowOffset,m_ColumnOffset) != std::tie(RowOffset,ColumnOffset))
        {
            SetUpdated(true);
            m_Redraw = true;
        }
        m_RowOffset = RowOffset;
        m_ColumnOffset = ColumnOffset;
    }
    void Absolute::SetOrientation(Orientation NewOrientation)
    {
        if(m_Orientation != NewOrientation)
        {
            SetUpdated(true);
            m_Redraw = true;
        }
        m_Orientation = NewOrientation;
    }
    MBCLI::Dimensions Absolute::p_GetOffsets(MBCLI::BufferView& View)
    {
        MBCLI::Dimensions ReturnValue;
        ReturnValue.Height = 0;
        ReturnValue.Width = 0;

        auto ContainerDims = !m_Relative ?  View.GetScreenDims() : m_ParentContainerSize;
        
        if(m_Orientation == Orientation::Center)
        {
            auto AbsoluteDims =  ContainerDims;
            ReturnValue.Height = (AbsoluteDims.Height - m_SubDims.Height)/2;
            ReturnValue.Width = (AbsoluteDims.Width - m_SubDims.Width)/2;
        }
        else if(m_Orientation == Orientation::Atop)
        {
            
        }
        else if(m_Orientation == Orientation::Left)
        {
            ReturnValue.Width -= m_SubDims.Width;
        }
        else if(m_Orientation == Orientation::Right)
        {
            ReturnValue.Width = ContainerDims.Width;
        }
        else if(m_Orientation == Orientation::Above)
        {
            ReturnValue.Height = ContainerDims.Height;
        }
        else if(m_Orientation == Orientation::Below)
        {
            ReturnValue.Height = -m_SubDims.Height;
        }
        ReturnValue.Height += m_RowOffset;
        ReturnValue.Width += m_ColumnOffset;
        return ReturnValue;
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
                    if(m_Relative)
                    {
                        m_SubDims = m_SubWindow->PreferedDimensions(m_ParentContainerSize);
                    }
                    else
                    {
                        m_SubDims = m_SubWindow->PreferedDimensions(View.GetScreenDims());
                    }
                }
                MBCLI::Dimensions Dims;
                Dims.Width = 0;
                Dims.Height = 0;
                Dims = p_GetOffsets(View);

                if(std::tie(m_PreviousWriteDims,m_PreviousWriteOffsets) != std::tie(m_SubDims,Dims))
                {
                    m_Redraw = true;
                }
                if(m_Relative)
                {
                    if(m_Redraw)
                    {
                        View.RelativeOverlay(m_Handle,Dims.Height,Dims.Width,m_SubDims).Clear();
                    }
                    m_SubWindow->WriteBuffer(View.RelativeOverlay(m_Handle,Dims.Height,Dims.Width,m_SubDims),Redraw||m_Redraw);
                }
                else
                {
                    if(m_Redraw)
                    {
                        View.AbsoluteOverlay(m_Handle,Dims.Height,Dims.Width,m_SubDims).Clear();
                    }
                    m_SubWindow->WriteBuffer(View.AbsoluteOverlay(m_Handle,Dims.Height,Dims.Width,m_SubDims),Redraw||m_Redraw);
                }
                m_PreviousWriteDims = m_SubDims;
                m_PreviousWriteOffsets = Dims;
            }
            m_Redraw = false;
            SetUpdated(false);   
        }
    }
}

