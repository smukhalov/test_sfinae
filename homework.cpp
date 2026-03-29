#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <type_traits>

using namespace std::string_literals;

#include <tuple>

// ########## vector #############
template <typename T>
struct is_vector : std::false_type
{};

// Partial specialization for std::vector
template <typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> : std::true_type
{};

template <typename T>
constexpr bool is_vector_v = is_vector<std::remove_cvref_t<T>>::value;
// ########## vector #############

// ########## list #############
template <typename T>
struct is_list : std::false_type
{};

template <typename T, typename Allocator>
struct is_list<std::list<T, Allocator>> : std::true_type
{};

template <typename T>
constexpr bool is_list_v = is_list<std::remove_cvref_t<T>>::value;
// ########## list #############

// ########## numeric #############
template <typename T>
struct is_strictly_numeric_integral{
    static constexpr bool value = std::is_integral_v<std::remove_cvref_t<T>> &&
                                  !std::is_same_v<std::remove_cvref_t<T>, bool> &&
                                  !std::is_same_v<std::remove_cvref_t<T>, char>;
};
template <typename T>
constexpr bool is_strictly_numeric_integral_v = is_strictly_numeric_integral<T>::value;
// ########## numeric #############

// ########## string #############
template <typename T>
struct is_string{
    static constexpr bool value = std::is_same_v<std::remove_cvref_t<T>, std::string>;
};

template <typename T>
constexpr bool is_string_v = is_string<T>::value;
// ########## string #############

// ########## tuple #############
// template <typename First, typename... T>
// struct all_same_type{
//     constexpr static bool value = std::is_same_v<std::tuple<First, T...>, std::tuple<T..., First>>;
// };
// template <typename... T>
// struct all_same_type<std::tuple<T...>> : all_same_type<T...>
// {};
// template <typename... T>
// constexpr bool all_same_type_v = all_same_type<T...>::value;

// template <typename Head, typename... Tail>
// constexpr bool all_same(const std::tuple<Head, Tail...> &){
//     return (std::is_same_v<Head, Tail> && ...);
// }
// constexpr bool all_same(const std::tuple<> &){
//     return true;
// }
template <typename First, typename... T>
struct all_same_type_tuple {
    constexpr static bool value = std::is_same_v<std::tuple<First, T...>, std::tuple<T..., First>>;
};

template <typename... T>
    struct all_same_type_tuple < std::tuple<T...>> : all_same_type_tuple<T...>
{};

template <typename... T>
constexpr bool all_same_type_tuple_v = all_same_type_tuple < std::decay_t<T>...>::value;

// ########## tuple #############

template <typename T,
    typename Enable = std::enable_if_t<is_string_v<T> 
        || is_strictly_numeric_integral_v<T> 
        || is_vector_v<T> || is_list_v<T>  || all_same_type_tuple_v<T>, 
    void>>
void Print(T &&t)
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    if constexpr (is_string_v<T>){
        std::cout << "string - " << const_cast<std::string&>(t) << '\n';
    } 
    else if constexpr (is_strictly_numeric_integral_v<T>){
        std::cout << "int - " << t << "; ";
        std::size_t size = sizeof(T);
        std::vector<unsigned char> aux_v(size);
        for(size_t i = 0; i < size; ++i){
            aux_v[size - i - 1] = (t >> (8 * i));
        }
        bool need_dot = false;
        for(const auto& x : aux_v){
            if (need_dot){
                std::cout << ".";
            }
            std::cout << static_cast<uint16_t>(x);
            if (!need_dot){
                need_dot = true;
            }
        }
        std::cout << '\n';
    } 
    else if constexpr (is_vector_v<T>){
        std::cout << "vector -";
        bool need_dot = false;
        for(const auto& x : t){
            if (need_dot){
                std::cout << ".";
            }
            std::cout << x;
            if (!need_dot){
                need_dot = true;
            }
        }
        std::cout << '\n';
    } 
    else if constexpr (is_list_v<T>) {
        std::cout << "list -";
        bool need_dot = false;
        for (const auto &x : t)
        {
            if (need_dot)
            {
                std::cout << ".";
            }
            std::cout << x;
            if (!need_dot)
            {
                need_dot = true;
            }
        }
        std::cout << '\n';
    } 
    else {
        static_assert(all_same_type_tuple_v<T&&>);
        std::stringstream ss;
        std::cout << "tuple - ";
        std::apply([&ss](auto &&...args)
                   { ((ss << args << '.'), ...); }, t);

        const std::string& s = ss.str();
        std::cout << s.substr(0, s.size()-1);
        std::cout << '\n';
    }
}

int main(){
    {
        std::string s = "Test12";
        Print(s);

        const std::string s1 = "const Test12";
        Print(s1);

        volatile std::string s2 = "volatile Test12";
        Print(s2);

        const volatile std::string s3 = "const volatile Test13";
        Print(s3);

        Print(std::string{"string r-value"});
        Print("string r-value literal"s);
    }

    {
        int x = -1;
        Print(x);

        const int y = 2130706433;
        Print(y);

        volatile int yy = 5;
        Print(yy);

        Print(-24);

        uint64_t z = 8875824491850138409; // std::numeric_limits<uint64_t>::max();
        Print(z);
    }

    {
        std::vector<int> x{1, 2, 3};
        Print(x);

        const std::vector<int> y{4, 5, 6};
        Print(y);

        Print(std::vector<int>{7, 8, 9});
    }

    {
        std::list<short> x{400, 300, 200, 100};
        Print(x);

        const std::list<int> y{4, 5, 6};
        Print(y);

        Print(std::list<int>{7, 8, 9});
    }

    {
        auto x = std::make_tuple(1, 2, 3, 1.0f, "asdf"s);
        //Print(x);

        const auto y = std::make_tuple(4, 5, 6);
        Print(y);

        Print(std::make_tuple(7, 8, 9));

        auto ss = std::make_tuple("1"s, "2"s, "3"s, 1.0f, "asdf"s);
        //Print(ss);
    }

    // std::tuple t{42, 'a', 4.2}; // Another C++17 feature: class template argument deduction
    // std::apply([](auto &&...args)
    //            { ((std::cout << args << '\n'), ...); }, t);

    std::cout << "OK\n";

    // std::tuple<int, int, int> t = {1, 1, 1};
    // std::cout << all_same_type_tuple_v<decltype(t)> << "\n";

    // std::tuple<int, float, int> u = {1, 1.0, 1};
    // std::cout << all_same_type_tuple_v<decltype(u)> << "\n";

    // std::tuple<int, int, int> t1{1, 1, 1};
    // std::cout << all_same_type_tuple_v<decltype(t1)> << "\n";
    //static_assert(all_same_type_tuple(decltype(t1)));
    // std::tuple<int, float, int> t2{1, 1.0f, 1};
    // std::cout << all_same_type_tuple_v<decltype(t2)> << "\n";
    //static_assert(!all_same_type(t2));

    return 0;
}