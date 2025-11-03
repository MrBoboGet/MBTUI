#pragma once
#include "MBTUI.h"
#include <optional>

namespace MBTUI
{
    class SpinWindow : public MBCLI::Window
    {
        std::vector<MBCLI::TerminalWindowBuffer> m_Frames;
        std::optional<MBCLI::UpdateNotifier> m_Notifier;
        std::thread m_NotifyThread;

        std::atomic<bool> m_Stopping = false;
        std::condition_variable m_StoppingNotification;
        std::atomic<int> m_CurrentIndex = 0;

        std::chrono::milliseconds m_RefreshDuration = std::chrono::milliseconds(166);

        void p_NotifyThread();
    public:
        SpinWindow();

        //virtual bool HandleInput(MBCLI::ConsoleInput const& Input) override;
        //virtual void SetFocus(bool IsFocused) override;
        //virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions Dims) override
        {
            return MBCLI::Dimensions(21,25);
        }
        ~SpinWindow();
    };
};
