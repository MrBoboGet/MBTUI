#pragma once

#include <MBCLI/MBCLI.h>
#include <MBCLI/Window.h>
#include <MBUtility/SmartPtr.h>

#include <MBUtility/Iterator.h>
namespace MBTUI
{
    class Stacker : public MBCLI::Window
    {
        bool m_Updated = true;
        struct SubWindow
        {
            MBUtility::SmartPtr<MBCLI::Window> Window;
            MBCLI::Dimensions Dims;
            size_t FlowIndex = 0;
            int FlowPosition = 0;
        };
        std::vector<SubWindow> m_StackedWindows;
        MBCLI::TerminalWindowBuffer m_Buffer;
        MBCLI::Dimensions m_Dims;
        MBCLI::Dimensions m_PreferedDims;

        size_t m_SelectedIndex = -1;
        size_t m_FlowIndex = -1;
        int m_FlowWidth = 0;
        bool m_VerticalFlow = true;
        bool m_Overflow = false;


        bool m_Reversed = false;
        bool m_OverflowReversed = false;

        std::vector<int> m_FlowSizes;


        void p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw);
        void p_AssignDimensions();

        class ChildIterator : public MBUtility::Iterator_Base<ChildIterator,SubWindow>
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
            void Increment()
            {
                if(m_AssociatedStacker->m_Reversed)
                {
                    m_Offset += 1;
                }
                else
                {
                    m_Offset -= 1;   
                }
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


        void AddElement(MBUtility::SmartPtr<MBCLI::Window> NewWindow)
        {
            auto& NewSubwindow = m_StackedWindows.emplace_back();
            NewSubwindow.Window = std::move(NewWindow);
            m_Updated = true;
        }
        void ClearChildren()
        {
            m_StackedWindows.clear();
            m_Updated = true;
        }
        void SetReversed(bool Reversed)
        {
            m_Reversed = true;
            m_Updated = true;
        }
        virtual bool Updated() override;
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused)  override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) override;
    };
}
