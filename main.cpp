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

std::string& remove_zeros(std::string& str)
{
    str.erase(std::find_if(str.rbegin(),str.rend(),[](const char& c){
       return c!='0';
    }).base(),str.end());
    if(str.back()=='.')
        str.pop_back();
    return str;
}

std::string& remove_zeros(std::string&& str)
{
    str.erase(std::find_if(str.rbegin(),str.rend(),[](const char& c){
        return c!='0';
    }).base(),str.end());
    if(str.back()=='.')
        str.pop_back();
    return str;
}


template<class T>
T round_to(const T& val, const int& N)
{
    double NPowered= pow(10,N);
    return trunc(val*NPowered)/NPowered;
}
struct RGB{
    RGB()=default;
    RGB(const uint8_t& R,const uint8_t& G,const uint8_t& B)
    : R(R)
    , G(G)
    , B(B)
    {}
    uint8_t R;
    uint8_t G;
    uint8_t B;
};


namespace YAML {
    template<>
    struct convert<RGB>{
        static Node encode(const RGB& rhs) {
            Node node;
            node.push_back(rhs.R);
            node.push_back(rhs.G);
            node.push_back(rhs.B);
            return node;
        }

        static bool decode(const Node& node, RGB& rhs) {
            if(!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs.R = node[0].as<uint8_t>();
            rhs.G = node[1].as<uint8_t>();
            rhs.B = node[2].as<uint8_t>();
            return true;
        }
    };
}

template <class T>
T translate(const T& Min,const T& Max,const T& toMin,const T& toMax,const T& value){
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}

double translate(const double& Min,const double& Max,const double& toMin,const double& toMax,const double& value){
    return ((value-Min)/(Max-Min))*(toMax-toMin)+toMin;
}

template<class T>
T enclose(const T& x,const T& min,const T& max)
{
    return x<min?min:x>max?max:x;
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

long double G(long double& x,long double& d)
{
    return x/(d*d);
}

long double G(const long long int& x,const long long int& d)
{
    return (long double)x/(long double)(d*d);
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

class App: public EventHandler::Subscriber{
    typedef float&  flr;
    typedef const float   cfl;
    typedef const float&  cflr;

    typedef const int   cint;
    typedef const int&  cintr;

    typedef const long int   clint;
    typedef const long int&  clintr;

    typedef const long long int cllint;
    typedef const long long int& cllintr;

    typedef const unsigned int   cuint;
    typedef const unsigned int&  cuintr;

    typedef const std::string   cstring;
    typedef const std::string&  cstringr;
    typedef const std::string&& cstringrr;


    class MassObj{
    public:
        MassObj(cllintr _mass,cV2fr moveSpeed,cV2fr position)
        : mass(_mass)
        , pos(position)
        , moveSpeed(moveSpeed)
        {

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

        template<class C>
        void setMass(cllintr _mass, void (C::*onMassEdit)(), C* c) {
            MassObj::mass = _mass;
            if(onMassEdit!= nullptr)
                c->onMassEdit();
        }

        template<class T>
        void setPos(const sf::Vector2<T>& position) {
            MassObj::pos = sf::Vector2f(position);
        }

        void setMoveSpeed(cV2fr _moveSpeed) {
            MassObj::moveSpeed = _moveSpeed;
        }


    private:
        V2f moveSpeed;
        long long int mass;
        sf::Vector2f pos;

    };

    struct Point{
    public:
        Point()=default;
        explicit Point(cV2fr fixedPosition)
        : pos(fixedPosition)
        , fixedPos(fixedPosition)
        {}
        [[nodiscard]] sf::Vector2f getPos () const
        {
            return pos;
        }
        [[nodiscard]] sf::Vector2f getFPos() const
        {
            return pos;
        }

        void setPos(cV2fr position) {
            Point::pos = position;
        }

        void setFixedPos(cV2fr fixedPosition) {
            Point::fixedPos = fixedPosition;
        }

        void fix()
        {
            pos=fixedPos;
        }

        Point& operator=(const Point& val)=default;
    private:
        sf::Vector2f pos;
        sf::Vector2f fixedPos;
    };

public:
    App(cflr windowSize,cstringr title,cuintr frameLimit,const sf::ContextSettings& settings,
        cuintr pointCount,const float& DP, cflr _offset,const sf::Font& font)
    : window(new sf::RenderWindow(sf::VideoMode((uint)windowSize,(uint)windowSize),title,
                                  sf::Style::Close,settings))
    , event(window)
    , Subscriber(&event)
    , WSize(windowSize)
    , PCount(pointCount)
    , MPCount(pointCount-1)
    , Dp(DP)
    , DPWSize(windowSize/DP)
    , PixelsInDp(windowSize/(windowSize/Dp))
    , offset(_offset)
    , relativeCanvasSize(1 - 2 * _offset)
    , canvasSize(DPWSize*relativeCanvasSize)
    , mainFont(font)
    , unitValue(canvasSize / (float)(PCount - 1))
    , showPoints(true)
    , pickedObj(nullptr)
    , LBPressed(false)
    , pointRadius(ceil(canvasSize / 12.5f /PCount))
    , objectRadius(ceil(canvasSize / 5.f /PCount))
    {
        window->setFramerateLimit(frameLimit);

        points=new Point*[PCount];

        for (int i = 0; i < PCount; ++i) {

            points[i]=new Point[PCount];

            for (int j = 0; j < PCount; ++j) {

                points[i][j]=Point(V2f(unitValue*i,unitValue*j)+DPWSize*offset);
            }
        }

        VerticalMiddles=new Point*[MPCount];

        for(int i=0;i<MPCount;++i)
        {
            VerticalMiddles[i]=new Point[PCount];

            for (int j = 0; j < PCount; ++j) {
                Point& point=points[i][j];
                Point& pointAfter=points[i+1][j];
                VerticalMiddles[i][j]=Point((point.getFPos()-pointAfter.getFPos())*0.5+pointAfter.getFPos());
            }
        }

        HorizontalMiddles=new Point*[PCount];

        for(int i=0;i<PCount;++i)
        {
            HorizontalMiddles[i]=new Point[MPCount];

            for (int j = 0; j < MPCount; ++j) {
                Point& point=points[i][j];
                Point& pointAfter=points[i][j+1];
                HorizontalMiddles[i][j]=Point((point.getFPos()-pointAfter.getFPos())*0.5+pointAfter.getFPos());
            }
        }

        event.SubscribeAll(this);
        obj.emplace_back(5000,V2f {0,0},V2f(dp(sf::Mouse::getPosition(*window))));

        info["massInfo"].setFont(mainFont);
        info["XmvspInfo"].setFont(mainFont);
        info["YmvspInfo"].setFont(mainFont);

        info["massInfo"].setFillColor(sf::Color::Black);
        info["XmvspInfo"].setFillColor(sf::Color::Black);
        info["YmvspInfo"].setFillColor(sf::Color::Black);

        info["massInfo"].setString("Mass: "+std::to_string(obj.begin()->getMass()));
        info["YmvspInfo"].setString("Y Move speed: " + remove_zeros(std::to_string(obj.begin()->getMoveSpeed().y)));
        info["XmvspInfo"].setString("X Move speed: " + remove_zeros(std::to_string(obj.begin()->getMoveSpeed().x)));

        float DPoffset=offset*DPWSize;
        info["massInfo"].setCharacterSize(pix(DPoffset*canvasSize/3200.f));
        info["XmvspInfo"].setCharacterSize(pix(DPoffset*canvasSize/3200.f));
        info["YmvspInfo"].setCharacterSize(pix(DPoffset*canvasSize/3200.f));

        info["massInfo"].setPosition(pix(V2f (DPoffset,DPWSize-DPoffset)));
        info["XmvspInfo"].setPosition(pix(V2f (DPoffset, DPWSize - DPoffset + dp(info["massInfo"].getCharacterSize()))));
        info["YmvspInfo"].setPosition(V2f(info["XmvspInfo"].getPosition().x+info["XmvspInfo"].getGlobalBounds().width+info["XmvspInfo"].getCharacterSize(),
                                          info["XmvspInfo"].getPosition().y));

    }

    void notify(const sf::Event::EventType& eventType) override {
        const sf::Event& Event=event.getEvent();
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
                        showPoints=!showPoints;
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


    ~App()
    {
        for (int i = 0; i < PCount; ++i) {
            delete [] points[i];
        }
        delete [] points;

        for(int i=0;i<MPCount;++i){
            delete [] VerticalMiddles[i];
            delete [] HorizontalMiddles[i];
        }
        delete [] VerticalMiddles;
        delete [] HorizontalMiddles;

        pickedObj= nullptr;

        delete window;
    }

    int processing()
    {
        while (window->isOpen())
        {

            event.pollEvent();

            obj.begin()->setPos(dp(sf::Mouse::getPosition(*window)));

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

    std::vector<MassObj> obj;

    sf::Font mainFont;

    //Inner variables//
    bool showPoints;
    bool LBPressed;
    MassObj* pickedObj;
    sf::Text* pickedText;

    //Interface//
    std::map<std::string,sf::Text> info; ///< massInfo XmvspInfo YmvspInfo

    //Drawing variables//
    float pointRadius;
    float objectRadius;

    void move_according_to_G(Point& point,const MassObj& object,cflr factor) const
    {
        const V2f relativeVec=object.getPos()-point.getPos();
        const long double d=hypotenuse(relativeVec.x,relativeVec.y);
        const long double attractionPower= enclose(G(object.getMass(),d)/PCount*10.f * relativeCanvasSize,0.L,1.L);
        if(attractionPower>0.001L)
            point.setPos(point.getPos()+relativeVec*attractionPower*factor);
    }

    void updateAttraction(cflr factor)
    {
        for(auto object:obj)
        {
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& point=points[i][j];
                    point.fix();

                    move_according_to_G(point,object,factor);
                }
            }

            for (int i = 0; i < MPCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& middle=VerticalMiddles[i][j];

                    middle.fix();

                    move_according_to_G(middle,object,factor);
                }
            }
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < MPCount; ++j) {
                    Point& middle=HorizontalMiddles[i][j];

                    middle.fix();

                    move_according_to_G(middle,object,factor);
                }
            }
        }
    }

    void onMassEdit()
    {
        info["massInfo"].setString("Mass: "+std::to_string(obj.begin()->getMass()));
        info["YmvspInfo"].setString("Y Move speed: " + remove_zeros(std::to_string(obj.begin()->getMoveSpeed().y)));
        info["XmvspInfo"].setString("X Move speed: " + remove_zeros(std::to_string(obj.begin()->getMoveSpeed().x)));
    }

    void handleText(const wchar_t& c)
    {
        if(pickedText!= nullptr && pickedObj!= nullptr) {
            if (pickedText == &info["massInfo"]) {
                switch (c) {
                    default:
                        if(std::isdigit(c))
                            pickedObj->setMass(pickedObj->getMass() * 10 + std::stoi(std::wstring(1,c)),&App::onMassEdit,this);
                        break;
                    case '\b':
                        pickedObj->setMass(pickedObj->getMass()/10,&App::onMassEdit,this);
                        break;

                }
            }
        }
    }

    void leftClick()
    {
        LBPressed=true;

        pickedObj= nullptr;
        for(const MassObj& object:obj) {

        }
        pickedText= nullptr;
        for(std::map<std::string,sf::Text>::iterator it=info.begin();it!=info.end();++it){
            sf::Text& text=it->second;
            if(text.getGlobalBounds().contains(V2f(sf::Mouse::getPosition(*window))))
                pickedText=&text;
        }
    }
    void update()
    {

    }
    void leftUnclick()
    {
        LBPressed=false;

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
        circle.setFillColor(sf::Color(248, 6, 48));
        circle.setOrigin(circle.getGlobalBounds()/2);
        for (const MassObj& object:obj) {
            circle.setPosition(pix(object.getPos()));

            window->draw(circle);
        }

        pickedObj=&*obj.begin();
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
