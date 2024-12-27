#include "Stacker.h"

namespace MBTUI
{
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
        if(m_Border)
        {
            auto PreviousColor = View.GetWriteColor();
            View.SetWriteColor(m_BorderColor);
            MBCLI::DrawBorder(View,0,0,m_PreferedDims.Width,m_PreferedDims.Height);
            View.SetWriteColor(PreviousColor);
        }
        if(m_Dims.Width <= 0 || m_Dims.Height <= 0)
        {
            return;
        }

        for(auto& Window : *this)
        {
            bool RedrawWindow = Redraw;
            if((Window.Offsets != Window.PreviousOffsets || Window.Dims != Window.PreviousDims) && 
                    (Window.PreviousDims != MBCLI::Dimensions() && Window.PreviousOffsets != MBCLI::Dimensions()))
            {
                View.Clear(Window.PreviousOffsets.Height,Window.PreviousOffsets.Width,Window.PreviousDims.Width,Window.PreviousDims.Height);
                View.Clear(Window.Offsets.Height,Window.Offsets.Width,Window.Dims.Width,Window.Dims.Height);
                RedrawWindow = true;
            }
            if(RedrawWindow || Window.Window->Updated())
            {
                Window.Window->WriteBuffer(View.SubView(Window.Offsets.Height,Window.Offsets.Width,Window.Dims),RedrawWindow);
            }
            Window.PreviousDims = Window.Dims;
            Window.PreviousOffsets = Window.Offsets;
        }
    }
    void Stacker::SetFlowDirection(bool IsVertical)
    {
        if(m_VerticalFlow != IsVertical)
        {
            SetUpdated(true);
        }
        m_VerticalFlow = IsVertical;
    }
    void Stacker::SetFlowWidth(int Size)
    {
        if(m_FlowWidth != Size)
        {
            SetUpdated(true);
        }
        m_FlowWidth = Size;
    }
    void Stacker::EnableOverflow(bool OverlowEnabled)
    {
        if(m_Overflow != OverlowEnabled)
        {
            SetUpdated(true);
        }
        m_Overflow = OverlowEnabled;
    }
    void Stacker::SetBorder(bool HasBorder)
    {
        if(m_Border != HasBorder)
        {
            SetUpdated(true);
            m_ClearView = true;
        }
        m_Border = HasBorder;
    }
    void Stacker::SetBorderColor(MBCLI::TerminalColor Color)
    {
        if(m_BorderColor != Color)
        {
            SetUpdated(true);
        }
        m_BorderColor = Color;
    }

    void Stacker::SetOverflowDirection(bool Reversed)
    {
        if(m_OverflowReversed != Reversed)
        {
            SetUpdated(true);
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

        const size_t PreviousFlowSizes = m_FlowSizes.size();
        m_FlowSizes.clear();


        bool NewDims = m_LastDims != m_Dims;
        m_LastDims = m_Dims;
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
            if(NewDims || SubWindow.Window->Updated())
            {
                SubWindow.Dims = SubWindow.Window->PreferedDimensions(CurrentDims);
            }
            SubWindow.Dims.*MainFlowMember = SubWindow.Dims.*MainFlowMember < 0 ? 1 : SubWindow.Dims.*MainFlowMember;
            if(m_Overflow)
            {
                SubWindow.Dims.*OtherFlowDirection = (m_FlowWidth > 0) ? m_FlowWidth : SubWindow.Dims.*OtherFlowDirection;
            }

            CurrentOverflow += SubWindow.Dims.*MainFlowMember;
            CurrentOtherFlowSize = std::max(CurrentOtherFlowSize,SubWindow.Dims.*OtherFlowDirection);
            MainFlowSize = std::max(MainFlowSize,CurrentOverflow);

            if(CurrentOverflow > CurrentDims.*MainFlowMember)
            {
                CurrentFlowIndex += 1;
                CurrentOverflow = SubWindow.Dims.*MainFlowMember;
                TotalOtherFlowSize += CurrentOtherFlowSize;
                m_FlowSizes.push_back(CurrentOtherFlowSize);
                CurrentOtherFlowSize = SubWindow.Dims.*OtherFlowDirection;
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
        m_PreferedDims.*MainFlowMember = std::min(CurrentDims.*MainFlowMember,MainFlowSize);
        if(m_Border)
        {
            m_PreferedDims.Height += 2;
            m_PreferedDims.Width += 2;
        }
        m_PreferedDims.Height = std::min(m_PreferedDims.Height,m_Dims.Height);
        m_PreferedDims.Width = std::min(m_PreferedDims.Width,m_Dims.Width);


        //Calculate the absolut offsets

        MBCLI::Dimensions CurrentOffsets;
        CurrentOffsets.Height = 0;
        CurrentOffsets.Width = 0;
        if(m_Border)
        {
            CurrentOffsets.Width += 1;
            CurrentOffsets.Height += 1;
        }
        if(m_OverflowReversed)
        {
            CurrentOffsets.*OtherFlowDirection = m_PreferedDims.*OtherFlowDirection-m_FlowSizes[0];
            if(m_Border)
            {
                CurrentOffsets.*OtherFlowDirection -= 1;
            }
        }
        CurrentFlowIndex = 0;
        for(auto& SubWindow : *this)
        {
            //bool ClearPreviou/s
            if(SubWindow.FlowIndex != CurrentFlowIndex)
            {
                if(m_OverflowReversed)
                {
                    CurrentOffsets.*OtherFlowDirection -= m_FlowSizes[SubWindow.FlowIndex];
                }
                else
                {
                    CurrentOffsets.*OtherFlowDirection += m_FlowSizes[SubWindow.FlowIndex];
                }
                CurrentFlowIndex = SubWindow.FlowIndex;
            }
            auto OffsetDims = CurrentOffsets;
            OffsetDims.*MainFlowMember += (SubWindow.FlowPosition - SubWindow.Dims.*MainFlowMember);
            SubWindow.Offsets = OffsetDims;
        }

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
        bool AssignDims = m_AssignDims;
        if(View.GetDimensions() != m_Dims)
        {
            m_Dims = View.GetDimensions();
            AssignDims = true;
        }
        if(m_ClearView)
        {
            View.Clear();   
            m_ClearView = false;
        }
        m_AssignDims = false;
        if(AssignDims || ChildUpdated())
        {
            p_AssignDimensions();
        }
        p_UpdateBuffer(View,Redraw);
        SetUpdated(false);
    }
    MBCLI::Dimensions Stacker::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        bool AssignDims = false;
        if(m_Dims != SuggestedDimensions || ChildUpdated())
        {
            AssignDims = true;
        }
        m_Dims = SuggestedDimensions;
        m_Dims.Height = m_Dims.Height < 0 ? 0 : m_Dims.Height;
        m_Dims.Width = m_Dims.Width < 0 ? 0 : m_Dims.Width;
        if(AssignDims)
        {
            p_AssignDimensions();
        }
        return m_PreferedDims;
    }
}
