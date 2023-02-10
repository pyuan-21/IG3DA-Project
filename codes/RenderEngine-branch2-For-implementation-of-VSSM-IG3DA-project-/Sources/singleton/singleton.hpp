#pragma once

namespace IceRender
{
    template <typename T>
    class Singleton
    {
    public:
        static T& GetInstance()
        {
            static T instance;
            return instance;
        };

    protected:
        Singleton() {};
        ~Singleton() {};

    public:
        Singleton(Singleton const&) = delete;
        void operator=(Singleton const&) = delete;
    };
}
