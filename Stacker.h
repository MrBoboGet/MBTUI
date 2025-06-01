#pragma once

#include <MBCLI/MBCLI.h>
#include <MBCLI/Window.h>
#include <MBUtility/SmartPtr.h>

#include <MBUtility/Iterator.h>

#include "SizeSpecification.h"
namespace MBTUI
{
    class Stacker : public MBCLI::Window
    {
        public:
        enum class Justification
        {
            Start,
            End,
            Center,
            Between,
            Evenly
        };
        private:
        struct FlowRowInfo
        {
            int Size = 0;
            int ElementCount = 0;
        };
        struct SubWindow
        {
            MBUtility::SmartPtr<MBCLI::Window> Window;
            MBCLI::Dimensions Dims;
            MBCLI::Dimensions PreviousDims;
            //kind hacky
            MBCLI::Dimensions Offsets;
            MBCLI::Dimensions PreviousOffsets;

            bool Redraw = false;


            size_t FlowIndex = 0;
            int FlowPosition = -1;
            int OtherFlowPosition = -1;

        };
        SizeSpecification m_SizeSpec;
        Justification m_ContentJustification = Justification::Start;
        bool m_Redraw = false;

        std::vector<SubWindow> m_StackedWindows;

        MBCLI::Dimensions m_LastDrawDims;
        MBCLI::Dimensions m_LastDims;
        MBCLI::Dimensions m_DisplayOffset = MBCLI::Dimensions(0,0);
        MBCLI::Dimensions m_Dims;
        MBCLI::Dimensions m_PreferedDims;

        MBCLI::TerminalColor m_BorderColor = MBCLI::ANSITerminalColor::BrightWhite;
        MBCLI::TerminalColor m_TextColor = MBCLI::ANSITerminalColor::Default;
        MBCLI::TerminalColor m_BGColor = MBCLI::ANSITerminalColor::Default;

        std::vector<std::string> m_InputPassthrough;

        int_least32_t m_SelectedIndex = -1;

        size_t m_FlowIndex = -1;
        int m_FlowWidth = 0;

        bool m_BorderDrawn = false;

        bool m_VerticalFlow = true;
        bool m_Overflow = false;
        bool m_Border = false;

        bool m_Reversed = false;
        bool m_OverflowReversed = false;

        bool m_ClearView = false;
        bool m_AssignDims = false;


        bool m_SubWindowActive = false;

        std::vector<int> m_FlowSizes;

        //MBCLI::ClearCache m_ClearCache;

        void p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw);

        bool p_AssignDimensions();


        template<typename ItType>
        static void p_Justify(ItType begin,ItType end,FlowRowInfo Info,int RowSize,int MBCLI::Dimensions::* FlowMember,Justification JustificationType)
        {
            assert(RowSize >= Info.Size);
            if(JustificationType == Justification::Start)
            {
                //
            }
            else if(JustificationType == Justification::End)
            {
                int SizeDiff = RowSize-Info.Size;
                while(begin != end)
                {
                    SubWindow& Window = *begin;
                    (Window.Offsets.*FlowMember) += SizeDiff;
                    ++begin;
                }
            }
            else if(JustificationType == Justification::Center)
            {
                int SizeDiff = (RowSize-Info.Size)/2;
                while(begin != end)
                {
                    SubWindow& Window = *begin;
                    (Window.Offsets.*FlowMember) += SizeDiff;
                    ++begin;
                }
            }
            else if(JustificationType == Justification::Between)
            {
                if(Info.ElementCount > 1)
                {
                    int ElemCount = Info.ElementCount-1;
                    int SizeDiff = RowSize-Info.Size;
                    int TotalIncrease = 0;
                    ++begin;
                    while(begin != end)
                    {
                        SubWindow& Window = *begin;
                        int Increase = SizeDiff/ElemCount;
                        TotalIncrease += Increase;
                        (Window.Offsets.*FlowMember) += TotalIncrease;
                        SizeDiff -= Increase;
                        ElemCount -= 1;
                        ++begin;
                    }
                }
            }
            else if(JustificationType == Justification::Evenly)
            {
                int ElemCount = Info.ElementCount+1;
                int SizeDiff = RowSize-Info.Size;
                int TotalIncrease = 0;
                while(begin != end)
                {
                    SubWindow& Window = *begin;
                    int Increase = SizeDiff/ElemCount;
                    TotalIncrease += Increase;
                    (Window.Offsets.*FlowMember) += TotalIncrease;
                    SizeDiff -= Increase;
                    ElemCount -= 1;
                    ++begin;
                }
            }
        }

        class ChildIterator : public MBUtility::Bidir_Base<ChildIterator,SubWindow>
                              , public MBUtility::RandomAccess_Base<ChildIterator,std::ptrdiff_t>
        {
            int m_Offset = 0;
            Stacker* m_AssociatedStacker;
            ChildIterator(Stacker& AssociatedStacker)
            {
                m_AssociatedStacker = &AssociatedStacker;
                if(!m_AssociatedStacker->m_Reversed)
                {
                    m_Offset = AssociatedStacker.m_StackedWindows.size()-1;
                }
            }

        public:

            typedef std::random_access_iterator_tag iterator_category;

            static ChildIterator begin(Stacker& Stacker)
            {
                return ChildIterator(Stacker);
            }
            static ChildIterator end(Stacker& Stacker)
            {
                auto ReturnValue = ChildIterator(Stacker);
                if(Stacker.m_Reversed)
                {
                    ReturnValue.m_Offset = Stacker.m_StackedWindows.size();
                }
                else
                {
                    ReturnValue.m_Offset = -1;
                }

                return ReturnValue;
            }
            SubWindow& GetRef()
            {
                return m_AssociatedStacker->m_StackedWindows[m_Offset];
            }
            SubWindow const& GetRef() const
            {
                return m_AssociatedStacker->m_StackedWindows[m_Offset];
            }
            int GetOffset()
            {
                return m_Offset;   
            }
            void Advance(int Increment)
            {
                if(!m_AssociatedStacker->m_Reversed)
                {
                    Increment *= -1;
                }
                m_Offset += Increment;
            }
            std::ptrdiff_t Diff(ChildIterator const& rhs) const
            {
                if(m_AssociatedStacker->m_Reversed)
                {
                    return rhs.m_Offset - m_Offset;
                }
                else
                {
                    return m_Offset-rhs.m_Offset;   
                }
            }
            void Increment()
            {
                Advance(1);
            }
            void Decrement()
            {
                Advance(-1);
            }
            bool IsEqual(ChildIterator const& OtherIterator) const
            {
                return OtherIterator.m_Offset == m_Offset;
            }
        };

        auto begin()
        {
            return ChildIterator::begin(*this);
        }
        auto end()
        {
            return ChildIterator::end(*this);
        }

    public:


        void SetFlowDirection(bool IsVertical);
        void SetFlowWidth(int Size);
        void EnableOverflow(bool OverlowEnabled);
        void SetOverflowDirection(bool Reversed);
        void SetBorder(bool HasBorder);

        void SetBorderColor(MBCLI::TerminalColor Color);

        void SetInputPassthrough(std::vector<std::string> Keys);

        static bool ParseJustification(std::string_view View,Justification& Out);
        void SetJustification(Justification NewJustification);
        void SetTextColor(MBCLI::TerminalColor Color);
        void SetBGColor(MBCLI::TerminalColor Color);

        void SetSizeSpec(SizeSpecification NewSpec)
        {
            m_SizeSpec = NewSpec;   
            SetUpdated(true);
        }
        SizeSpecification GetSizeSpec()
        {
            return m_SizeSpec;   
        }

        auto SelectedWindowIndex() const
        {
            return m_SelectedIndex;
        }
        bool WindowSelected() const
        {
            bool ReturnValue = false;
            if(m_SelectedIndex >= 0 && m_SelectedIndex < m_StackedWindows.size())
            {
                return true;
            }
            return ReturnValue;
        }

        MBUtility::SmartPtr<MBCLI::Window>& GetSelectedWindow()
        {
            if(WindowSelected())
            {
                return m_StackedWindows[m_SelectedIndex].Window;
            }
            throw std::runtime_error("No window selected");
        }
        MBUtility::SmartPtr<MBCLI::Window>& First()
        {
            if(m_StackedWindows.size() != 0)
            {
                return m_StackedWindows[0].Window;
            }
            throw std::runtime_error("Error getting first child: stacker has no children");
        }

        size_t ChildCount() const
        {
            return m_StackedWindows.size();   
        }
        void AddElement(MBUtility::SmartPtr<MBCLI::Window> NewWindow)
        {
            auto& NewSubwindow = m_StackedWindows.emplace_back();
            NewSubwindow.Window = std::move(NewWindow);
            m_AssignDims = true;
            SetChild(*NewSubwindow.Window);
            SetUpdated(true);
        }
        void ClearChildren()
        {
            m_StackedWindows.clear();
            m_ClearView = true;
            m_BorderDrawn = false;
            SetUpdated(true);
        }
        void SetReversed(bool Reversed)
        {
            m_Reversed = true;
            m_ClearView = true;
            m_BorderDrawn = false;
            SetUpdated(true);
        }
        virtual bool HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused)  override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) override;
    };
}
