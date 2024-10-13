#include "Layerer.h"
namespace MBTUI
{
    void Layerer::AddLayer(MBUtility::SmartPtr<MBCLI::Window> Window)
    {
        if(m_ActiveLayerIndex < m_Layers.size())
        {
            m_Layers[m_ActiveLayerIndex].Window->SetFocus(false);
        }
        Layer& NewLayer = m_Layers.emplace_back();
        NewLayer.Dims = m_Dims;
        NewLayer.Window = std::move(Window);
        NewLayer.Window->SetFocus(true);
        NewLayer.Window->SetDimensions(m_Dims);
        m_ActiveLayerIndex = m_Layers.size()-1;
        m_Updated = true;
    }
    void Layerer::PopLayer()
    {
        m_Updated = true;
        m_Layers.pop_back();
        m_ActiveLayerIndex = m_Layers.size()-1;
        if(m_ActiveLayerIndex < m_Layers.size())
        {
            m_Layers[m_ActiveLayerIndex].Window->SetFocus(true);   
        }
    }
    bool Layerer::Updated()
    {
        if(m_Updated)
        {
            return m_Updated;
        }
        for(auto& Layer : m_Layers)
        {
            if(Layer.Window->Updated())
            {
                return true;   
            }
        }
        return false;
    }
    void Layerer::HandleInput(MBCLI::ConsoleInput const& Input)
    {
        if(m_ActiveLayerIndex < m_Layers.size())
        {
            m_Layers[m_ActiveLayerIndex].Window->HandleInput(Input);   
        }
    }
    void Layerer::SetDimensions(MBCLI::Dimensions NewDimensions) 
    {
        if(NewDimensions != m_Dims || m_Updated)
        {
            m_Updated = true;
            m_Dims = NewDimensions;
            for(auto& Layer : m_Layers)
            {
                Layer.Window->SetDimensions(NewDimensions);
            }
        }
    }
    void Layerer::SetFocus(bool IsFocused) 
    {
        if(m_ActiveLayerIndex < m_Layers.size())
        {
            m_Layers[m_ActiveLayerIndex].Window->SetFocus(IsFocused);
        }
    }
    MBCLI::CursorInfo Layerer::GetCursorInfo() 
    {
        if(m_ActiveLayerIndex < m_Layers.size())
        {
            return m_Layers[m_ActiveLayerIndex].Window->GetCursorInfo();   
        }
        return MBCLI::CursorInfo();
    }
    MBCLI::TerminalWindowBuffer Layerer::GetBuffer() 
    {
        MBCLI::TerminalWindowBuffer ReturnValue(m_Dims.Width,m_Dims.Height);
        for(auto& Layer : m_Layers)
        {
            ReturnValue.WriteBuffer(Layer.Window->GetBuffer(),Layer.RowOffset,Layer.ColumnOffset);
        }
        return ReturnValue;
    }
}
