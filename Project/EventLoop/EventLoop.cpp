//
// Created by hackerl on 10/12/18.
//

#include "EventLoop.h"
#include "Common/IInstanceManager.h"
#include <glog/logging.h>

CEventLoop::CEventLoop()
{
    m_EventBase = event_base_new();
}

CEventLoop::~CEventLoop()
{
    event_base_free(m_EventBase);
    m_EventBase = nullptr;
}

bool CEventLoop::Add(int fd, ISocketCallback * SocketHandler)
{
    struct Stub
    {
        static void OnEvent(int fd ,short Event, void* arg)
        {
            auto Handler = (ISocketCallback *) arg;

            if (Event & EV_READ)
            {
                Handler->OnRead(fd, EV_READ);
                return;
            }

            if (Event & EV_WRITE)
            {
                Handler->OnWrite(fd, EV_WRITE);
                return;
            }

            if (Event & EV_TIMEOUT)
            {
                LOG(INFO) << "Socket TimeOut Event";

                Handler->OnRead(fd, EV_TIMEOUT);
                Handler->OnWrite(fd, EV_TIMEOUT);
            }
        }
    };

    event* Event = event_new(m_EventBase, fd, EV_READ | EV_CLOSED | EV_PERSIST, Stub::OnEvent , SocketHandler);

    event_add(Event, nullptr);

    EventSession Session = {};

    Session.Event = Event;
    Session.SocketHandler = SocketHandler;

    AutoMutex _0_(m_Mutex);

    m_SocketEventMap.insert(std::pair<int, EventSession>(fd, Session));

    AddRef(SocketHandler);

    return true;
}

bool CEventLoop::SetEvent(int fd, short Mode, time_t TimeOut)
{
    AutoMutex _0_(m_Mutex);

    auto Iterator = m_SocketEventMap.find(fd);

    if(Iterator == m_SocketEventMap.end())
        return false;

    EventSession& Session = Iterator->second;

    event_del(Session.Event);

    Session.Event->ev_events = Mode;

    if (!TimeOut)
    {
        event_add(Session.Event, nullptr);
        return true;
    }

    timeval TimeOutVal = {};

    TimeOutVal.tv_sec = TimeOut;
    TimeOutVal.tv_usec = 0;

    event_add(Session.Event, &TimeOutVal);

    return true;
}

bool CEventLoop::Remove(int fd)
{
    AutoMutex _0_(m_Mutex);

    auto Iterator = m_SocketEventMap.find(fd);

    if(Iterator == m_SocketEventMap.end())
        return false;

    EventSession & Session = Iterator->second;

    event_del(Session.Event);
    event_free(Session.Event);

    Release(Session.SocketHandler);

    m_SocketEventMap.erase(Iterator);

    return true;
}

void CEventLoop::Loop()
{
    event_base_dispatch(m_EventBase);
}

void CEventLoop::Destroy()
{
    AutoMutex _0_(m_Mutex);

    for (auto const& Iterator : m_SocketEventMap)
    {
        const EventSession& Session = Iterator.second;

        event_del(Session.Event);
        event_free(Session.Event);

        Session.SocketHandler->OnClose(Iterator.first, EV_CLOSED);

        Release(Session.SocketHandler);
    }

    m_SocketEventMap.clear();

    event_base_loopbreak(m_EventBase);
}