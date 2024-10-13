#pragma once
#include "MBTUI.h"

namespace MBTUI
{
    class BufferWindow : public MBCLI::Window
    {
        MBCLI::TerminalWindowBuffer m_OriginalBuffer;
        MBCLI::TerminalWindowBuffer m_CroppedBuffer;
        bool m_Updated = false;
    public:
        void SetBuffer(MBCLI::TerminalWindowBuffer Buffer);
        BufferWindow(MBCLI::TerminalWindowBuffer Buffer);
        BufferWindow() = default;

        virtual bool Updated() override;
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetDimensions(MBCLI::Dimensions NewDimensions) override;
        virtual void SetFocus(bool IsFocused) override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual MBCLI::TerminalWindowBuffer GetBuffer() override;
    };
};
