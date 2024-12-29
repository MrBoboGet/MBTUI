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
        struct SubWindow
        {
            MBUtility::SmartPtr<MBCLI::Window> Window;
            MBCLI::Dimensions Dims;
            MBCLI::Dimensions PreviousDims;
            //kind hacky
            MBCLI::Dimensions Offsets;
            MBCLI::Dimensions PreviousOffsets;

            size_t FlowIndex = 0;
            int FlowPosition = -1;
            int OtherFlowPosition = -1;
        };
        SizeSpecification m_SizeSpec;

        std::vector<SubWindow> m_StackedWindows;
        MBCLI::TerminalWindowBuffer m_Buffer;


        MBCLI::Dimensions m_LastDrawDims;
        MBCLI::Dimensions m_LastDims;
        MBCLI::Dimensions m_Dims;
        MBCLI::Dimensions m_PreferedDims;

        MBCLI::TerminalColor m_BorderColor = MBCLI::ANSITerminalColor::BrightWhite;

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

        std::vector<int> m_FlowSizes;

        //MBCLI::ClearCache m_ClearCache;

        void p_UpdateBuffer(MBCLI::BufferView& View,bool Redraw);

        bool p_AssignDimensions();

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

        void SetSizeSpec(SizeSpecification NewSpec)
        {
            m_SizeSpec = NewSpec;   
            SetUpdated(true);
        }
        SizeSpecification GetSizeSpec()
        {
            return m_SizeSpec;   
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
            SetUpdated(true);
        }
        void SetReversed(bool Reversed)
        {
            m_Reversed = true;
            m_ClearView = true;
            SetUpdated(true);
        }
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused)  override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions SuggestedDimensions) override;
    };
}
