#include "Spin.h"
#include <MBCLI/MBSpinData.h>
namespace MBTUI
{
       
    SpinWindow::SpinWindow()
    {
        MBUtility::MBBufferInputStream GoatInput(SpinGoatData,SpinGoatData_size);
        m_Frames = MBCLI::ReadRawRGBABufferInfo(GoatInput,8,21,25);
    }
    void SpinWindow::WriteBuffer(MBCLI::BufferView View,bool Redraw) 
    {
        if(!m_Notifier.has_value())
        {
            m_Notifier = View.GetNotifier();
            m_NotifyThread = std::thread(&SpinWindow::p_NotifyThread,this);
        }
        SetUpdated(false);
        View.WriteBuffer(0,0,m_Frames[m_CurrentIndex.load()]);
    }
    void SpinWindow::p_NotifyThread()
    {
        std::mutex ThreadMutex;
        std::unique_lock ThreadLock(ThreadMutex);
        while(!m_Stopping.load())
        {
            m_StoppingNotification.wait_for(ThreadLock,m_RefreshDuration);
            if(!m_Stopping.load())
            {
                m_Notifier->Update(*this);
            }
            m_CurrentIndex.store( (m_CurrentIndex.load()+1)%m_Frames.size());
        }
    }
    SpinWindow::~SpinWindow()
    {
        m_Stopping.store(false);
        m_StoppingNotification.notify_one();
        m_NotifyThread.join();
    }
}
