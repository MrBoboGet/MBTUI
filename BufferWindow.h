#pragma once
#include "MBTUI.h"

namespace MBTUI
{
    class BufferWindow : public MBCLI::Window
    {
        MBCLI::TerminalWindowBuffer m_OriginalBuffer;
    public:
        void SetBuffer(MBCLI::TerminalWindowBuffer Buffer);
        BufferWindow(MBCLI::TerminalWindowBuffer Buffer);
        BufferWindow() = default;

        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused) override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
        virtual MBCLI::Dimensions PreferedDimensions(MBCLI::Dimensions Dims) override;
    };
};
