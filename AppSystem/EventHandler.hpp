//
// Created by Max Roschupkin on 08.03.2021.
//

#ifndef PROJECTS_EVENTHANDLER_HPP
#define PROJECTS_EVENTHANDLER_HPP

#include <SFML/Graphics.hpp>
#include <set>
#include <cstdarg>


class EventHandler{
public:
    class Subscriber{
    public:

        explicit Subscriber(EventHandler* newHandler)
        : toClear(newHandler)
        {}

        ~Subscriber(){ toClear->clearSub(this); }
        virtual void notify(const sf::Event::EventType& eventType)=0;

    private:
        EventHandler* toClear;
    };


    explicit EventHandler(sf::Window* setWindow);

    [[nodiscard]] bool happened(const sf::Event::EventType& eventType) const;

    const sf::Event& getEvent(){ return event; }

    /// \warning Use only in notify func to avoid losing keys
    const sf::Keyboard::Key& getCode();

    void pollEvent();

    void waitEvent();

    void Subscribe(Subscriber* newSub,unsigned int num,...);

    void Subscribe(const sf::Event::EventType& listenTo,Subscriber* newSub);


    void UnSubscribe(Subscriber* oldSub,unsigned int num,...);

    void UnSubscribe(const sf::Event::EventType& listenedTo,Subscriber* oldSub);

    void clearSub(Subscriber* deletingSub);




    bool stillPressed(const sf::Keyboard::Key& key);
private:
    sf::Event event;
    sf::Window* window;
    std::multimap<sf::Event::EventType,Subscriber*> subscribers;
    std::map<sf::Event::EventType,bool> events;
    std::map<sf::Keyboard::Key,bool> PressedKeys;
};


#endif //PROJECTS_EVENTHANDLER_HPP
