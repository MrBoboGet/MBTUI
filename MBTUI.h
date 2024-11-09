#pragma once

#include <MBCLI/Window.h>

#include <MBUtility/MOFunction.h>

namespace MBTUI
{
    struct REPL_Line
    {
        std::vector<std::string> Tokens;
        int TokenIndex = -1;
    };
    class REPL : public MBCLI::Window
    {
    public:
        typedef MBUtility::MOFunction<std::vector<std::string>(REPL_Line  const& LineInfo)> CompletionFuncType; 
    private:
        bool m_Updated = true;

        //history stuff
        std::vector<std::string> m_History;
        size_t m_CurrentCommandIndex = -1;

        int m_CursorPosition = 0;
        std::vector<MBUnicode::GraphemeCluster> m_LineBuffer;
        MBCLI::Dimensions m_Dims;
        MBCLI::Dimensions m_MaxDims;

        REPL_Line p_TokenizeString();
        std::string p_GetCompletion(REPL_Line const& Line);

        std::vector<CompletionFuncType> m_CompletionFuncs;
        MBUtility::MOFunction<void(std::string const& Line)> m_EnterFunc;
        void p_SetLine(std::string const& Data);

    public:
        void AddCompletionFunc(CompletionFuncType Func);
        void SetOnEnterFunc(MBUtility::MOFunction<void(std::string const& Line)> EnterFunc);
        std::string GetLineString();
        void Reset();
        std::vector<MBUnicode::GraphemeCluster> GetLineBuffer()
        {
            return m_LineBuffer;   
        }
        void SetMaxDims(MBCLI::Dimensions Dims);
        virtual bool Updated() override;
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused) override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
    };





    class KeyMapper
    {
    public:
        typedef std::shared_ptr<MBUtility::MOFunction<void()>> ActionType;
        struct RemappingResult
        {
            std::vector<std::variant<std::vector<MBCLI::ConsoleInput>,ActionType>> Events;
        };
    private:
        struct Binding
        {
            bool Recursive = true;
            bool Rebindable = false;
            std::vector<MBCLI::ConsoleInput> ReboundCharacters;
            std::variant<ActionType,std::vector<MBCLI::ConsoleInput>> Result;
            bool operator<(Binding const& rhs) const
            {
                return ReboundCharacters < rhs.ReboundCharacters;
            }
            bool operator<(std::vector<MBCLI::ConsoleInput> const& rhs) const
            {
                return ReboundCharacters < rhs;
            }
            bool operator<(MBCLI::ConsoleInput const& Input) const
            {
                if(ReboundCharacters.size() == 0)
                {
                    return false;   
                }
                return ReboundCharacters[0] < Input;
            }
        };

        static constexpr int MAX_EXPAND_DEPTH = 10000;

        std::vector<MBCLI::ConsoleInput> m_CurrentInput;
        std::vector<Binding> m_Bindings;
        size_t m_CurrentLowerBound = 0;
        bool m_OutputAvailable = false;

        //invalid to add bindings while exapding is in progress
        //Either the check for recursive expansions has to be more complicated,
        //the check for recursive expansions removed, or modifying the 
        //binding state has to be explciitly disallowed. The last option 
        //feels like the most appropriate
        bool m_Expanding = false;

        std::vector<MBCLI::ConsoleInput> p_StringToConsoleInput(std::string const& StringToConvert);
        void p_InsertBinding(Binding NewBinding);
        RemappingResult p_GetResult(std::vector<bool>& ActiveBindings);
        RemappingResult p_ExpandString(std::vector<MBCLI::ConsoleInput> InputToExpand,std::vector<bool>& ActiveBindings);
    public:

        void AddInput(MBCLI::ConsoleInput Input);
        bool OutputAvailable();
        RemappingResult GetResult();

        void AddBinding(std::string const& InputCharacters, std::string const& OutputCharacters,bool Recursive = false);
        void AddBinding(std::string const& InputCharacters, ActionType Action);
    };
}
