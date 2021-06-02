#include <iostream>
#include <vector>
#include "yaml-cpp/yaml.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "EventHandler.hpp"

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
    return (double)x/(double)(d*d);
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

    typedef sf::Vector2f   V2f;
    typedef sf::Vector2f&  V2fr;
    typedef sf::Vector2f&& V2frr;
    typedef const sf::Vector2f   cV2f;
    typedef const sf::Vector2f&  cV2fr;
    typedef const sf::Vector2f&& cV2frr;

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

    private:
        sf::Vector2f pos;
        sf::Vector2f fixedPos;
    };

public:
    App(cflr windowSize,cstringr title,cuintr frameLimit,cuintr pointCount,const float& DP)
    : window(new sf::RenderWindow(sf::VideoMode((uint)windowSize,(uint)windowSize),title))
    , event(window)
    , WSize(windowSize)
    , PCount(pointCount)
    , Dp(DP)
    , DPWSize(windowSize/DP)
    , PixelsInDp(windowSize/(windowSize/Dp))
    {
        window->setFramerateLimit(frameLimit);

        points=new Point*[pointCount];

        for (int i = 0; i < pointCount; ++i) {

            points[i]=new Point[pointCount];

            for (int j = 0; j < pointCount; ++j) {

                const float unitValue=(DPWSize*0.8f)/(PCount-1);
                points[i][j]=Point(V2f(unitValue*i,unitValue*j)+DPWSize*0.1);
            }
        }



        obj.emplace_back(5000,V2f(dp(sf::Mouse::getPosition(*window))));
    }
    ~App()
    {
        delete window;
        for (int i = 0; i < PCount; ++i) {
                delete points[i];
        }
        delete [] points;
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
    void updateAttraction(cintr factor)
    {
        for(auto object:obj)
        {
            for (int i = 0; i < PCount; ++i) {
                for (int j = 0; j < PCount; ++j) {
                    Point& point=points[i][j];
                    point.fix();

                    V2f relativeVec=object.getPos()-point.getPos();
                    const double d=sqrt(pow(relativeVec.x,2)+pow(relativeVec.y,2));
                    long double attractionPower=G(object.getMass(),d);
                    attractionPower=attractionPower>1?1:attractionPower;
                    if(attractionPower>0.001)
                    point.setPos(point.getFPos()+relativeVec*attractionPower*factor);
                }
            }
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

    std::vector<MassObj> obj;

    void render()
    {
        window->clear(sf::Color::White);

        sf::VertexArray array(sf::PrimitiveType::Lines);

        for (int i = 0; i < PCount; ++i) {
            for (int j = 0; j < PCount; ++j) {
                Point& point=points[i][j];
                sf::CircleShape temp(WSize/200);
                temp.setOrigin(temp.getGlobalBounds()/2);
                temp.setFillColor(sf::Color::Black);
                temp.setPosition(pix(point.getPos()));

                window->draw(temp);
            }
        }
        for (int i = 0; i < PCount; ++i) {
            for (int j = 0; j < PCount; ++j) {
                Point& point=points[i][j];
                array.append({pix(point.getPos()),sf::Color::Black});

            }
        }
        for (int i = 0; i < PCount; ++i) {
            for (int j = 1; j < PCount-1; ++j) {
                Point& point=points[i][j];
                array.append({pix(point.getPos()),sf::Color::Black});

            }
        }
        for (int i = 0; i < PCount; ++i) {
            for (int j = 0; j < PCount; ++j) {
                Point& point=points[j][i];
                array.append({pix(point.getPos()),sf::Color::Black});

            }
        }
        for (int i = 0; i < PCount; ++i) {
            for (int j = 1; j < PCount-1; ++j) {
                Point& point=points[j][i];
                array.append({pix(point.getPos()),sf::Color::Black});

            }
        }
        window->draw(array);


        window->display();
    }

    [[nodiscard]] double dp(const double& pix) const
    {
        return pix/Dp;
    }

    [[nodiscard]] double pix(const double& dp) const
    {
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
    sf::VideoMode fullScreen=sf::VideoMode::getFullscreenModes()[0];
    std::vector<sf::VideoMode> m=sf::VideoMode::getFullscreenModes();
//    for(auto it=m.begin();it!=m.end();it++)
//        std::cout<<"Parameters:\n"
//                   "Width: " <<it->width<<"\n"
//                   "Height: "<<it->height<<"\n"
//                   "Pixels: "<<it->bitsPerPixel
//                   <<std::endl<<std::endl;
    float maxWindowSize=fullScreen.width<fullScreen.height?fullScreen.width:fullScreen.height;

    float WindowSize=maxWindowSize/1.5f;
    float dp=WindowSize/1000.f;

    std::cout<<"Window size: "<<WindowSize<<std::endl<<"Independent pixel: "<<dp<<std::endl;


    App app(WindowSize,"Gravity",30,10,dp);
    app.processing();



    return 0;
}
