#include "Stacker.h"

namespace MBTUI
{
    bool Stacker::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        //navigation
        //
        
        
        if(m_SelectedIndex >= 0 && m_SelectedIndex < m_StackedWindows.size())
        {
            if(m_SubWindowActive)
            {
                auto Result = m_StackedWindows[m_SelectedIndex].Window->HandleInput(Input);
                if(!Result)
                {
                    m_SubWindowActive = false;
                    //if(m_SelectedIndex != -1 && m_SelectedIndex < m_StackedWindows.size())
                    //{
                    //    m_StackedWindows[m_SelectedIndex].Window->SetFocus(false);
                    //}
                }
                return true;
            }
        }
        if(m_InputPassthrough.size() > 0)
        {
            auto String = Input.GetStringRepresentation();
            auto It = std::lower_bound(m_InputPassthrough.begin(),m_InputPassthrough.end(),String);
            if(It != m_InputPassthrough.end() && *It == String)
            {
                return m_StackedWindows[m_SelectedIndex].Window->HandleInput(Input);
            }
        }
        if(Input.SpecialInput == MBCLI::SpecialKey::Esc)
        {
            return false;
        }
        int AxisIncrease = 0;
        int FlowIncrease = 0;

        if(Input.CharacterInput == "i")
        {
            m_SubWindowActive = true;   
            //if(m_SelectedIndex != -1 && m_SelectedIndex < m_StackedWindows.size())
            //{
            //    m_StackedWindows[m_SelectedIndex].Window->SetFocus(true);
            //}
            return true;
        }

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
            if(Input.SpecialInput == MBCLI::SpecialKey::Esc)
            {
                return false;
            }
            return true;
        }
        if(Input.CharacterInput == "j")
        {
            if(m_VerticalFlow)
            {
                AxisIncrease = m_Reversed ? -1 : 1;
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
                AxisIncrease = m_Reversed ? 1 : -1;
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
                AxisIncrease = m_Reversed ? 1 : -1;
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
                AxisIncrease = m_Reversed ? -1 : 1;
            }
        }

        if(AxisIncrease != 0)
        {
            m_SelectedIndex += AxisIncrease;
            m_SelectedIndex = std::max(m_SelectedIndex,0);
            if(m_StackedWindows.size() > 0)
            {
                m_SelectedIndex = std::min(m_StackedWindows.size()-1,(size_t)m_SelectedIndex);
            }
            else
            {
                m_SelectedIndex = 0;
            }
        }
        else if(FlowIncrease != 0)
        {

            if(m_SelectedIndex < m_StackedWindows.size())
            {
                auto& SelectedWindow = m_StackedWindows[m_SelectedIndex];
                auto TargetFlow = SelectedWindow.FlowIndex + FlowIncrease;
                auto SelectedFlowPosition = SelectedWindow.FlowPosition - (m_VerticalFlow ? SelectedWindow.Dims.Height : SelectedWindow.Dims.Width)/2;

                auto FlowCompLower = [](SubWindow const& lhs,int rhs)
                {
                    return lhs.FlowIndex < rhs;
                };
                auto FlowCompHigher = [](int lhs,SubWindow const& rhs)
                {
                    return lhs < rhs.FlowIndex;
                };
                auto LowerBound = std::lower_bound(begin(),end(),TargetFlow,FlowCompLower);
                auto HigherBound = std::upper_bound(begin(),end(),TargetFlow,FlowCompHigher);
                auto Result = std::upper_bound(LowerBound,HigherBound,SelectedFlowPosition,[](int lhs,SubWindow const& rhs)
                        {
                            return lhs < rhs.FlowPosition;
                        });
                if(Result != HigherBound)
                {
                    m_SelectedIndex = Result.GetOffset();
                }
            }
        }
        if(m_SelectedIndex < m_StackedWindows.size())
        {
            auto& SelectedWindow = m_StackedWindows[m_SelectedIndex];
            try
            {
                SelectedWindow.Window->SetFocus(true);
            }
            catch(...)
            {
                   
            }
            //modify display offset so the element is included
            int FirstRowOffset = m_Border ? 1 : 0;
            int LastRowOffset = m_Border ? m_Dims.Height-1 : m_Dims.Height;
            int FirstColumnOffset = m_Border ? 1 : 0;
            int LastColumnOffset = m_Border ? m_Dims.Width-1 : m_Dims.Width;


            auto AbsoluteOffset = SelectedWindow.Offsets;
            AbsoluteOffset.Height += m_DisplayOffset.Height;
            AbsoluteOffset.Width += m_DisplayOffset.Width;
            if(AbsoluteOffset.Height < FirstRowOffset || AbsoluteOffset.Height + SelectedWindow.Dims.Height > LastRowOffset)
            {
                auto TopDiff = LastRowOffset - (AbsoluteOffset.Height + SelectedWindow.Dims.Height);
                auto BottomDiff =FirstRowOffset-AbsoluteOffset.Height;
                if(std::abs(TopDiff) < std::abs(BottomDiff))
                {
                    m_DisplayOffset.Height = LastRowOffset-(SelectedWindow.Offsets.Height + SelectedWindow.Dims.Height);
                }
                else
                {
                    m_DisplayOffset.Height = FirstRowOffset-SelectedWindow.Offsets.Height;
                }
                SetUpdated(true);
            }
            if(AbsoluteOffset.Width < FirstColumnOffset || AbsoluteOffset.Width + SelectedWindow.Dims.Width > LastColumnOffset)
            {
                auto TopDiff = LastColumnOffset - (AbsoluteOffset.Width + SelectedWindow.Dims.Width);
                auto BottomDiff = FirstColumnOffset - AbsoluteOffset.Width;
                if(std::abs(TopDiff) < std::abs(BottomDiff))
                {
                    m_DisplayOffset.Width= LastColumnOffset-(SelectedWindow.Offsets.Width + SelectedWindow.Dims.Width);
                }
                else
                {
                    m_DisplayOffset.Width = FirstColumnOffset-SelectedWindow.Offsets.Width;
                }
                SetUpdated(true);
            }
        }
        return true;
    }
    void Stacker::p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw)
    {
        MBCLI::Dimensions DrawOffsets;
        if(m_Border)
        {
            DrawOffsets.Height = 1;
            DrawOffsets.Width = 1;
        }
        else
        {
            DrawOffsets.Height = 0;
            DrawOffsets.Width = 0;
        }
        auto ClearView = View.SubView(0,0);
        if(m_Border)
        {
            ClearView.ModifyAllowedArea(1,1,m_Dims.Height-2,m_Dims.Width-2);
        }
        for(auto& Window : *this)
        {
            auto DrawOffset = Window.Offsets;
            DrawOffset.Height += m_DisplayOffset.Height;
            DrawOffset.Width += m_DisplayOffset.Width;

            if((DrawOffset != Window.PreviousOffsets || Window.Dims != Window.PreviousDims) && 
                    (Window.PreviousDims != MBCLI::Dimensions() && Window.PreviousOffsets != MBCLI::Dimensions()))
            {
                if(!Redraw)
                {
                    ClearView.Clear(
                               Window.PreviousOffsets.Height,
                               Window.PreviousOffsets.Width,
                               Window.PreviousDims.Width,
                               Window.PreviousDims.Height);
                    ClearView.Clear(
                               DrawOffset.Height,
                               DrawOffset.Width,
                               Window.Dims.Width,
                               Window.Dims.Height);
                }
                Window.Redraw = true;
            }
        }

        if(m_Border && (!m_BorderDrawn || Redraw))
        {
            auto PreviousColor = View.GetWriteColor();
            View.SetWriteColor(m_BorderColor);
            MBCLI::DrawBorder(View,0,0,m_PreferedDims.Width,m_PreferedDims.Height);
            View.SetWriteColor(PreviousColor);
            m_BorderDrawn = true;
        }
        if(m_Dims.Width <= 0 || m_Dims.Height <= 0)
        {
            return;
        }
        if(m_Border)
        {
            View.ModifyAllowedArea(1,1,m_Dims.Height-2,m_Dims.Width-2);
        }
        for(auto& Window : *this)
        {
            bool RedrawWindow = Redraw || Window.Redraw;
            auto AbsoluteOffset = Window.Offsets;
            AbsoluteOffset.Height += m_DisplayOffset.Height;
            AbsoluteOffset.Width += m_DisplayOffset.Width;
            if(RedrawWindow || Window.Window->Updated())
            {
                //MBCLI::Dimensions CurrentDrawOffset;
                //CurrentDrawOffset.Height = std::max(DrawOffsets.Height-Window.Offsets.Height,0);
                //CurrentDrawOffset.Width = std::max(DrawOffsets.Width-Window.Offsets.Width,0);
                Window.Window->WriteBuffer(View.SubView(AbsoluteOffset.Height,AbsoluteOffset.Width,Window.Dims),RedrawWindow);
            }
            Window.PreviousDims = Window.Dims;
            Window.PreviousOffsets = AbsoluteOffset;
            Window.Redraw = false;
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
            m_BorderDrawn = false;
        }
        m_Border = HasBorder;
    }
    void Stacker::SetBorderColor(MBCLI::TerminalColor Color)
    {
        if(m_BorderColor != Color)
        {
            SetUpdated(true);
            m_BorderDrawn = false;
        }
        m_BorderColor = Color;
    }
    void Stacker::SetTextColor(MBCLI::TerminalColor Color)
    {
        if(m_TextColor != Color)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }
        m_TextColor = Color;
    }
    bool Stacker::ParseJustification(std::string_view View,Justification& Out)
    {
        bool ReturnValue = true;
        if(View == "start")
        {
            Out = Justification::Start;
        }
        else if(View == "end")
        {
            Out = Justification::End;
        }
        else if(View == "center")
        {
            Out = Justification::Center;
        }
        else if(View == "between")
        {
            Out = Justification::Between;
        }
        else if(View == "evenly")
        {
            Out = Justification::Evenly;
        }
        else
        {
            ReturnValue = false;
        }
        return ReturnValue;
    }
    void Stacker::SetJustification(Justification NewJustification)
    {
        if(m_ContentJustification != NewJustification)
        {
            SetUpdated(true);
        }
        m_ContentJustification = NewJustification;
    }
    void Stacker::SetBGColor(MBCLI::TerminalColor Color)
    {
        if(m_BGColor != Color)
        {
            SetUpdated(true);   
            m_Redraw = true;
        }

        m_BGColor = Color;
    }
    void Stacker::SetInputPassthrough(std::vector<std::string> Keys)
    {
        m_InputPassthrough = std::move(Keys);
        std::sort(m_InputPassthrough.begin(),m_InputPassthrough.end());
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
        std::vector<FlowRowInfo> FlowRows;
        bool NewDims = m_LastDims != m_Dims;
        m_LastDims = m_Dims;
        MBCLI::Dimensions CurrentDims = m_SizeSpec.GetDims(m_Dims);
        if(m_Border)
        {
            CurrentDims.Width -= 2;
            CurrentDims.Height -= 2;
        }
        if(CurrentDims.Width <= 0 || CurrentDims.Height <= 0)
        {
            return false;
        }
        if(m_StackedWindows.size() > 0)
        {
            FlowRows.emplace_back();
        }
        for(auto& SubWindow : *this)
        {
            if(NewDims || SubWindow.Window->Updated())
            {
                SubWindow.Dims = SubWindow.Window->PreferedDimensions(CurrentDims);
                SubWindow.Dims.Height = std::min(SubWindow.Dims.Height,CurrentDims.Height);
                SubWindow.Dims.Width = std::min(SubWindow.Dims.Width,CurrentDims.Width);
            }
            SubWindow.Dims.*MainFlowMember = SubWindow.Dims.*MainFlowMember < 0 ? 1 : SubWindow.Dims.*MainFlowMember;
            if(m_Overflow)
            {
                SubWindow.Dims.*OtherFlowDirection = (m_FlowWidth > 0) ? m_FlowWidth : SubWindow.Dims.*OtherFlowDirection;
            }

            CurrentOverflow += SubWindow.Dims.*MainFlowMember;
            MainFlowSize = std::max(MainFlowSize,CurrentOverflow);

            if(CurrentOverflow > CurrentDims.*MainFlowMember && CurrentOtherFlowSize != 0)
            {
                CurrentFlowIndex += 1;
                FlowRows.back().Size = CurrentOverflow;
                CurrentOverflow = SubWindow.Dims.*MainFlowMember;
                TotalOtherFlowSize += CurrentOtherFlowSize;
                m_FlowSizes.push_back(CurrentOtherFlowSize);
                FlowRows.emplace_back();
                CurrentOtherFlowSize = SubWindow.Dims.*OtherFlowDirection;
            }
            FlowRows.back().ElementCount += 1;
            CurrentOtherFlowSize = std::max(CurrentOtherFlowSize,SubWindow.Dims.*OtherFlowDirection);

            SubWindow.FlowIndex = CurrentFlowIndex; 
            SubWindow.FlowPosition = CurrentOverflow;
            SubWindow.OtherFlowPosition = TotalOtherFlowSize;
        }
        if(CurrentOtherFlowSize != 0 || CurrentFlowIndex == m_FlowSizes.size())
        {
            m_FlowSizes.push_back(CurrentOtherFlowSize);   
        }
        if(FlowRows.size() > 0)
        {
            FlowRows.back().Size = CurrentOverflow;
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

        if(!m_SizeSpec.Empty())
        {
            auto SpecDims = m_SizeSpec.GetDims(m_Dims);
            if(m_SizeSpec.HeightSpecified())
            {
                m_PreferedDims.Height = SpecDims.Height;
            }
            if(m_SizeSpec.WidthSpecified())
            {
                m_PreferedDims.Width = SpecDims.Width;   
            }
        }


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
                    CurrentOffsets.*OtherFlowDirection -= m_FlowSizes[CurrentFlowIndex];
                }
                else
                {
                    CurrentOffsets.*OtherFlowDirection += m_FlowSizes[CurrentFlowIndex];
                }
                CurrentFlowIndex = SubWindow.FlowIndex;
            }
            auto OffsetDims = CurrentOffsets;
            OffsetDims.*MainFlowMember += (SubWindow.FlowPosition - SubWindow.Dims.*MainFlowMember);
            SubWindow.Offsets = OffsetDims;
        }
        //pass using justification, which only modifies SubWindow.FlowPosition
        if(m_ContentJustification != Justification::Start)
        {
            size_t WindowIndex = 0;
            auto begin = this->begin();
            for(auto const& Row : FlowRows)
            {
                p_Justify(begin+WindowIndex,begin+WindowIndex+Row.ElementCount,Row,CurrentDims.*MainFlowMember,MainFlowMember,m_ContentJustification);
                WindowIndex += Row.ElementCount;
            }
        }
        //another pass to change position according to the current view offse
        //if(std::tie(m_DisplayOffset.Width,m_DisplayOffset.Width) != std::tuple(0,0))
        //{
        //    for(auto& SubWindow : *this)
        //    {
        //        SubWindow.Offsets.Width += m_DisplayOffset.Width;
        //        SubWindow.Offsets.Height += m_DisplayOffset.Height;
        //    }
        //}
        return ReturnValue;
    }
    void Stacker::SetFocus(bool IsFocused)
    {
        //
    }
    MBCLI::CursorInfo Stacker::GetCursorInfo() 
    {
        if(m_SubWindowActive)
        {
            if(m_SelectedIndex >= 0 && m_SelectedIndex < m_StackedWindows.size())
            {
                return m_StackedWindows[m_SelectedIndex].Window->GetCursorInfo();
            }
        }
        return MBCLI::CursorInfo();
    }
    void Stacker::WriteBuffer(MBCLI::BufferView View,bool Redraw) 
    {
        bool AssignDims = m_AssignDims;
        if(m_Redraw)
        {
            Redraw = true;   
            m_Redraw = false;
        }
        if(m_TextColor != MBCLI::ANSITerminalColor::Default)
        {
            View.SetCascadingWriteColor(m_TextColor);
        }
        if(m_BGColor != MBCLI::ANSITerminalColor::Default)
        {
            View.SetCascadingBGColor(m_BGColor);
        }
        if(View.GetDimensions() != m_Dims)
        {
            m_Dims = View.GetDimensions();
            AssignDims = true;
            m_BorderDrawn = false;
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
        if(m_LastDrawDims != m_PreferedDims)
        {
            m_BorderDrawn = false;
        }
        m_LastDrawDims = m_PreferedDims;
        p_UpdateBuffer(View,Redraw);
        SetUpdated(false);
    }
    MBCLI::Dimensions Stacker::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        bool AssignDims = false;
        if(m_Dims != SuggestedDimensions || Updated())
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
