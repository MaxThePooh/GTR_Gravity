#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <string>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "EventHandler.hpp"

typedef sf::Vector2f   V2f;
typedef sf::Vector2f&  V2fr;
typedef sf::Vector2f&& V2frr;
typedef const sf::Vector2f   cV2f;
typedef const sf::Vector2f&  cV2fr;
typedef const sf::Vector2f&& cV2frr;

template <class T>
long double hypotenuse(const sf::Vector2<T>& t)
{
    return sqrt(t.x*t.x+t.y*t.y);
}
template <class T>
long double hypotenuse(const T& t,const T& tt)
{
    return sqrt((long double)(t*t+tt*tt));
}
template<class T>
long double hypotenuse(const sf::Vector2<T>& t,const sf::Vector2<T>& tt)
{
    return hypotenuse(t.x-tt.x,t.y-tt.y);
}

template<class T>
bool inCircle(const sf::Vector2<T>& point, const sf::Vector2<T>& pos, const T& radius){  // 19.1, 19.1, 19.1
    int dx = abs(point.x-pos.x);
    if (    dx >  radius ) return false; // \*
    int dy = abs(point.y-pos.y);         // square around a circle
    if (    dy >  radius ) return false; // /*
    if ( dx+dy <= radius ) return true;  // rhombus around the circle
    return ( dx*dx + dy*dy <= radius*radius ); //If nothing happened-Pythagoras
}

template<class T>
bool circleIntersects(const sf::Vector2<T>& Pos1,const double& r1,const sf::Vector2<T>& Pos2,const double& r2)
{
    return hypotenuse(Pos1.x-Pos2.x,Pos1.y-Pos2.y)<r1+r2;
}
template<class T>
std::basic_string<T>& remove_zeros(std::basic_string<T>& str)
{
    if(!str.empty()) {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](const char& c) {
            return c != '0';
        }).base(), str.end());
        if (str.back() == '.')
            str.pop_back();


        typename std::basic_string<T>::iterator begin = str.front() == T('-') ? str.begin() + 1 : str.begin();
        typename std::basic_string<T>::iterator end=std::find_if(begin,str.end(),
                                                                 [](const T& c){ return std::isdigit(c) || c==T('.') && c!=T('0'); });
        if(*end==T('.') && std::distance(str.begin(),end)>0)
            --end;
        str.erase(begin,end);
    }
    return str;
}
template<class T>
std::basic_string<T>& remove_zeros(std::basic_string<T>&& str)
{
    if(!str.empty()) {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](const char& c) {
            return c != '0';
        }).base(), str.end());
        if (str.back() == '.')
            str.pop_back();


        typename std::basic_string<T>::iterator begin = str.front() == T('-') ? str.begin() + 1 : str.begin();
        typename std::basic_string<T>::iterator end=std::find_if(begin,str.end(),
                                                                 [](const T& c){ return std::isdigit(c) || c==T('.') && c!=T('0'); });
        if(*end==T('.') && std::distance(str.begin(),end)>0)
            --end;
        str.erase(begin,end);
    }
    return str;
}


template<class T>
T round_to(const T& val, const int& N)
{
    double NPowered= pow(10,N);
    return trunc(val*NPowered)/NPowered;
}


namespace YAML {
    template<>
    struct convert<sf::Color>{
        static Node encode(const sf::Color& rhs) {
            Node node;
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }

        static bool decode(const Node& node, sf::Color& rhs) {
            if(!node.IsSequence() || node.size() != 4) {
                return false;
            }

            rhs.r = node[0].as<uint8_t>();
            rhs.g = node[1].as<uint8_t>();
            rhs.b = node[2].as<uint8_t>();
            rhs.a = node[3].as<uint8_t>();

            return true;
        }
    };
}

template <class T>
T translate(const T& value,const T& Min, const T& Max, const T& toMin, const T& toMax) {
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}

double translate(const double& value,const double& Min, const double& Max, const double& toMin, const double& toMax) {
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}

template<class T>
T enclose(const T& x,const T& min,const T& max)
{
    return x<min?min:x>max?max:x;
}

template<class T>
sf::Vector2<T> enclose(const sf::Vector2<T>& val, const sf::Vector2<T>& min, const sf::Vector2<T>& max)
{
    return {enclose(val.x,min.x,max.x), enclose(val.y,min.y,max.y)};
}

template<class T>
T loopEnclose(const T& x,const T& min,const T& max)
{
    return x>max? x-max*floor(x/max) : x<min ? max*ceil(abs(x/max))-abs(x) : x;
}

template<class T>
sf::Vector2<T> loopEnclose(const sf::Vector2<T>& val, const sf::Vector2<T>& min, const sf::Vector2<T>& max)
{
    return {loopEnclose(val.x,min.x,max.x), loopEnclose(val.y,min.y,max.y)};
}


template<class T>
sf::Vector2<T> operator/(const sf::Rect<T>& rect,const double& t)
{
    return sf::Vector2<T>(rect.width/t,rect.height/t);
}

template<class T>
sf::Vector2<T> operator/(const sf::Vector2<T>& vec,const double& t)
{
    return sf::Vector2<T>(vec.x/t,vec.y/t);
}
template<class T>
sf::Vector2<T> operator*(const sf::Vector2<T>& vec,const double& t)
{
    return sf::Vector2<T>(vec.x*t,vec.y*t);
}
template<class T>
sf::Vector2<T> operator+(const sf::Vector2<T>& vec,const double& t)
{
    return sf::Vector2<T>(vec.x+t,vec.y+t);
}
template<class T>
sf::Vector2<T> operator-(const sf::Vector2<T>& vec,const double& t)
{
    return sf::Vector2<T>(vec.x-t,vec.y-t);
}

long double G(const long double& x,const long double& d)
{
    return x/(d*d);
}

namespace {
    void de_casteljau(cV2fr begin, cV2fr end, cV2fr clarifying,sf::VertexArray& array, const float& step) {
        int stepCount=(int)(1.f/step);
        for (int i = 0; i <= stepCount; ++i) {
            V2f first=clarifying+(begin-clarifying)*(1.f-(float)i*step);
            V2f second=clarifying+(end-clarifying)*((float)i*step);
            array.append({{second+(first-second)*(1.f-(float)i*step)},sf::Color::Black});
        }
    }
}

class App: public EventHandler::Subscriber {
    typedef float& flr;
    typedef const float cfl;
    typedef const float& cflr;

    typedef const int cint;
    typedef const int& cintr;

    typedef const long int clint;
    typedef const long int& clintr;

    typedef const long long int cllint;
    typedef const long long int& cllintr;

    typedef const unsigned int cuint;
    typedef const unsigned int& cuintr;

    typedef const std::string cstring;
    typedef const std::string& cstringr;
    typedef const std::string&& cstringrr;


    class MassObj {
    public:
        MassObj(cllintr _mass, cV2fr moveSpeed, cV2fr position)
                : mass(_mass)
                , pos(position)
                , moveSpeed(moveSpeed) {

        }

        [[nodiscard]] long long int getMass() const {
            return mass;
        }

        [[nodiscard]] const sf::Vector2f& getPos() const {
            return pos;
        }

        [[nodiscard]] V2f getMoveSpeed() const {
            return moveSpeed;
        }

        void setMass(cllintr _mass) {
            MassObj::mass = _mass;
        }

        template<class T>
        void setPos(const sf::Vector2<T>& position) {
            MassObj::pos = sf::Vector2f(position);
        }

        void setMoveSpeed(cV2fr _moveSpeed) {
            MassObj::moveSpeed = _moveSpeed;
        }


        void setXMoveSpeed(const float& xsp) {
            moveSpeed.x = xsp;
        }

        void setYMoveSpeed(const float& ysp) {
            moveSpeed.y = ysp;
        }

        bool operator==(const App::MassObj& obj)
        {
            return this==&obj;
        }
    private:
        V2f moveSpeed;
        long long int mass;
        sf::Vector2f pos;

    };

    struct Point {
    public:
        Point() = default;

        explicit Point(cV2fr fixedPosition)
                : pos(fixedPosition)
                , fixedPos(fixedPosition) {}

        [[nodiscard]] sf::Vector2f getPos() const {
            return pos;
        }

        [[nodiscard]] sf::Vector2f getFPos() const {
            return pos;
        }

        void setPos(cV2fr position) {
            Point::pos = position;
        }

        void setFixedPos(cV2fr fixedPosition) {
            Point::fixedPos = fixedPosition;
        }

        void fix() {
            pos = fixedPos;
        }

        Point& operator=(const Point& val) = default;

    private:
        sf::Vector2f pos;
        sf::Vector2f fixedPos;
    };

public:
    App(cflr windowSize, cstringr title, cuintr frameLimit, const sf::ContextSettings& settings,
        cuintr pointCount, const float& DP, cflr _offset, const sf::Font& font)
            : window(new sf::RenderWindow(sf::VideoMode((uint) windowSize, (uint) windowSize), title,
                                          sf::Style::Close, settings))
            , event(window)
            , Subscriber(&event)
            , WSize(windowSize)
            , PCount(pointCount)
            , MPCount(pointCount - 1)
            , Dp(DP)
            , DPWSize(windowSize / DP)
            , PixelsInDp(windowSize / (windowSize / Dp))
            , offset(_offset)
            , relativeCanvasSize(1 - 2 * _offset)
            , canvasSize(DPWSize * relativeCanvasSize)
            , mainFont(font)
            , unitValue(canvasSize / (float) (PCount - 1))
            , showPoints(true)
            , pickedObj(nullptr)
            , pickedOnThisFrame(false)
            , pointRadius(ceil(canvasSize / 12.5f / PCount))
            , objectRadius(ceil(canvasSize / 5.f / PCount))
            , visual(YAML::LoadFile("visual.yaml")) {
        window->setFramerateLimit(frameLimit);

        points = new Point* [PCount];

        for (int i = 0; i < PCount; ++i) {

            points[i] = new Point[PCount];

            for (int j = 0; j < PCount; ++j) {

                points[i][j] = Point(V2f(unitValue * i, unitValue * j) + DPWSize * offset);
            }
        }

        VerticalMiddles = new Point* [MPCount];

        for (int i = 0; i < MPCount; ++i) {
            VerticalMiddles[i] = new Point[PCount];

            for (int j = 0; j < PCount; ++j) {
                Point& point = points[i][j];
                Point& pointAfter = points[i + 1][j];
                VerticalMiddles[i][j] = Point((point.getFPos() - pointAfter.getFPos()) * 0.5 + pointAfter.getFPos());
            }
        }

        HorizontalMiddles = new Point* [PCount];

        for (int i = 0; i < PCount; ++i) {
            HorizontalMiddles[i] = new Point[MPCount];

            for (int j = 0; j < MPCount; ++j) {
                Point& point = points[i][j];
                Point& pointAfter = points[i][j + 1];
                HorizontalMiddles[i][j] = Point((point.getFPos() - pointAfter.getFPos()) * 0.5 + pointAfter.getFPos());
            }
        }

        event.SubscribeAll(this);

        info["massInfo"].setFont(mainFont);
        info["XmvspInfo"].setFont(mainFont);
        info["YmvspInfo"].setFont(mainFont);

        info["massInfo"].setFillColor(sf::Color::Black);
        info["XmvspInfo"].setFillColor(sf::Color::Black);
        info["YmvspInfo"].setFillColor(sf::Color::Black);

        info["massInfo"].setString(visual["defaultStrings"]["mass"].as<std::string>());
        info["XmvspInfo"].setString(visual["defaultStrings"]["xmovesp"].as<std::string>());
        info["YmvspInfo"].setString(visual["defaultStrings"]["ymovesp"].as<std::string>());

        float DPoffset = offset * DPWSize;
        info["massInfo"].setCharacterSize(pix(DPoffset * canvasSize / 3200.f));
        info["XmvspInfo"].setCharacterSize(pix(DPoffset * canvasSize / 3200.f));
        info["YmvspInfo"].setCharacterSize(pix(DPoffset * canvasSize / 3200.f));

        info["massInfo"].setPosition(pix(V2f(DPoffset, DPWSize - DPoffset)));
        info["XmvspInfo"].setPosition(pix(V2f(DPoffset, DPWSize - DPoffset + dp(info["massInfo"].getCharacterSize()))));
        info["YmvspInfo"].setPosition(
                V2f(info["XmvspInfo"].getPosition().x + info["XmvspInfo"].getGlobalBounds().width +
                    info["XmvspInfo"].getCharacterSize(),
                    info["XmvspInfo"].getPosition().y));
    }

    void notify(const sf::Event::EventType& eventType) override {
        const sf::Event& Event = event.getEvent();
        switch (eventType) {
            default:
                break;
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::LostFocus:
                break;
            case sf::Event::GainedFocus:
                break;
            case sf::Event::TextEntered:
                handleText(wchar_t(event.getEvent().text.unicode));
                break;
            case sf::Event::KeyPressed:
                switch (event.getCode()) {
                    case sf::Keyboard::P:
                        showPoints = !showPoints;
                        break;
                    case sf::Keyboard::N:
                        obj.push_back(MassObj(0LL, {0.f, 0.f},
                                              pixasPointPos(V2f(abs(sf::Mouse::getPosition(*window).x),
                                                                abs(sf::Mouse::getPosition(*window).y)))));
                        break;
                    case sf::Keyboard::D:
                    case sf::Keyboard::Delete:
                        if (pickedObj != nullptr) {
                            obj.erase(std::find(obj.begin(), obj.end(), *pickedObj));
                            pickedObj = nullptr;
                        }
                        break;
                    case sf::Keyboard::H:
                        if (!obj.empty()) {
                            if (pickedObj == nullptr) {
                                pickedObj = &*obj.begin();
                            } else {
                                std::vector<MassObj>::iterator it = std::find(obj.begin(), obj.end(),
                                                                              *pickedObj);
                                pickedObj = &*(it + 1 == obj.end() || it==obj.end()? obj.begin() : it + 1);
                            }
                        }
                        pickedObjectChanged();
                        break;
                }
                break;
            case sf::Event::KeyReleased:
                break;
            case sf::Event::MouseWheelMoved:
                break;
            case sf::Event::MouseWheelScrolled:
                break;
            case sf::Event::MouseButtonPressed:
                switch (Event.mouseButton.button) {
                    case sf::Mouse::Left:
                        leftClick();
                        break;
                    case sf::Mouse::Right:
                        break;
                    case sf::Mouse::Middle:
                        break;
                }
                break;
            case sf::Event::MouseButtonReleased:
                switch (Event.mouseButton.button) {
                    case sf::Mouse::Left:
                        leftUnclick();
                        break;
                    case sf::Mouse::Right:
                        break;
                    case sf::Mouse::Middle:
                        break;
                }
                break;
            case sf::Event::MouseMoved:
                break;
            case sf::Event::MouseEntered:
                break;
            case sf::Event::MouseLeft:
                break;
        }
    }


    ~App() {
        for (int i = 0; i < PCount; ++i) {
            delete[] points[i];
        }
        delete[] points;

        for (int i = 0; i < MPCount; ++i) {
            delete[] VerticalMiddles[i];
            delete[] HorizontalMiddles[i];
        }
        delete[] VerticalMiddles;
        delete[] HorizontalMiddles;

        pickedObj = nullptr;

        delete window;
    }

    int processing() {
        while (window->isOpen()) {

            event.pollEvent();

            update();
            updateAttraction(1.f);
            render();
        }
        return 0;
    }

private:
    //DP//
    float Dp;
    float PixelsInDp;

    //Window//
    sf::RenderWindow* window;
    EventHandler event;
    Point** points;

    //Points//
    uint PCount;
    Point** VerticalMiddles;
    Point** HorizontalMiddles;
    uint MPCount;

    //Config//
    float WSize;
    float DPWSize;
    float offset;
    float relativeCanvasSize;
    float canvasSize;
    float unitValue;
    YAML::Node visual;

    std::vector<MassObj> obj;

    sf::Font mainFont;

    //Inner variables//
    bool showPoints;
    bool pickedOnThisFrame;
    MassObj* pickedObj;
    sf::Text* pickedText;

    //Interface//
    std::map<std::string, sf::Text> info; ///< massInfo XmvspInfo YmvspInfo

    //Drawing variables//
    float pointRadius;
    float objectRadius;


    sf::Vector2f pixasPointPos(cV2fr regPos) const{
        return (regPos-WSize*offset)/pix(unitValue);
    }
    sf::Vector2f dpasPointPos(cV2fr regPos) const{
        return (regPos-DPWSize*offset)/unitValue;
    }
    sf::Vector2f asRegPos(cV2fr ptPos) const{
        int flx=floor(ptPos.x);
        int fly=floor(ptPos.y);
        int cx =ceil(ptPos.x);
        int cy =ceil(ptPos.y);

        V2f leftupP   =pix(points[flx]  [fly].getPos());
        V2f rightupP  =pix(points [cx]  [fly].getPos());
        V2f leftdownP =pix(points[flx]  [cy].getPos());
        V2f rightdownP=pix(points [cx]  [cy].getPos());

        float xf=ptPos.x-trunc(ptPos.x);
        float yf=ptPos.y- trunc(ptPos.y);

        V2f leftSide=(leftdownP-leftupP);
        V2f rightSide=(rightdownP-rightupP);
        V2f downSide=(rightdownP-leftdownP);
        V2f upSide=(rightupP-leftupP);

        float movingx= ((rightSide.x*yf -leftSide.x*yf+upSide.x)*xf)+leftSide.x*yf;
        float movingy= ((downSide.y*xf -upSide.y*xf+leftSide.y)*yf)+upSide.y*xf;
        return {leftupP.x+movingx,leftupP.y+movingy};

    }
    sf::Vector2f asRegPosDP(cV2fr ptPos) const{
        return dp(asRegPos(ptPos));
    }
    sf::Vector2f asRegFPos(cV2fr ptPos) const{
        return ptPos*pix(unitValue)+WSize*offset;
    }
    sf::Vector2f asRegFPosDP(cV2fr ptPos) const{
        return ptPos*unitValue+DPWSize*offset;
    }

    void move_according_to_G(Point& point,const MassObj& object,cflr factor) const
    {
        const V2f relativeVec= asRegFPosDP(object.getPos())-point.getPos();
        const long double d=hypotenuse(relativeVec.x,relativeVec.y);
        const long double attractionPower= enclose(G(object.getMass(),d)/PCount*10.f * relativeCanvasSize,0.L,1.L);
        if(attractionPower>0.001L)
            point.setPos(point.getPos()+relativeVec*attractionPower*factor);
    }

    void updateAttraction(cflr factor)
    {
        for (int i = 0; i < PCount; ++i)
            for (int j = 0; j < PCount; ++j)
                points[i][j].fix();
        for (int i = 0; i < MPCount; ++i)
            for (int j = 0; j < PCount; ++j)
                VerticalMiddles[i][j].fix();
        for (int i = 0; i < PCount; ++i)
            for (int j = 0; j < MPCount; ++j)
                HorizontalMiddles[i][j].fix();
        for(auto object:obj)
        {
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& point=points[i][j];

                    move_according_to_G(point,object,factor);
                }
            }

            for (int i = 0; i < MPCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& middle=VerticalMiddles[i][j];


                    move_according_to_G(middle,object,factor);
                }
            }
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < MPCount; ++j) {
                    Point& middle=HorizontalMiddles[i][j];

                    move_according_to_G(middle,object,factor);
                }
            }
        }
    }

    void handleText(const wchar_t& c) {
            if(std::isdigit(c) || c==L'.' || c==L'\b' || c==L'-') {
                if(pickedText==&info["massInfo"]){
                    std::wstring str=pickedText->getString().toWideString();
                    std::wstring vstr(std::find_if(str.begin(),str.end(),[](const wchar_t& c){ return (bool)std::isdigit(c) || c==L'-'; }),str.end());
                    str.erase(std::find_if(str.begin(),str.end(),[](const wchar_t& c){ return (bool)std::isdigit(c); }),str.end());
                    vstr+=c;
                    if(c==L'\b')
                    {
                        vstr.pop_back();
                        if(!vstr.empty())
                            vstr.pop_back();
                    }
                    else if(c==L'0')
                    {
                        if(vstr.size()==2 && vstr.front()==L'0')
                            vstr.pop_back();
                    }
                    else if(c==L'.'){
                        vstr.pop_back();
                    }
                    else if(c==L'-'){
                        vstr.pop_back();
                    }
                    else{
                        if(vstr.size()==2 && vstr.front()==L'0')
                            vstr.erase(vstr.begin());
                    }
                    if(vstr.empty())
                        pickedObj->setMass(0LL);
                    else
                        pickedObj->setMass(std::stoll(vstr));
                    pickedText->setString(str+vstr);

                }
                else if(pickedText==&info["XmvspInfo"]){
                    std::wstring str=pickedText->getString().toWideString();
                    std::wstring::iterator firstDigit=std::find_if(str.begin(),str.end(),[](const wchar_t& c){ return (bool)std::isdigit(c) || c==L'-'; });
                    std::wstring vstr(firstDigit,str.end());
                    str.erase(firstDigit,str.end());
                    vstr+=c;
                    if(c==L'\b')
                    {
                        vstr.pop_back();
                        if(!vstr.empty())
                            vstr.pop_back();
                    }
                    else if(c==L'0')
                    {
                        if(vstr.size()==2 && vstr.front()==L'0')
                            vstr.pop_back();
                    }
                    else if(c==L'-'){
                        if(vstr.size()>1)
                            vstr.pop_back();
                    }
                    else if(c==L'.'){
                        if(vstr.size()==1)
                            vstr.pop_back();
                        else if(*(vstr.end()-2)==L'-')
                            vstr.pop_back();
                        else if(std::find_if(vstr.begin(),vstr.end(),[](const wchar_t& c){ return c==L'.';})!=vstr.end()-1)
                            vstr.pop_back();
                    }
//                    remove_zeros(vstr);
                    if(vstr.front()==L'.')
                        vstr.erase(vstr.begin());
                    if(vstr.empty() || vstr.size()==1 && vstr.front()==L'-')
                        pickedObj->setXMoveSpeed(0.f);
                    else
                        pickedObj->setXMoveSpeed(std::stof(vstr));
                    pickedText->setString(str+vstr);
                }
                else if(pickedText==&info["YmvspInfo"]){
                    std::wstring str=pickedText->getString().toWideString();
                    std::wstring::iterator firstDigit=std::find_if(str.begin(),str.end(),[](const wchar_t& c){ return (bool)std::isdigit(c) || c==L'-'; });
                    std::wstring vstr(firstDigit,str.end());
                    str.erase(firstDigit,str.end());
                    vstr+=c;
                    if(c==L'\b')
                    {
                        vstr.pop_back();
                        if(!vstr.empty())
                            vstr.pop_back();
                    }
                    else if(c==L'0')
                    {
                        if(vstr.size()==2 && vstr.front()==L'0')
                            vstr.pop_back();
                    }
                    else if(c==L'-'){
                        if(vstr.size()>1)
                            vstr.pop_back();
                    }
                    else if(c==L'.'){
                        if(vstr.size()==1)
                            vstr.pop_back();
                        else if(*(vstr.end()-2)==L'-')
                            vstr.pop_back();
                        else if(std::find_if(vstr.begin(),vstr.end(),[](const wchar_t& c){ return c==L'.';})!=vstr.end()-1)
                            vstr.pop_back();
                    }
//                    remove_zeros(vstr);
                    if(vstr.front()==L'.')
                        vstr.erase(vstr.begin());
                    if(vstr.empty() || vstr.size()==1 && vstr.front()==L'-')
                        pickedObj->setYMoveSpeed(0.f);
                    else
                        pickedObj->setYMoveSpeed(std::stof(vstr));
                    pickedText->setString(str+vstr);
                }
                info["YmvspInfo"].setPosition(
                        V2f(info["XmvspInfo"].getPosition().x + info["XmvspInfo"].getGlobalBounds().width +
                            info["XmvspInfo"].getCharacterSize(),
                            info["XmvspInfo"].getPosition().y));
            }
    }

    void pickedObjectChanged()
    {
        if(pickedObj!= nullptr) {
            info["massInfo"].setString(visual["defaultStrings"]["mass"].as<std::string>() +
                                       remove_zeros(std::to_string(pickedObj->getMass())));
            info["XmvspInfo"].setString(visual["defaultStrings"]["xmovesp"].as<std::string>() +
                                        remove_zeros(std::to_string(pickedObj->getMoveSpeed().x)));
            info["YmvspInfo"].setString(visual["defaultStrings"]["ymovesp"].as<std::string>() +
                                        remove_zeros(std::to_string(pickedObj->getMoveSpeed().y)));
            info["YmvspInfo"].setPosition(
                    V2f(info["XmvspInfo"].getPosition().x + info["XmvspInfo"].getGlobalBounds().width +
                        info["XmvspInfo"].getCharacterSize(),
                        info["XmvspInfo"].getPosition().y));
        }
    }

    void leftClick()
    {
        V2f mousePos(sf::Mouse::getPosition(*window));
        pickedOnThisFrame= false;
        for(MassObj& object:obj) {
            if(inCircle(V2f (mousePos), asRegPos(object.getPos()),objectRadius)) {
                pickedObj = &object;
                pickedObjectChanged();
                pickedOnThisFrame=true;
                return;
            }
        }
        for(std::map<std::string,sf::Text>::iterator it=info.begin();it!=info.end();++it){
            sf::Text& text=it->second;
            if(text.getGlobalBounds().contains(V2f(mousePos))) {
                pickedText = &text;
                return;
            }
        }
        pickedObj= nullptr;
        pickedText= nullptr;
    }
    void update()
    {
        if(pickedObj== nullptr)
            pickedText== nullptr;
            for(MassObj& object:obj)
            {
                object.setPos(object.getPos()+object.getMoveSpeed());
                object.setPos(loopEnclose(object.getPos(),{0.f,0.f},V2f(PCount-1,PCount-1)));
            }
    }
    void leftUnclick()
    {
    }

    void render()
    {
        window->clear(sf::Color::White);


        for (int i = 0; i < PCount; ++i) {
            //Drawing the axles
            const float DPoffset=DPWSize*offset;

            sf::Text coordinates(std::to_string(i),mainFont,pix(unitValue/3.f));
            coordinates.setFillColor(sf::Color::Black);
            coordinates.setOrigin(coordinates.getGlobalBounds()/2.f);

            const float shift=DPoffset-canvasSize/32.f;

            coordinates.setPosition(pix(sf::Vector2f(unitValue*i+DPoffset,shift)));
            window->draw(coordinates);

            coordinates.setPosition(pix(sf::Vector2f(shift,unitValue*i+DPoffset)));
            window->draw(coordinates);

            //Drawing points//
            if(showPoints) {
                sf::CircleShape circle(pointRadius);
                circle.setOrigin(circle.getGlobalBounds() / 2);
                circle.setFillColor(sf::Color::Black);
                for (int j = 0; j < PCount; ++j) {
                    const Point& point = points[i][j];
                    circle.setPosition(pix(point.getPos()));

                    window->draw(circle);
                }
            }
        }

        //Drawing vertical middle points//
        for (int i = 0; i < MPCount; ++i) {
            for (int j = 0; j < PCount; ++j) {
                sf::VertexArray array(sf::PrimitiveType::LinesStrip);
                Point& point = points[i][j];
                Point& pointAfter = points[i+1][j];
                Point& middle=VerticalMiddles[i][j];

                de_casteljau(pix(point.getPos()), pix(pointAfter.getPos()),
                             pix(middle.getPos()), array,0.1f);

                window->draw(array);
            }
        }
        //Drawing horizontal middle points//
        for (int i = 0; i < PCount; ++i) {
            for (int j = 0; j < MPCount; ++j) {
                sf::VertexArray array(sf::PrimitiveType::LinesStrip);
                Point& point = points[i][j];
                Point& pointAfter = points[i][j+1];
                Point& middle=HorizontalMiddles[i][j];

                de_casteljau(pix(point.getPos()), pix(pointAfter.getPos()),
                             pix(middle.getPos()), array,0.1f);

                window->draw(array);
            }
        }

        //Drawing objects with mass//
        sf::CircleShape circle(objectRadius);
        circle.setFillColor(visual["objectColor"].as<sf::Color>());
        circle.setOrigin(circle.getGlobalBounds()/2);
        for (const MassObj& object:obj) {
            if(&object==pickedObj)
                circle.setFillColor(visual["pickedObjectColor"].as<sf::Color>());
            else
                circle.setFillColor(visual["objectColor"].as<sf::Color>());
            circle.setPosition(asRegPos(object.getPos()));

            window->draw(circle);
        }

        //Drawing object info//
        if(pickedObj!= nullptr)
        {
            window->draw(info["massInfo"]);
            window->draw(info["XmvspInfo"]);
            window->draw(info["YmvspInfo"]);
            if(pickedText!= nullptr)
            {
                const sf::FloatRect& bounds=pickedText->getGlobalBounds();
                sf::RectangleShape rect(V2f(bounds.width,bounds.height));
                rect.setFillColor(sf::Color::Transparent);
                rect.setPosition(bounds.left,bounds.top);
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color::Red);
                window->draw(rect);
            }
        }

        window->display();
    }

    [[nodiscard]] double dp(const double& pix) const
    {
        return pix/Dp;
    }

    [[nodiscard]] double pix(const double& dp) const{
        return dp * PixelsInDp;
    }

    template<class T>
    [[nodiscard]] sf::Vector2<T> dp(const sf::Vector2<T>& pix) const
    {
        return pix/Dp;
    }

    template<class T>
    [[nodiscard]] sf::Vector2<T> pix(const sf::Vector2<T>& dp) const
    {
        return dp*PixelsInDp;
    }
};

int main(int args,char** argv) {
#ifdef TARGET_OS_MAC
    std::string path=argv[0];
    while(path.back()!='/')
        path.pop_back();
    path.pop_back();
    std::cout<<path<<std::endl;

    std::filesystem::current_path(path);
#endif


    YAML::Node config=YAML::LoadFile("config.yaml");

    sf::VideoMode fullScreen=sf::VideoMode::getFullscreenModes()[0];
    std::vector<sf::VideoMode> m=sf::VideoMode::getFullscreenModes();

    const float maxWindowSize=fullScreen.width<fullScreen.height?fullScreen.width:fullScreen.height;


    float WindowSize=maxWindowSize;
    if(!config["MwindowSize"].IsNull())
        WindowSize=config["MwindowSize"].as<float>();
    else if(config["windowSize"].as<std::string>()=="halfAvailable")
    {
        WindowSize=maxWindowSize/2;
    }
    else if(config["windowSize"].as<std::string>()=="partAvailable")
    {
        WindowSize=maxWindowSize/1.5f;
    }
    else if(config["windowSize"].as<std::string>()=="fullAvailable")
    {
        WindowSize=maxWindowSize;
    }

    float dp=WindowSize/config["dpInWindow"].as<float>();

    std::cout<<"Window size: "<<WindowSize<<std::endl<<"Independent pixel: "<<dp<<std::endl;

    unsigned int frameLimit=30;
    if(!config["frameLimit"].IsNull())
        frameLimit=config["frameLimit"].as<unsigned int>();


    sf::ContextSettings settings;
    settings.antialiasingLevel=config["antialiasingLevel"].as<unsigned int>();

    unsigned int pointCount=10;
    if(!config["pointCount"].IsNull())
        pointCount=config["pointCount"].as<unsigned int>();

    const float offset=enclose(config["offset"].as<float>(),0.1f,0.4f);

    sf::Font font;
    font.loadFromFile(config["fontFile"].as<std::string>());

    App app(WindowSize,"Gravity",frameLimit,settings,pointCount,dp,offset,font);
    app.processing();
    return 0;
}
