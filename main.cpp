#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>
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
            array.append({{second+(first-second)*(1-i*step)},sf::Color::Black});
        }
    }
}

class App{
    typedef float&  flr;
    typedef const float   cfl;
    typedef const float&  cflr;

    typedef const int   cint;
    typedef const int&  cintr;

    typedef const unsigned int   cuint;
    typedef const unsigned int&  cuintr;

    typedef const std::string   cstring;
    typedef const std::string&  cstringr;
    typedef const std::string&& cstringrr;



    class MassObj{
    public:
        MassObj(cintr mass_,cV2fr position)
        : mass(mass_)
        , pos(position)
        {

        }

        [[nodiscard]] int getMass() const {
            return mass;
        }

        [[nodiscard]] const sf::Vector2f& getPos() const {
            return pos;
        }

        void setMass(cintr _mass) {
            MassObj::mass = _mass;
        }

        template<class T>
        void setPos(const sf::Vector2<T>& position) {
            MassObj::pos = sf::Vector2f(position);
        }

    private:
        int mass;
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

        Point& operator=(const Point& val)
        {
            pos=val.pos;
            fixedPos=val.fixedPos;
            return *this;
        }
    private:
        sf::Vector2f pos;
        sf::Vector2f fixedPos;
    };

public:
    App(cflr windowSize,cstringr title,cuintr frameLimit,const sf::ContextSettings& settings,cuintr pointCount,const float& DP, cflr _offset,
        const sf::Font& font)
    : window(new sf::RenderWindow(sf::VideoMode((uint)windowSize,(uint)windowSize),title,
                                  sf::Style::Close,settings))
    , event(window)
    , WSize(windowSize)
    , PCount(pointCount)
    , MPCount(pointCount-1)
    , Dp(DP)
    , DPWSize(windowSize/DP)
    , PixelsInDp(windowSize/(windowSize/Dp))
    , offset(_offset)
    , relativeCanvasSize(1 - 2 * _offset)
    , mainFont(font)
    {
        window->setFramerateLimit(frameLimit);

        points=new Point*[PCount];

        for (int i = 0; i < PCount; ++i) {

            points[i]=new Point[PCount];

            for (int j = 0; j < PCount; ++j) {

                const float unitValue= (DPWSize * relativeCanvasSize) / (PCount - 1);
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



        obj.emplace_back(5000,V2f(dp(sf::Mouse::getPosition(*window))));
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

        delete window;
    }


    int processing()
    {
        while (window->isOpen())
        {

            event.pollEvent();
            if(event.happened(sf::Event::Closed))
                window->close();
            if(event.happened(sf::Event::Resized))
                std::cout<<window->getSize().y;
            obj.begin()->setPos(dp(sf::Mouse::getPosition(*window)));
            updateAttraction(1);
            render();
        }
        return 0;
    }
    void MouseInput()
    {

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

            /*for (int i = 0; i < MPCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& point=points[i][j];
                    Point& pointAfter=points[i+1][j];
                    Point& middle=VerticalMiddles[i][j];

                    middle.setPos((point.getFPos()-pointAfter.getFPos())*0.5+pointAfter.getFPos());
//                    middle.fix();

                    move_according_to_G(middle,object,factor);
                }
            }
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < MPCount; ++j) {
                    Point& point=points[i][j];
                    Point& pointAfter=points[i][j+1];
                    Point& middle=HorizontalMiddles[j][i];

                    middle.setPos((point.getFPos()-pointAfter.getFPos())*0.5+pointAfter.getFPos());

                    move_according_to_G(middle,object,factor);
                }
            }*/
        }
    }

private:
    float Dp;
    float PixelsInDp;

    sf::RenderWindow* window;
    float WSize;
    float DPWSize;
    EventHandler event;

    Point** points;
    uint PCount;
    Point** VerticalMiddles;
    Point** HorizontalMiddles;
    uint MPCount;

    float offset;
    float relativeCanvasSize;

    std::vector<MassObj> obj;

    sf::Font mainFont;

    void move_according_to_G(Point& point,const MassObj& object,cflr factor) const
    {
        V2f relativeVec=object.getPos()-point.getPos();
        const long double d=hypotenuse(relativeVec.x,relativeVec.y);
        const long double attractionPower= enclose(G(object.getMass(),d) * relativeCanvasSize,0.L,1.L);
        if(attractionPower>0.001L)
            point.setPos(point.getPos()+relativeVec*attractionPower*factor);
    }

    void render()
    {
        window->clear(sf::Color::White);


        for (int i = 0; i < PCount; ++i) {
            const float DPoffset=DPWSize*offset;

            sf::Text coordinates(std::to_string(i),mainFont,pix(DPoffset/4));
            coordinates.setFillColor(sf::Color::Black);
            const float unitValue=DPWSize*relativeCanvasSize/(PCount-1);
            coordinates.setOrigin(coordinates.getGlobalBounds()/2);

            const float shift=pix(DPoffset*0.5f);
            coordinates.setPosition(pix(sf::Vector2f(unitValue*i+DPoffset,shift)));
            window->draw(coordinates);
            coordinates.setPosition(pix(sf::Vector2f(shift,unitValue*i+DPoffset)));
            window->draw(coordinates);


            for (int j = 0; j < PCount; ++j) {
                Point& point=points[i][j];
                uint r= DPWSize * relativeCanvasSize / 12.5f /PCount;
                r=r<1?1:r;
                sf::CircleShape temp(r);
                temp.setOrigin(temp.getGlobalBounds()/2);
                temp.setFillColor(sf::Color::Black);
                temp.setPosition(pix(point.getPos()));

                window->draw(temp);
            }
        }

        for (int i = 0; i < MPCount; ++i) {
            for (int j = 0; j < PCount; ++j) {
                sf::VertexArray array(sf::PrimitiveType::LinesStrip);
                Point& point = points[i][j];
                Point& pointAfter = points[i+1][j];
                Point& middle=VerticalMiddles[i][j];

                de_casteljau(pix(point.getPos()), pix(pointAfter.getPos()),
                             pix(middle.getPos()), array,0.1f);


//                sf::CircleShape test(10);
//                test.setOrigin(test.getGlobalBounds()/2);
//                test.setFillColor(sf::Color::Red);
//                test.setPosition(pix(middle.getFPos()));
//                window->draw(test);

                window->draw(array);
            }
        }
        for (int i = 0; i < PCount; ++i) {
            for (int j = 0; j < MPCount; ++j) {
                sf::VertexArray array(sf::PrimitiveType::LinesStrip);
                Point& point = points[i][j];
                Point& pointAfter = points[i][j+1];
                Point& middle=HorizontalMiddles[i][j];

                de_casteljau(pix(point.getPos()), pix(pointAfter.getPos()),
                             pix(middle.getPos()), array,0.1f);

//                sf::CircleShape test(10);
//                test.setOrigin(test.getGlobalBounds()/2);
//                test.setFillColor(sf::Color::Red);
//                test.setPosition(pix(middle.getFPos()));
//                window->draw(test);

                window->draw(array);
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

int main() {
    YAML::Node config=YAML::LoadFile("config.yaml");

    sf::VideoMode fullScreen=sf::VideoMode::getFullscreenModes()[0];
    std::vector<sf::VideoMode> m=sf::VideoMode::getFullscreenModes();

    const float maxWindowSize=fullScreen.width<fullScreen.height?fullScreen.width:fullScreen.height;

    float WindowSize=maxWindowSize;
    if(!config["MwindowSize"].IsNull())
        WindowSize=config["MwindowSize"].as<float>();
    else
    if(config["windowSize"].as<std::string>()=="halfAvailable")
    {
        WindowSize=maxWindowSize/2;
    }else
    if(config["windowSize"].as<std::string>()=="partAvailable")
    {
        WindowSize=maxWindowSize/1.5f;
    }else
    if(config["windowSize"].as<std::string>()=="fullAvailable")
    {
        WindowSize=maxWindowSize;
    }

    float dp=WindowSize/config["dpInWindow"].as<float>();

    std::cout<<"Window size: "<<WindowSize<<std::endl<<"Independent pixel: "<<dp<<std::endl;

    const float offset=enclose(config["offset"].as<float>(),0.05f,0.4f);
    sf::Font font;
    font.loadFromFile(config["fontFile"].as<std::string>());

    sf::ContextSettings settings;
    settings.antialiasingLevel=config["antialiasingLevel"].as<unsigned int>();
    App app(WindowSize,"Gravity",30,settings,10,dp,offset,font);
    app.processing();


    return 0;
}
