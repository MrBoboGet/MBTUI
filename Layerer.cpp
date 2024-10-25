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
        NewLayer.Window = std::move(Window);
        NewLayer.Window->SetFocus(true);
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
    void  Layerer::WriteBuffer(MBCLI::BufferView View,bool Redraw) 
    {
        //MBCLI::TerminalWindowBuffer ReturnValue(m_Dims.Width,m_Dims.Height);
        if(m_Updated)
        {
            Redraw = true;
            View.Clear();
        }
        m_Updated = false;
        for(auto& Layer : m_Layers)
        {
            //ReturnValue.WriteBuffer(Layer.Window->GetBuffer(),Layer.RowOffset,Layer.ColumnOffset);
            if(Redraw || Layer.Window->Updated())
            {
                Layer.Window->WriteBuffer(View.SubView(Layer.RowOffset,Layer.ColumnOffset),Redraw);
            }
        }
    }
}
