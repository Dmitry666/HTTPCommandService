#ifndef HTTP_DELEGATE_H
#define HTTP_DELEGATE_H

#include <vector>
#include <functional>

template<typename ...Types>
class Delegate
{
    template<typename ...InnerTypes>
    class IConteiner
    {
    public:
        virtual ~IConteiner()
        {}

        virtual void Call(InnerTypes ...args) = 0;
    };

    template<typename TFunc, typename ...InnerTypes>
    class Conteiner
        : public IConteiner<InnerTypes...>
    {
    public:
        Conteiner(TFunc func)
            : _func(func)
        {}

        virtual void Call(InnerTypes ... args) override
        {
            _func(args...);
        }

    protected:
        TFunc _func;
    };

public:
    ~Delegate()
    {
        Clear();
    }

    void Clear()
    {
        for(IConteiner<Types...>* conteiner : _containers)
            delete conteiner;
        _containers.clear();
    }

    void Call(Types ... args)
    {
        for(IConteiner<Types...>* conteiner : _containers)
            _containers->Call(args...);
    }

    Delegate& operator ()(Types ... args)
    {
        Call(args...);
        return *this;
    }

    template<typename TFunc>
    Delegate& operator += (TFunc func)
    {
        auto conteiner = new Conteiner<TFunc, Types...>(func);
        _containers.push_back(conteiner);

        return *this;
    }

    template<typename TFunc>
    Delegate& operator = (TFunc func)
    {
        Clear();

        return (*this) += func;
    }

protected:
    std::vector<IConteiner<Types...>*> _containers;
};

#endif // DELEGATE

