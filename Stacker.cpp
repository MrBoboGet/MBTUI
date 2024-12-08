#include "Stacker.h"

namespace MBTUI
{
    bool Stacker::Updated() 
    {
        if(m_Updated)
        {
            return true;   
        }
        m_Updated = p_AssignDimensions();
        return m_Updated;
    }
    void Stacker::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //navigation
        //
        if(m_SelectedIndex != -1)
        {
            try
            {
                if(m_SelectedIndex < m_StackedWindows.size())
                {
                    m_StackedWindows[m_SelectedIndex].Window->SetFocus(false);
                }
            }
            catch(...)
            {
                   
            }
        }
        if(m_SelectedIndex == -1)
        {
            if(m_StackedWindows.size() > 0)
            {
                m_SelectedIndex = 0;
                m_StackedWindows[m_SelectedIndex].Window->SetFocus(true);
            }
            return;
        }

        int AxisIncrease = 0;
        int FlowIncrease = 0;

        if(Input.CharacterInput == "j")
        {
            if(m_VerticalFlow)
            {
                AxisIncrease = m_Reversed ? 1 : -1;
            }
            else 
            {
                FlowIncrease = m_OverflowReversed ? 1 : -1;
            }
        }
        else if(Input.CharacterInput == "k")
        {
            if(m_VerticalFlow)
            {
                AxisIncrease = m_Reversed ? -1 : 1;
            }
            else 
            {
                FlowIncrease = m_OverflowReversed ? -1 : 1;
            }
        }
        else if(Input.CharacterInput == "l")
        {
            if(m_VerticalFlow)
            {
                FlowIncrease = m_OverflowReversed ? -1 : 1;
            }
            else 
            {
                AxisIncrease = m_Reversed ? -1 : 1;
            }
        }
        else if(Input.CharacterInput == "h")
        {
            if(m_VerticalFlow)
            {
                FlowIncrease = m_OverflowReversed ? 1 : -1;
            }
            else 
            {
                AxisIncrease = m_Reversed ? 1 : -1;
            }
        }

        if(AxisIncrease != 0)
        {
            m_SelectedIndex += AxisIncrease;
            m_SelectedIndex = std::max(m_SelectedIndex,0);
            m_SelectedIndex = std::min(m_StackedWindows.size(),(size_t)m_SelectedIndex);
        }
        else if(FlowIncrease != 0)
        {
            if(m_SelectedIndex < m_StackedWindows.size())
            {
                auto& SelectedWindow = m_StackedWindows[m_SelectedIndex];
                auto TargetFlow = SelectedWindow.FlowIndex + FlowIncrease;
                auto SelectedFlowPosition = SelectedWindow.FlowPosition + (m_VerticalFlow ? SelectedWindow.Dims.Height : SelectedWindow.Dims.Width)/2;
                if(FlowIncrease > 0)
                {
                    int_least32_t NewIndex = m_StackedWindows.size()-1;
                    for(size_t i = m_SelectedIndex;i < m_StackedWindows.size();i++)
                    {
                        auto const& CurrentWindow = m_StackedWindows[i];
                        auto WindowFlowEnd = CurrentWindow.FlowPosition + (m_VerticalFlow ? CurrentWindow.Dims.Height : CurrentWindow.Dims.Width);
                        if(SelectedFlowPosition <= WindowFlowEnd)
                        {
                            NewIndex = i;
                            break;
                        }
                    }
                    m_SelectedIndex = NewIndex;
                }
                else if(FlowIncrease < 0)
                {
                    int_least32_t NewIndex = 0;
                    for(auto i = m_SelectedIndex;i > 0; i--)
                    {
                        auto const& CurrentWindow = m_StackedWindows[i];
                        auto WindowFlowEnd = CurrentWindow.FlowPosition + (m_VerticalFlow ? CurrentWindow.Dims.Height : CurrentWindow.Dims.Width);
                        if(SelectedFlowPosition <= WindowFlowEnd)
                        {
                            NewIndex = i;
                            break;
                        }
                    }
                    m_SelectedIndex = NewIndex;
                }
            }
        }
        if(m_SelectedIndex < m_StackedWindows.size())
        {
            m_StackedWindows[m_SelectedIndex].Window->SetFocus(true);
        }
    }
    void Stacker::p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw)
    {
        int FlowOffset = 0;
        int OtherFlowOffset = 0;


        int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;
        int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;

        if(m_OverflowReversed)
        {
            OtherFlowOffset = m_PreferedDims.*OtherFlowDirection - m_FlowSizes[0];
            if(m_Border)
            {
                OtherFlowOffset -= 2;
            }
            if(OtherFlowOffset < 0)
            {
                return;   
            }
        }
        size_t CurrentFlowIndex = 0;

        int NextOtherFlowIncrement = 0;


        MBCLI::Dimensions CurrentDims = m_Dims;
        if(m_Border)
        {
            auto PreviousColor = View.GetWriteColor();
            View.SetWriteColor(m_BorderColor);
            MBCLI::DrawBorder(View,0,0,m_PreferedDims.Width,m_PreferedDims.Height);
            View.SetWriteColor(PreviousColor);
            View.ModifyView(1,1,MBCLI::Dimensions(m_Dims.Width-2,m_Dims.Height-2));
            CurrentDims.Width -= 2;
            CurrentDims.Height -= 2;
        }
        if(CurrentDims.Width <= 0 || CurrentDims.Height <= 0)
        {
            return;
        }

        for(auto& Window : *this)
        {
            if(FlowOffset >= CurrentDims.*MainFlowMember)
            {
                if(!m_Overflow)
                {
                    break;   
                }
            }
            if(OtherFlowOffset >= CurrentDims.*OtherFlowDirection)
            {
                break;
            }
            if(CurrentFlowIndex != Window.FlowIndex && m_Overflow)
            {
                FlowOffset = 0;
                if(!m_OverflowReversed)
                {
                    OtherFlowOffset += NextOtherFlowIncrement;
                }
                else
                {
                    OtherFlowOffset -= m_FlowSizes[Window.FlowIndex];
                }
                NextOtherFlowIncrement = 0;
                CurrentFlowIndex = Window.FlowIndex;
            }
            if(OtherFlowOffset + m_FlowSizes[CurrentFlowIndex] <= 0 || OtherFlowOffset >= CurrentDims.*OtherFlowDirection)
            {
                break;
            }
            if(m_Overflow)
            {
                NextOtherFlowIncrement = (m_FlowWidth > 0) ? m_FlowWidth : std::max(NextOtherFlowIncrement,Window.Dims.*OtherFlowDirection);
            }
            MBCLI::Dimensions OffsetDims;
            OffsetDims.*MainFlowMember = FlowOffset;
            OffsetDims.*OtherFlowDirection = OtherFlowOffset;
            MBCLI::Dimensions WindowDims  = Window.Dims;
            WindowDims.*MainFlowMember = std::min(WindowDims.*MainFlowMember,CurrentDims.*MainFlowMember - FlowOffset);
            WindowDims.*OtherFlowDirection = std::min(WindowDims.*OtherFlowDirection,CurrentDims.*OtherFlowDirection - OtherFlowOffset);
            if(Redraw || Window.Redraw)
            {
                Window.Window->WriteBuffer(View.SubView(OffsetDims.Height,OffsetDims.Width,Window.Dims),Redraw);
            }
            Window.Redraw = false;
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
    void Stacker::SetBorder(bool HasBorder)
    {
        if(m_Border != HasBorder)
        {
            m_Updated = true;
        }
        m_Border = HasBorder;
    }
    void Stacker::SetBorderColor(MBCLI::TerminalColor Color)
    {
        if(m_BorderColor != Color)
        {
            m_Updated = true;   
        }
        m_BorderColor = Color;
    }

    void Stacker::SetOverflowDirection(bool Reversed)
    {
        if(m_OverflowReversed != Reversed)
        {
            m_Updated = true;
        }
        m_OverflowReversed = Reversed;
    }
    bool Stacker::p_AssignDimensions()
    {
        bool ReturnValue = false;
        MBCLI::Dimensions SuggestedDims = MBCLI::Dimensions(m_Dims.Width,m_Dims.Height);
        int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;;
        int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;
        int CurrentOverflow = 0;
        size_t CurrentFlowIndex = 0;
        int MainFlowSize = 0;
        int TotalOtherFlowSize = 0;
        int CurrentOtherFlowSize = 0;
        m_FlowSizes.clear();


        MBCLI::Dimensions CurrentDims = m_Dims;
        if(m_Border)
        {
            CurrentDims.Width -= 2;
            CurrentDims.Height -= 2;
        }
        if(CurrentDims.Width <= 0 || CurrentDims.Height <= 0)
        {
            return false;
        }

        for(auto& SubWindow : *this)
        {
            SubWindow.Dims = SubWindow.Window->PreferedDimensions(SuggestedDims);
            SubWindow.Dims.*MainFlowMember = SubWindow.Dims.*MainFlowMember < 0 ? 1 : SubWindow.Dims.*MainFlowMember;
            CurrentOverflow += SubWindow.Dims.*MainFlowMember;
            if(m_Overflow)
            {
                SubWindow.Dims.*OtherFlowDirection = (m_FlowWidth > 0) ? m_FlowWidth : SubWindow.Dims.*OtherFlowDirection;
            }
            CurrentOtherFlowSize = std::max(CurrentOtherFlowSize,SubWindow.Dims.*OtherFlowDirection);
            if(MainFlowSize < CurrentDims.*MainFlowMember)
            {
                MainFlowSize = CurrentOverflow;
            }
            if(CurrentOverflow >= CurrentDims.*MainFlowMember)
            {
                CurrentFlowIndex += 1;
                CurrentOverflow = SubWindow.Dims.*MainFlowMember;
                TotalOtherFlowSize += CurrentOtherFlowSize;
                m_FlowSizes.push_back(CurrentOtherFlowSize);
                CurrentOtherFlowSize = 0;
                MainFlowSize = CurrentDims.*MainFlowMember;
            }
            bool WindowUpdated = SubWindow.Window->Updated();
            ReturnValue = ReturnValue || WindowUpdated;
            if(WindowUpdated || 
                    (std::tie(SubWindow.FlowIndex,SubWindow.FlowPosition,SubWindow.OtherFlowPosition) 
                     != std::tie(CurrentFlowIndex,CurrentOverflow,TotalOtherFlowSize)))
            {
                SubWindow.Redraw = true;
            }
            SubWindow.FlowIndex = CurrentFlowIndex;
            SubWindow.FlowPosition = CurrentOverflow;
            SubWindow.OtherFlowPosition = TotalOtherFlowSize;
        }
        if(CurrentOtherFlowSize != 0 || CurrentFlowIndex == m_FlowSizes.size())
        {
            m_FlowSizes.push_back(CurrentOtherFlowSize);   
        }
        TotalOtherFlowSize += CurrentOtherFlowSize;
        m_PreferedDims.*OtherFlowDirection = TotalOtherFlowSize;
        m_PreferedDims.*MainFlowMember = MainFlowSize;
        if(m_Border)
        {
            m_PreferedDims.Height += 2;
            m_PreferedDims.Width += 2;
        }
        m_PreferedDims.Height = std::min(m_PreferedDims.Height,m_Dims.Height);
        m_PreferedDims.Width = std::min(m_PreferedDims.Width,m_Dims.Width);
        return ReturnValue;
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
        if(m_Dims.Height == -1 || m_Dims.Width == -1)
        {
            m_Dims.Height = SuggestedDimensions.Height;
            m_Dims.Width = SuggestedDimensions.Width;
            m_Dims.Height = m_Dims.Height < 0 ? 0 : m_Dims.Height;
            m_Dims.Width = m_Dims.Width < 0 ? 0 : m_Dims.Width;
            m_Updated = true;
        }
        if(m_Updated)
        {
            p_AssignDimensions();   
        }
        //MBCLI::Dimensions ReturnValue = SuggestedDimensions;
        //int MBCLI::Dimensions::* MainFlowMember = m_VerticalFlow ? &MBCLI::Dimensions::Height : &MBCLI::Dimensions::Width;;
        //int MBCLI::Dimensions::* OtherFlowDirection = MainFlowMember == &MBCLI::Dimensions::Height ? &MBCLI::Dimensions::Width : &MBCLI::Dimensions::Height;
        //ReturnValue.*OtherFlowDirection = 0;
        //int TotalOtherFlowSize = 0;
        //for(auto& Child : *this)
        //{
        //    auto ChildPreferedDims = Child.Window->PreferedDimensions(SuggestedDimensions);
        //    ReturnValue.*MainFlowMember += ChildPreferedDims.*MainFlowMember;
        //    ReturnValue.*OtherFlowDirection = std::max(ReturnValue.*OtherFlowDirection,ChildPreferedDims.*OtherFlowDirection);
        //}
        //if(ReturnValue.Height >= SuggestedDimensions.Height)
        //{
        //    ReturnValue.Height = SuggestedDimensions.Height;
        //}
        //if(ReturnValue.Width >= SuggestedDimensions.Width)
        //{
        //    ReturnValue.Width = SuggestedDimensions.Width;
        //}
        return m_PreferedDims;
    }
}
