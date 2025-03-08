#include "MBTUI.h"

namespace MBTUI
{



    REPL_Line REPL::p_TokenizeString()
    {
        REPL_Line ReturnValue;
        bool Escaped = false;
        bool IsString = false;
        std::string CurrentToken;
        int CurrentOffset = 0;
        for(auto const& Character : m_LineBuffer)
        {
            bool IsWhiteSpace = true;
            auto CurrentString = Character.ToString();
            for(auto Character : CurrentString)
            {
                if(!std::isspace(Character))
                {
                    IsWhiteSpace = false;
                    break;
                }
            }
            if(CurrentOffset == m_CursorPosition && (!IsWhiteSpace || CurrentToken.size() > 0))
            {
                ReturnValue.TokenIndex = ReturnValue.Tokens.size();
            }
            if(IsWhiteSpace && !IsString)
            {
                if(CurrentToken.size() > 0)
                {
                    ReturnValue.Tokens.push_back(std::move(CurrentToken));
                    CurrentToken.clear();
                }
            }
            else if(CurrentString == "\"")
            {
                if(!IsString)
                {
                    IsString = true;
                }
                else
                {
                    if(!Escaped)
                    {
                        IsString = false;
                    }
                    else
                    {
                        CurrentToken += CurrentString;
                    }
                }
            }
            else if(CurrentString == "\\" && IsString && !Escaped)
            {
                Escaped = true;
            }
            else
            {
                Escaped = false;
                CurrentToken += CurrentString;
            }
            CurrentOffset += 1;
        }
        if(CurrentToken.size() > 0)
        {
            if(m_CursorPosition == m_LineBuffer.size() && m_LineBuffer.size() > 0 && m_LineBuffer.back().ToString() != " ")
            {
                ReturnValue.TokenIndex = ReturnValue.Tokens.size();
            }
            ReturnValue.Tokens.push_back(std::move(CurrentToken));   
        }
        return ReturnValue;
    }
    std::string REPL::p_GetCompletion(REPL_Line const& Line)
    {
        std::string ReturnValue;
        auto const& CompareString = Line.Tokens[Line.TokenIndex];
        std::vector<std::string> Alternatives;
        for(auto& Func : m_CompletionFuncs)
        {
            auto NewCompletions = Func(Line);
            Alternatives.insert(Alternatives.end(),std::make_move_iterator(NewCompletions.begin()),std::make_move_iterator(NewCompletions.end()));
        }
        std::sort(Alternatives.begin(),Alternatives.end());
        std::vector<size_t> MatchingIndexes;
        for(size_t i = 0; i < Alternatives.size();i++)
        {
            auto const& CurrentString = Alternatives[i];
            size_t MatchingCount = std::mismatch(CompareString.begin(),CompareString.end(),
                    CurrentString.begin(),CurrentString.end()).first-CompareString.begin();
            if(MatchingCount == CompareString.size())
            {
                MatchingIndexes.push_back(i);
            }
            else if(MatchingIndexes.size() > 0)
            {
                break;
            }
        }
        if(MatchingIndexes.size() == 1)
        {
            ReturnValue = Alternatives[MatchingIndexes.front()];
        }
        else if(MatchingIndexes.size() > 1)
        {
            size_t LongestCommonPrefix = std::numeric_limits<size_t>::max();
            auto const& FirstAlternative = Alternatives[MatchingIndexes.front()];
            for(size_t i = 1; i < Alternatives.size();i++)
            {
                LongestCommonPrefix = std::min(LongestCommonPrefix,
                size_t(std::mismatch(FirstAlternative.begin(),FirstAlternative.end(),
                    Alternatives[i].begin(),Alternatives[i].end()).first-FirstAlternative.begin()));
            }
            ReturnValue = FirstAlternative.substr(0,LongestCommonPrefix);
        }
        return ReturnValue;
    }
    void REPL::AddCompletionFunc(CompletionFuncType Func)
    {
        m_CompletionFuncs.emplace_back(std::move(Func));
    }
    void REPL::SetOnEnterFunc(MBUtility::MOFunction<void(std::string const& Line)> EnterFunc)
    {
        m_EnterFunc = std::move(EnterFunc);   
    }
    void REPL::SetText(std::string_view Content)
    {
        p_SetLine(Content);
        SetUpdated(true);
    }
    std::string REPL::GetLineString()
    {
        std::string ReturnValue;
        for(auto const& Character : m_LineBuffer)
        {
            ReturnValue += Character.ToString();
        }
        return ReturnValue;
    }
    void REPL::Reset()
    {
        m_CursorPosition = 0;
        m_LineBuffer.clear();
    }
    void REPL::p_SetLine(std::string_view const& Data)
    {
        m_LineBuffer.clear();
        MBUnicode::GraphemeCluster::ParseGraphemeClusters(m_LineBuffer, Data.data(), Data.size(),0);
        m_CursorPosition = m_LineBuffer.size();
    }
    bool REPL::HandleInput(MBCLI::ConsoleInput const& Input) 
    {
        SetUpdated(true);
        if (!Input.CharacterInput.IsEmpty())
        {
            if (Input.CharacterInput == '\n' || Input.CharacterInput == "\r\n")
            {
                std::string CurrentLine = GetLineString();
                m_CurrentCommandIndex = -1;
                if(CurrentLine != "")
                {
                    m_History.push_back(CurrentLine);
                }
                Reset();
                if(m_EnterFunc != nullptr)
                {
                    m_EnterFunc(CurrentLine);
                }
                return true;
            }
            else if(Input.CharacterInput == '\t')
            {
                auto Line = p_TokenizeString();
                if(Line.TokenIndex != -1)
                {
                    std::string Completion = p_GetCompletion(Line);
                    if(!Completion.empty())
                    {
                        Line.Tokens[Line.TokenIndex] = Completion;
                        m_LineBuffer.clear();
                        for(int i = 0; i < Line.Tokens.size();i++)
                        {
                            std::vector<MBUnicode::GraphemeCluster> NewClusters;
                            bool Result = MBUnicode::GraphemeCluster::
                                ParseGraphemeClusters(NewClusters, Line.Tokens[i].data(),Line.Tokens[i].size(),0);
                            m_LineBuffer.insert(m_LineBuffer.end(),
                                    std::make_move_iterator(NewClusters.begin()),std::make_move_iterator(NewClusters.end()));
                            if(i == Line.TokenIndex)
                            {
                                m_CursorPosition = m_LineBuffer.size()+1;
                            }
                            m_LineBuffer.push_back(MBUnicode::GraphemeCluster(" "));
                        }
                    }
                }
            }
            else if(!Input.CharacterInput.IsASCIIControl())
            {
                m_LineBuffer.insert(m_LineBuffer.begin()+m_CursorPosition,Input.CharacterInput);
                m_CursorPosition += 1;
            }
        }
        if (Input.SpecialInput != MBCLI::SpecialKey::Null) 
        {
            if (Input.SpecialInput == MBCLI::SpecialKey::Left)
            {
                m_CursorPosition = std::max(m_CursorPosition - 1, 0);
            }
            else if(Input.SpecialInput == MBCLI::SpecialKey::Backspace)
            {
                if (m_CursorPosition > 0)
                {
                    if(m_LineBuffer.size() == 0)
                    {
                        return true;
                    }
                    m_LineBuffer.erase(m_LineBuffer.begin() + m_CursorPosition - 1);
                    m_CursorPosition -= 1;
                }   
            }
            else if (Input.SpecialInput == MBCLI::SpecialKey::Right)
            {
                m_CursorPosition = std::min(m_LineBuffer.size(), size_t(m_CursorPosition + 1));
            }
            if (Input.SpecialInput == MBCLI::SpecialKey::Up)
            {
                if (m_CurrentCommandIndex == -1)
                {
                    m_CurrentCommandIndex = 0;
                    std::string CurrentCommand = GetLineString();
                    if (CurrentCommand != "")
                    {
                        m_History.push_back(std::move(CurrentCommand));
                        m_CurrentCommandIndex += 1;
                    }
                }
                m_CurrentCommandIndex += 1;
                if (m_CurrentCommandIndex > m_History.size())
                {
                    m_CurrentCommandIndex = m_History.size();
                }
                if (m_CurrentCommandIndex > 0)
                {
                    Reset();
                    p_SetLine(m_History[m_History.size()-m_CurrentCommandIndex]);
                }
            }
            if (Input.SpecialInput == MBCLI::SpecialKey::Down)
            {
                if (m_CurrentCommandIndex == 0 || m_CurrentCommandIndex == 1 || m_CurrentCommandIndex == -1)
                {
                    m_CurrentCommandIndex = -1;
                }
                else
                {
                    m_CurrentCommandIndex -= 1;
                    Reset();
                    p_SetLine(m_History[m_History.size()- m_CurrentCommandIndex]);
                }
            }
        }
        return true;
    }
    void REPL::SetFocus(bool IsFocused)
    {
           
    }
    MBCLI::CursorInfo REPL::GetCursorInfo()
    {
        MBCLI::CursorInfo ReturnValue;
        ReturnValue.Hidden = false;
        ReturnValue.Position.ColumnIndex = m_CursorPosition;
        ReturnValue.Position.RowIndex = 0;
        return ReturnValue;
    }
    MBCLI::Dimensions REPL::PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) 
    {
        MBCLI::Dimensions  ReturnValue;
        ReturnValue.Height = 1;
        ReturnValue.Width = SuggestedDimensions.Width;
        return ReturnValue;
    }
    void REPL::SetMaxDims(MBCLI::Dimensions Dims)
    {
        m_MaxDims = Dims;
    }
    
    void  REPL::WriteBuffer(MBCLI::BufferView SuppliedView,bool Redraw) 
    {
        auto View = SuppliedView.SubView(0,0,MBCLI::Dimensions(SuppliedView.GetDimensions().Width,1));
        View.Clear();
        SetUpdated(false);
        m_Dims = View.GetDimensions();
        MBCLI::Dimensions Dims = m_Dims;
        Dims.Height = m_MaxDims.Height < 0 ? Dims.Height : std::min(m_MaxDims.Height,Dims.Height);
        Dims.Width = m_MaxDims.Width < 0 ? Dims.Width : std::min(m_MaxDims.Width,Dims.Width);
        
        //int i = 0;
        //for(auto const& Character : m_LineBuffer)
        //{
        //    ReturnValue.SetCharacter(MBCLI::TerminalCharacter(Character),Dims.Height-1,i);
        //    i++;
        //}
        for(int i = 0; i < m_LineBuffer.size();i++)
        {
            View.WriteCharacters(Dims.Height-1,i,m_LineBuffer[i].GetView());
        }
    }




    void KeyMapper::AddInput(MBCLI::ConsoleInput Input)
    {
        if(m_OutputAvailable)
        {
            throw std::runtime_error("Error adding input to KeyMapper: can only add input after previous result has been popped");
        }
        m_CurrentInput.push_back(std::move(Input));
        m_CurrentLowerBound = std::lower_bound(m_Bindings.begin()+m_CurrentLowerBound,m_Bindings.end(),m_CurrentInput)- m_Bindings.begin();
        if(m_CurrentLowerBound == m_Bindings.size())
        {
            m_OutputAvailable = true;   
        }
        else if(m_Bindings[m_CurrentLowerBound].ReboundCharacters == m_CurrentInput)
        {
            m_OutputAvailable = true;
        }
        else
        {
            //check if the first mismatching character is less than the full size of the current input,
            //if true, no possible match can exist
            if((std::mismatch(m_CurrentInput.begin(),m_CurrentInput.end(),
                        m_Bindings[m_CurrentLowerBound].ReboundCharacters.begin(),m_Bindings[m_CurrentLowerBound].ReboundCharacters.end()).first
                - m_CurrentInput.begin()) != m_CurrentInput.size())
            {
                m_OutputAvailable = true;
                m_CurrentLowerBound = m_Bindings.size();
            }
        }
    }
    bool KeyMapper::OutputAvailable()
    {
        return m_OutputAvailable;
    }
    KeyMapper::RemappingResult KeyMapper::p_GetResult(std::vector<bool>& ActiveBindings)
    {
        KeyMapper::RemappingResult ReturnValue;
        m_OutputAvailable = false;

        if(m_CurrentLowerBound == m_Bindings.size())
        {
            ReturnValue.Events.emplace_back(std::move(m_CurrentInput));
            m_CurrentLowerBound = 0;
            m_CurrentInput.clear();
            return ReturnValue;
        }
        else if(m_Bindings[m_CurrentLowerBound].Recursive == false || std::holds_alternative<ActionType>(m_Bindings[m_CurrentLowerBound].Result))
        {
            auto& Binding = m_Bindings[m_CurrentLowerBound];
            if(std::holds_alternative<ActionType>(Binding.Result))
            {
                ReturnValue.Events.push_back(std::get<ActionType>(Binding.Result));
            }
            else
            {
                ReturnValue.Events.push_back(std::get<std::vector<MBCLI::ConsoleInput>>(Binding.Result));
            }
            m_CurrentLowerBound = 0;
            m_CurrentInput.clear();
            return ReturnValue;
        }
        //no a action type, recursively expand the characters
        auto BindingIndex = m_CurrentLowerBound;
        if(ActiveBindings[BindingIndex])
        {
            throw std::runtime_error("Error expanding mapping: recursive expansion");
        }
        ActiveBindings[BindingIndex] = true;

        m_CurrentLowerBound = 0;
        std::vector<MBCLI::ConsoleInput> StringToExpand;
        std::swap(m_CurrentInput,StringToExpand);
        ReturnValue = p_ExpandString(std::move(StringToExpand),ActiveBindings);

        ActiveBindings[BindingIndex] = false;
        m_CurrentLowerBound = 0;
        return ReturnValue;
           
    }
    KeyMapper::RemappingResult KeyMapper::GetResult()
    {
        std::vector<bool> ActiveBindings(m_Bindings.size(),false);
        return p_GetResult(ActiveBindings);
    }
    KeyMapper::RemappingResult KeyMapper::p_ExpandString(std::vector<MBCLI::ConsoleInput> InputToExpand,std::vector<bool>& ActiveBindings)
    {
        RemappingResult ReturnValue;
        size_t LastExpansionOffset = 0;
        for(size_t i = 0; i < InputToExpand.size();i++)
        {
            AddInput(InputToExpand[i]);
            if(OutputAvailable())
            {
                auto SubOutput = p_GetResult(ActiveBindings);
                for(auto& Event : SubOutput.Events)
                {
                    if(ReturnValue.Events.size() > 0 && (std::holds_alternative<std::vector<MBCLI::ConsoleInput>>(ReturnValue.Events.back()) && 
                                std::holds_alternative<std::vector<MBCLI::ConsoleInput>>(Event)))
                    {
                        auto& CurrentCharacters = std::get<std::vector<MBCLI::ConsoleInput>>(ReturnValue.Events.back());
                        auto& NewCharacters = std::get<std::vector<MBCLI::ConsoleInput>>(Event);
                        CurrentCharacters.insert(CurrentCharacters.end(),std::make_move_iterator(NewCharacters.begin()),std::make_move_iterator(NewCharacters.end()));
                    }
                    else
                    {
                        ReturnValue.Events.push_back(std::move(Event));   
                    }
                }
                LastExpansionOffset = i+1;
            }
        }
        if(LastExpansionOffset != InputToExpand.size())
        {
            if(ReturnValue.Events.size() > 0 && std::holds_alternative<std::vector<MBCLI::ConsoleInput>>(ReturnValue.Events.back()))
            {
                auto& CurrentCharacters = std::get<std::vector<MBCLI::ConsoleInput>>(ReturnValue.Events.back());
                CurrentCharacters.insert(CurrentCharacters.end(),std::make_move_iterator(InputToExpand.begin())+LastExpansionOffset,std::make_move_iterator(InputToExpand.end()));
            }
            else
            {
                ReturnValue.Events.push_back(std::vector(std::make_move_iterator(InputToExpand.begin())+LastExpansionOffset,std::make_move_iterator(InputToExpand.end())));
            }
        }
        m_CurrentInput.clear();
        m_CurrentLowerBound = 0;
        return ReturnValue;
    }
    std::vector<MBCLI::ConsoleInput> KeyMapper::p_StringToConsoleInput(std::string const& StringToConvert)
    {
        std::vector<MBCLI::ConsoleInput> ReturnValue;
        std::vector<MBUnicode::GraphemeCluster> Clusters;
        MBUnicode::GraphemeCluster::ParseGraphemeClusters(Clusters,StringToConvert);
        for(auto& Cluster : Clusters)
        {
            MBCLI::ConsoleInput& NewInput = ReturnValue.emplace_back();
            NewInput.CharacterInput = std::move(Cluster);
        }
        return ReturnValue;
    }
    void KeyMapper::p_InsertBinding(Binding NewBinding)
    {
        auto NewPosition = std::lower_bound(m_Bindings.begin(),m_Bindings.end(),NewBinding);
        if(NewPosition != m_Bindings.end() && NewPosition->ReboundCharacters == NewBinding.ReboundCharacters)
        {
            *NewPosition = std::move(NewBinding);
        }
        else
        {
            m_Bindings.insert(NewPosition,std::move(NewBinding));
        }
    }
    void KeyMapper::AddBinding(std::string const& InputCharacters, std::string const& OutputCharacters,bool Recursive)
    {
        Binding NewBinding;
        NewBinding.Recursive = Recursive;
        NewBinding.ReboundCharacters = p_StringToConsoleInput(InputCharacters);
        NewBinding.Result = p_StringToConsoleInput(OutputCharacters);
        p_InsertBinding(std::move(NewBinding));
    }
    void KeyMapper::AddBinding(std::string const& InputCharacters, ActionType Action)
    {
        Binding NewBinding;
        NewBinding.ReboundCharacters = p_StringToConsoleInput(InputCharacters);
        NewBinding.Result = std::move(Action);
        p_InsertBinding(std::move(NewBinding));
    }
}
