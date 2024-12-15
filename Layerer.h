#pragma once
#include <MBCLI/Window.h>

namespace MBTUI
{
    class Layerer : public MBCLI::Window
    {
        struct Layer
        {
            int RowOffset = 0;
            int ColumnOffset = 0;
            MBCLI::Dimensions Dims;
            MBUtility::SmartPtr<MBCLI::Window> Window;
        };

        std::vector<Layer> m_Layers;
        MBCLI::Dimensions m_Dims;
        size_t m_ActiveLayerIndex = 0;
    public:

        void AddLayer(MBUtility::SmartPtr<MBCLI::Window> Window);
        
        template<typename T>
        std::enable_if_t<std::is_base_of_v<MBCLI::Window,T>,void> AddLayer(T Window)
        {
            std::unique_ptr<MBCLI::Window> Ptr = std::make_unique<T>(std::move(Window));
            MBUtility::SmartPtr<MBCLI::Window> Container = MBUtility::SmartPtr<MBCLI::Window>(std::move(Ptr));
            AddLayer(std::move(Container));
        }


        void PopLayer();
        virtual void HandleInput(MBCLI::ConsoleInput const& Input) override;
        virtual void SetFocus(bool IsFocused) override;
        virtual MBCLI::CursorInfo GetCursorInfo() override;
        virtual void WriteBuffer(MBCLI::BufferView View,bool Redraw) override;
    };
}
