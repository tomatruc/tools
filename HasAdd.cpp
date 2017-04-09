// g++ sfinae.cpp -std=c++1z -fconcepts -o sfinae


#include <iostream>


class A
{
protected:
   int m_n;
public:
   A()
      : m_n(0)
   {
   }
   A(int n)
      : m_n(n)
   {
   }
   ~A() = default;
   int get()
   {
      return m_n;
   }
};

class B : public A
{
public:
   B() = default;
   B(int n)
      : A(n)
   {
   }
   ~B() = default;
   int add(int n)
   {
      m_n += n;
      return m_n;
   }
};

class C : public B
{
public:
   C() = default;
   C(int n)
      : B(n)
   {}
   ~C() = default;
};

class D : public A
{
public:
   D() = default;
   D(int n)
      : A(n)
   {
   }
   ~D() = default;
   void add(int n)
   {
      m_n += n;
   }
};



template<class T, class R>
T ClassOfMethod(R T::*);

template <typename U>
using LocalAddMethodType = auto (U::*)(int) -> int;

template <typename U>
using AddMethodType = LocalAddMethodType<decltype(ClassOfMethod(&U::add))>;

template <typename U, U>
struct TypeChecker;



// using template specialisation + sfinae
template <typename T, typename = void>
class has_add_1 : public std::false_type {};
template <typename T>
class has_add_1<T, std::void_t<TypeChecker<AddMethodType<T>, &T::add>>> : public std::true_type {};



// using variadic function overloading + sfinae
template <typename T>
class has_add_2
{
private:
   template <typename U> static std::false_type Test(...);
   template <typename U> static std::true_type Test(TypeChecker<AddMethodType<U>, &U::add>*);
public:
   using type = decltype(Test<T>(nullptr));
   static const bool value = type::value;
};



// using concept
template <typename T>
concept bool has_add_3 = requires(LocalAddMethodType<T> pAdd)
{
   {pAdd = &T::add};
};



int main()
{
   std::cout << "has_add_1<A> = " << has_add_1<A>::value << " ; "
             << "has_add_2<A> = " << has_add_2<A>::value << " ; "
             << "has_add_3<A> = " << has_add_3<A> << std::endl;
   std::cout << "has_add_1<B> = " << has_add_1<B>::value << " ; "
             << "has_add_2<B> = " << has_add_2<B>::value << " ; "
             << "has_add_3<B> = " << has_add_3<B> << std::endl;
   std::cout << "has_add_1<C> = " << has_add_1<C>::value << " ; "
             << "has_add_2<C> = " << has_add_2<C>::value << " ; "
             << "has_add_3<C> = " << has_add_3<C> << std::endl;
   std::cout << "has_add_1<D> = " << has_add_1<D>::value << " ; "
             << "has_add_2<D> = " << has_add_2<D>::value << " ; "
             << "has_add_3<D> = " << has_add_3<D> << std::endl;
   return 0;
}


