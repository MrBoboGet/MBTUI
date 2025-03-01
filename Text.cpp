#include "Text.h"
#include <cctype>
#include <MBUnicode/MBUnicode.h>


namespace MBTUI
{
       
    bool Text::p_IsWord(unsigned char const* Begin,unsigned char const* End) const
    {
        if((End-Begin) != 1)
        {
            return true;   
        }
        return !(Begin[0] == ' ' || Begin[0] == '\t' || Begin[0] == '\v');
    }
    std::vector<std::pair<int_least32_t,int_least32_t>> Text::p_Tokenize(MBCLI::Dimensions Dims,std::string_view Content) const
    {
        std::vector<std::pair<int_least32_t,int_least32_t>> ReturnValue;

        int_least32_t LineBegin = 0;
        int_least32_t WordBegin = 0;
        int_least32_t WordCharacters = 0;

        int CurrentLineCharacters = 0;

        unsigned char const* const Begin = (unsigned const char *)Content.data();
        unsigned char const* NextChar = (unsigned const char *)Content.data();
        unsigned char const* End = (unsigned const char *)Content.data()+Content.size();
        while(NextChar != End)
        {
            auto GraphemeEnd = MBUnicode::GraphemeClusterSegmenter::ParseGraphemeCluster(NextChar,End);
            auto Size = GraphemeEnd-NextChar;
            auto CurrentCharOffset = NextChar-Begin;
            auto CurrentCharEnd = GraphemeEnd-Begin;

            if(Size == 1 && *NextChar == '\n' || (Size == 2 && NextChar[0] == '\r' && NextChar[1] == '\n' ))
            {
                ReturnValue.push_back({LineBegin,CurrentCharOffset});
                WordBegin = -1;
                LineBegin = CurrentCharEnd;
                CurrentLineCharacters = 0;
                if(!m_Multiline)
                {
                    break;   
                }
            }
            else if(CurrentLineCharacters == Dims.Width)
            {
                if(!m_Multiline)
                {
                    break;   
                }

                //word boundary should carry over, unless it's larger than the whole row
                if(WordBegin != -1 && WordBegin != LineBegin)
                {
                    ReturnValue.push_back({LineBegin,WordBegin});
                    LineBegin = WordBegin;
                    CurrentLineCharacters = WordCharacters + (p_IsWord(NextChar,GraphemeEnd) ? 1 : 0);
                }
                else
                {
                    ReturnValue.push_back({LineBegin,CurrentCharOffset});
                    LineBegin = CurrentCharOffset;
                    WordBegin = p_IsWord(NextChar,GraphemeEnd) ? LineBegin : -1; 
                    CurrentLineCharacters = WordBegin == -1 ? 0 : 1;
                }
            }
            else if(p_IsWord(NextChar,GraphemeEnd))
            {
                if(WordBegin == -1)
                {
                    WordBegin  = CurrentCharOffset;
                    WordCharacters = 1;
                }
                else
                {
                    WordCharacters += 1;
                }
                CurrentLineCharacters += 1;
            }
            else 
            {
                WordBegin = -1;
                WordCharacters = 0;
                CurrentLineCharacters += 1;
            }
            NextChar = GraphemeEnd;
        }
        if(LineBegin < Content.size())
        {
            ReturnValue.push_back({LineBegin,Content.size()});
        }

        return ReturnValue;
    }
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
        Dims = m_SizeSpec.GetDims(SuggestedDimensions);

        if(!m_Multiline)
        {
            Dims.Width = m_Content.size();
            Dims.Height = 1;
        }
        else
        {
            auto Tokens = p_Tokenize(Dims,m_Content);
            Dims.Height = Tokens.size();
        }
       
        if(Dims.Width > SuggestedDimensions.Width)
        {
            Dims.Width = SuggestedDimensions.Width;
        }
        if(Dims.Height > SuggestedDimensions.Height)
        {
            Dims.Height = SuggestedDimensions.Height;   
        }
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
    void Text::SetBGColor(MBCLI::TerminalColor Color)
    {
        if(m_BGColor != Color)
        {
            SetUpdated(true);   
            m_BGColor = Color;
        }
    }
    void Text::SetText(std::string Content)
    {
        std::swap(m_Content,Content);
        m_Recalculate = true;
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
    void Text::SetMultiline(bool Multiline)
    {
        if(m_Multiline != Multiline)
        {
            SetUpdated(true);
            m_Multiline = Multiline;
        }
    }
    void Text::WriteBuffer(MBCLI::BufferView View,bool Redraw)
    {
        if(m_LastWriteDims != View.GetDimensions() || m_Recalculate)
        {
            m_LastWriteDims = View.GetDimensions();
            m_LineOffsets = p_Tokenize(View.GetDimensions(),m_Content);
            m_Recalculate = false;
        }
        View.SetWriteColor(m_Focus ? m_HighlightColor : m_Color);
        View.SetBGColor(m_BGColor);

        int LineOffset = 0;
        for(auto It = m_LineOffsets.rbegin();It != m_LineOffsets.rend();++It)
        {
            View.WriteCharacters(LineOffset,0,std::string_view(m_Content.data()+It->first,It->second-It->first));
            LineOffset += 1;
        }


        SetUpdated(false);
    }
}
