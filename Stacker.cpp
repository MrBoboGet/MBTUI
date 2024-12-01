#include "Stacker.h"

namespace MBTUI
{
    bool Stacker::Updated() 
    {
        if(m_Updated)
        {
            return true;   
        }
        for(auto& SubWindow : m_StackedWindows)
        {
            if(SubWindow.Window->Updated())
            {
                return true;   
            }
        }
        return false;
    }
    void Stacker::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //navigation
        //
        if(m_SelectedIndex != -1)
        {
            try
            {
                m_StackedWindows[m_SelectedIndex].Window->SetFocus(false);
            }
            catch(...)
            {
                   
            }
        }
        if(Input.CharacterInput == "j")
        {

        }
        else if(Input.CharacterInput == "k")
        {
               
        }
        else if(Input.CharacterInput == "l")
        {
               
        }
        else if(Input.CharacterInput == "h")
        {
               
        }
    }
    void Stacker::p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw)
    {
        int FlowOffset = 0;
        int OtherFlowOffset = 0;


        int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;
        int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;

        size_t CurrentFlowIndex = 0;

        int NextOtherFlowIncrement = 0;
        for(auto& Window : *this)
        {
            if(FlowOffset >= m_Dims.*MainFlowMember)
            {
                if(!m_Overflow)
                {
                    break;   
                }
            }
            if(OtherFlowOffset >= m_Dims.*OtherFlowDirection)
            {
                break;
            }
            if(CurrentFlowIndex != Window.FlowIndex && m_Overflow)
            {
                FlowOffset = 0;
                OtherFlowOffset += NextOtherFlowIncrement;
                NextOtherFlowIncrement = 0;
            }
            if(m_Overflow)
            {
                NextOtherFlowIncrement = (m_FlowWidth > 0) ? m_FlowWidth : std::max(NextOtherFlowIncrement,Window.Dims.*OtherFlowDirection);
            }
            MBCLI::Dimensions OffsetDims;
            OffsetDims.*MainFlowMember = FlowOffset;
            OffsetDims.*OtherFlowDirection = OtherFlowOffset;
            Window.Window->WriteBuffer(View.SubView(OffsetDims.Height,OffsetDims.Width,Window.Dims),Redraw);
            FlowOffset += Window.Dims.*MainFlowMember;
        }
    }
    void Stacker::SetFlowDirection(bool IsVertical)
    {
        if(m_VerticalFlow != IsVertical)
        {
            m_Updated = true;   
        }
        m_VerticalFlow = IsVertical;
    }
    void Stacker::SetFlowWidth(int Size)
    {
        if(m_FlowWidth != Size)
        {
            m_Updated = true;   
        }
        m_FlowWidth = Size;
    }
    void Stacker::EnableOverflow(bool OverlowEnabled)
    {
        if(m_Overflow != OverlowEnabled)
        {
            m_Updated = true;   
        }
        m_Overflow = OverlowEnabled;
    }
    void Stacker::p_AssignDimensions()
    {
        MBCLI::Dimensions SuggestedDims = MBCLI::Dimensions(m_Dims.Width,m_Dims.Height);
        int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;;
        int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;
        int CurrentOverflow = 0;
        size_t CurrentFlowIndex = 0;
        for(auto& SubWindow : *this)
        {
            SubWindow.Dims = SubWindow.Window->PreferedDimensions(SuggestedDims);
            SubWindow.Dims.*MainFlowMember = SubWindow.Dims.*MainFlowMember < 0 ? 1 : SubWindow.Dims.*MainFlowMember;
            SubWindow.FlowPosition = CurrentOverflow;
            CurrentOverflow += SubWindow.Dims.*MainFlowMember;
            if(CurrentOverflow > m_Dims.*MainFlowMember)
            {
                CurrentFlowIndex += 1;
                CurrentOverflow = 0;
            }
            SubWindow.FlowIndex = CurrentFlowIndex;
            if(m_Overflow == true)
            {
                SubWindow.Dims.*OtherFlowDirection = (m_FlowWidth > 0) ? m_FlowWidth : SubWindow.Dims.*OtherFlowDirection;
            }
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
    void Stacker::WriteBuffer(MBCLI::BufferView View,bool Redraw) 
    {
        if(View.GetDimensions() != m_Dims)
        {
            m_Dims = View.GetDimensions();
            m_Updated = true;
        }
        if(m_Updated)
        {
            View.Clear();   
            Redraw = true;
            p_AssignDimensions();
        }
        m_Updated = false;
        p_UpdateBuffer(View,Redraw);
    }
    MBCLI::Dimensions Stacker::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        MBCLI::Dimensions ReturnValue = SuggestedDimensions;
        int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;;
        int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;
        ReturnValue.*OtherFlowDirection = 0;
        for(auto& Child : *this)
        {
            auto ChildPreferedDims = Child.Window->PreferedDimensions(SuggestedDimensions);
            ReturnValue.*MainFlowMember += ChildPreferedDims.*MainFlowMember;
            ReturnValue.*OtherFlowDirection = std::max(ReturnValue.*OtherFlowDirection,ChildPreferedDims.*OtherFlowDirection);
        }
        if(ReturnValue.Height >= SuggestedDimensions.Height)
        {
            ReturnValue.Height = SuggestedDimensions.Height;
        }
        if(ReturnValue.Width >= SuggestedDimensions.Width)
        {
            ReturnValue.Width = SuggestedDimensions.Width;
        }
        return ReturnValue;
    }
}
