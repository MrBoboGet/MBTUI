#include "Text.h"


namespace MBTUI
{
       
    std::string_view Text::GetContent() const
    {
        return m_Content;
    }
    SizeSpecification Text::GetSizeSpec() const
    {
        return m_SizeSpec;
    }
    void Text::SetSizeSpec(SizeSpecification NewSpec) 
    {
        m_SizeSpec = NewSpec;
    }

    void Text::HandleInput(MBCLI::ConsoleInput const& Input)
    {
           
    }
    MBCLI::Dimensions Text::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions)
    {
        MBCLI::Dimensions Dims;
        Dims.Width = m_Content.size();
        Dims.Height = 1;
        Dims = m_SizeSpec.ModifyDims(Dims,SuggestedDimensions);
        return Dims;
    }
    void Text::SetFocus(bool IsFocused)
    {
        if(m_Focus != IsFocused)
        {
            SetUpdated(true);
        }
        m_Focus = IsFocused;
    }
    MBCLI::CursorInfo Text::GetCursorInfo()
    {
        return MBCLI::CursorInfo();
    }
    void Text::SetColor(MBCLI::TerminalColor Color)
    {
        if(m_Color != Color)
        {
            SetUpdated(true);   
            m_Color = Color;
        }
    }
    void Text::SetText(std::string Content)
    {
        std::swap(m_Content,Content);
        SetUpdated(true);
    }
    void Text::SetHighlightColor(MBCLI::TerminalColor Color)
    {
        if(m_HighlightColor != Color)
        {
            SetUpdated(true);
            m_HighlightColor = Color;
        }
    }
    void Text::WriteBuffer(MBCLI::BufferView View,bool Redraw)
    {
        View.SetWriteColor(m_Focus ? m_HighlightColor : m_Color);
        View.WriteCharacters(0,0,m_Content);
        SetUpdated(false);
    }
}
