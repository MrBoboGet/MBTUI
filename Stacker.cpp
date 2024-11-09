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
        //nothing
    }
    void Stacker::p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw)
    {
        int RowOffset = 0;
        for(auto& Window : *this)
        {
            if(RowOffset > m_Dims.Height)
            {
                break;
            }
            MBCLI::Dimensions UsedDims = Window.Dims;
            UsedDims.Width = m_Dims.Width;
            UsedDims.Height = UsedDims.Height < 0 ? 1 : UsedDims.Height;
            Window.Window->WriteBuffer(View.SubView(RowOffset,0,UsedDims),Redraw);
            RowOffset += UsedDims.Height;
        }
    }
    void Stacker::p_AssignDimensions()
    {
        MBCLI::Dimensions SuggestedDims = MBCLI::Dimensions(m_Dims.Width,m_Dims.Height);
        if(m_Reversed)
        {
            for(auto& SubWindow : m_StackedWindows)
            {
                SubWindow.Dims = SubWindow.Window->PreferedDimensions(SuggestedDims);
            }
        }
        else
        {
            for(int i = m_StackedWindows.size()-1;i >= 0;i--)
            {
                auto& SubWindow = m_StackedWindows[i];
                SubWindow.Dims = SubWindow.Window->PreferedDimensions(SuggestedDims);
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
        m_Updated = false;
        if(View.GetDimensions() != m_Dims)
        {
            m_Dims = View.GetDimensions();
            p_AssignDimensions();
        }
        p_UpdateBuffer(View,Redraw);
    }
    MBCLI::Dimensions Stacker::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        MBCLI::Dimensions ReturnValue;
        ReturnValue.Width = SuggestedDimensions.Width;
        ReturnValue.Height = 0;
        for(auto& Child : *this)
        {
            auto ChildPreferedDims = Child.Window->PreferedDimensions(SuggestedDimensions);
            if(ChildPreferedDims.Height < 0)
            {
                ReturnValue.Height = SuggestedDimensions.Height;
                return ReturnValue;
            }
            ReturnValue.Height += ChildPreferedDims.Height;
        }
        if(ReturnValue.Height >= SuggestedDimensions.Height)
        {
            ReturnValue.Height = SuggestedDimensions.Height;
        }
        return ReturnValue;
    }
}
