//
// Created by Max Roschupkin on 08.03.2021.
//

#include "EventHandler.hpp"

EventHandler::EventHandler(sf::Window *setWindow)
: event{}
{
    window = setWindow;
}

[[nodiscard]] bool EventHandler::happened(const sf::Event::EventType &eventType)  const{
    try {
        return events.at(eventType);
    } catch (std::exception& dontExist) {
        return false;
    }
}

void EventHandler::pollEvent() {
    for(std::map<sf::Event::EventType,bool>::iterator it=events.begin();it!=events.end();it++)
        it->second=false;

    while (window->pollEvent(event)){
        for(std::multimap<sf::Event::EventType,Subscriber*>::iterator it=subscribers.lower_bound(event.type);
            it!=subscribers.upper_bound(event.type);it++)
            it->second->notify(event.type);

        /*///////////////////////////////////*/
        if(event.type==sf::Event::KeyPressed)
            PressedKeys[event.key.code]=true;
        /*///////////////////////////////////*/

        /*///////////////////////////////////*/
        if(event.type==sf::Event::KeyReleased)
            PressedKeys[event.key.code]=false;
        /*///////////////////////////////////*/


        events[event.type]=true;
    }
}

bool EventHandler::stillPressed(const sf::Keyboard::Key& key) {
    return PressedKeys[key];
}

void EventHandler::waitEvent() {
    for(std::map<sf::Event::EventType,bool>::iterator it=events.begin();it!=events.end();it++)
        it->second=false;
    if(window->waitEvent(event)){
        for(std::multimap<sf::Event::EventType,Subscriber*>::iterator it=subscribers.lower_bound(event.type);
            it!=subscribers.upper_bound(event.type);it++)
            it->second->notify(event.type);

        /*///////////////////////////////////*/
        if(event.type==sf::Event::KeyPressed)
            PressedKeys[event.key.code]=true;
        /*///////////////////////////////////*/

        /*///////////////////////////////////*/
        if(event.type==sf::Event::KeyReleased)
            PressedKeys[event.key.code]=false;
        /*///////////////////////////////////*/

        events[event.type]=true;
    }
}

void EventHandler::UnSubscribe(EventHandler::Subscriber *oldSub, unsigned int num, ...) {
    va_list list;
    va_start(list,num);
    std::set<sf::Event::EventType> subscriptions;
    for(int i=0;i<num;i++)
        subscriptions.emplace((sf::Event::EventType)va_arg(list,int));
    va_end(list);
    for(std::multimap<sf::Event::EventType,Subscriber*>::iterator it=subscribers.begin();
        it!=subscribers.end();)
        if(it->second==oldSub && subscriptions.find(it->first)!=subscriptions.end()) {
            subscriptions.erase(it->first);
            it=subscribers.erase(it);
            if(subscriptions.empty())
                return;
        }else{
            it++;
        }
}

void EventHandler::UnSubscribe(const sf::Event::EventType &listenedTo, EventHandler::Subscriber *oldSub) {
    for(std::multimap<sf::Event::EventType,Subscriber*>::iterator it=subscribers.begin();
        it!=subscribers.end();it++)
        if(it->first==listenedTo && it->second==oldSub)
        {
            subscribers.erase(it);
            return;
        }
}

void EventHandler::clearSub(EventHandler::Subscriber *deletingSub) {
    for(std::multimap<sf::Event::EventType,Subscriber*>::iterator it=subscribers.begin();
        it!=subscribers.end();)
        if(it->second==deletingSub)
            it=subscribers.erase(it);
        else
            it++;
}

void EventHandler::Subscribe(const sf::Event::EventType &listenTo, EventHandler::Subscriber *newSub) {
    subscribers.emplace(listenTo,newSub);
}

void EventHandler::Subscribe(EventHandler::Subscriber *newSub, unsigned int num, ...) {
    va_list list;
    va_start(list,num);
    for(int i=0;i<num;i++)
        subscribers.emplace((sf::Event::EventType)va_arg(list,int),newSub);
    va_end(list);
}

const sf::Keyboard::Key& EventHandler::getCode() {
    return event.key.code;
}

