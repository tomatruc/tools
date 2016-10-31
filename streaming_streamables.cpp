
// for solutions 1 and 2

template<typename U, U>		
struct Checker;		
template <typename StreamType		
        , typename T		
        , typename = typename std::enable_if<std::is_enum<T>::value>::type		
        , typename = Checker<auto(*)(StreamType&, const T&) -> StreamType&, &toto::operator<< <StreamType> > >		
inline StreamType& operator<<(StreamType& stream, const T& x)		
{		
   return toto::operator<<(stream, x);		
}		


// *************

// solution 1

template<typename StreamType, typename T>
    struct HasStreamingOperator
    {
        template<typename U> using StreamingOperatorType = auto(StreamType&, const U&) -> StreamType&;
        template<typename U, StreamingOperatorType<U>> struct Checker {};
        template<typename U> static std::true_type Test(Checker<U, &::operator<< <StreamType, U> >*);
        template<typename U> static std::false_type Test(...);
        using type = decltype(Test<T>(nullptr));
        static const bool value = type::value;
    };

    template <typename T>
    template <typename StreamType>
    inline StreamType& EnumTruc::operator<<(StreamType& ioStream) const
    {
        if (true == HasStreamingOperator<StreamType, T>::value)
        {
            ioStream << this->Get();
        }
        else
        {
            const std::underlying_type_t<T>& value = this->GetAsUnderlying();

            if ((true == std::is_same<std::underlying_type_t<T>, char>::value) &&
                unlikely((value < 32) || (value > 126)))
            {
                ioStream << static_cast<int>(value) << ">";
            }
            else
            {
                ioStream << value;
            }
        }

        return ioStream;
    }


// *************

// solution 2

    template<typename StreamType, typename T>
    class HasStreamingOperator
    {
    private:
        template<typename U>
        using StreamingOperatorType = auto(*)(StreamType&, const U&) -> StreamType&;
        template<typename U, U>
        struct Checker;
        template<typename U>
        static std::true_type Test(Checker<StreamingOperatorType<U>, &::operator<< >*);
        template<typename U>
        static std::false_type Test(...);
    public:
        using type = decltype(Test<T>(nullptr));
        static const bool value = type::value;
    };

    template <typename StreamType, typename T, typename Enable = void>
    class StreamableEnum
    {
    private:
        const T & m_t;
    public:
        StreamableEnum(const T & t) : m_t(t) {}
        inline StreamType& operator<<(StreamType& stream) const
        {
            const std::underlying_type_t<T>& value = enum_cast<T>(m_t);
            if ((true == std::is_same<std::underlying_type_t<T>, char>::value) &&
                unlikely((value < 32) || (value > 126)))
            {
                stream << "<" << static_cast<int>(value) << ">";
            }
            else
            {
                stream << value;
            }
            return stream;
        }
    };

    //template<typename U, U>
    //struct Checker;
    template <typename StreamType, typename T>
    class StreamableEnum<StreamType, T, typename std::enable_if<HasStreamingOperator<StreamType, T>::value>::type>
    //class StreamableEnum<StreamType, T, Checker<auto(*)(StreamType&, const T&) -> StreamType&, &::operator<< <StreamType, T> > >
    {
    private:
        const T & m_t;
    public:
        StreamableEnum(const T & t) : m_t(t) {}
        inline StreamType& operator<<(StreamType& stream) const
        {
            stream << m_t;
            return stream;
        }
    };

    template <typename StreamType, typename T>
    inline StreamType& operator<<(StreamType& stream, const StreamableEnum<StreamType, T>& value)
    {
        value.operator<<(stream);
        return stream;
    }

    template <typename T>
    template <typename StreamType>
    inline StreamType& EnumTruc::operator<<(StreamType& ioStream) const
    {
        ioStream << StreamableEnum<StreamType, T>(this->Get());
        return ioStream;
    }


// *************

// solution 3

    template <typename StreamType, typename T>
    concept bool Streamable()
    {
        return requires(StreamType& stream, const T& x) { stream << x; };
    }

    template <typename T>
    inline rtcommon::LogStream& EnumTruc::operator<<(rtcommon::LogStream& ioStream) const
    {
        if constexpr (true == Streamable<T>())
        {
            ioStream << this->Get();
        }
        else
        {
            const std::underlying_type_t<T>& value = this->GetAsUnderlying();

            if ((true == std::is_same<std::underlying_type_t<T>, char>::value) && unlikely((value < 32) || (value > 126)))
            {
                ioStream << "<" << static_cast<int>(value) << ">";
            }
            else
            {
                ioStream << value;
            }
        }

        return ioStream;
    }


