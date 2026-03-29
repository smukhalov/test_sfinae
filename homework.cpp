#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

using namespace std::string_literals;

#include <tuple>

// Primary template: default to false
template <typename T>
struct is_vector : std::false_type
{};

// Partial specialization for std::vector
template <typename T, typename Allocator>
struct is_vector<std::vector<T, Allocator>> : std::true_type
{};

// Helper constant for convenience (C++14 onwards)
template <typename T>
constexpr bool is_vector_v = is_vector<std::remove_cvref_t<T>>::value;

template <typename T>
struct is_strictly_numeric_integral{
    static constexpr bool value = std::is_integral_v<std::remove_cvref_t<T>> &&
                                  !std::is_same_v<std::remove_cvref_t<T>, bool> &&
                                  !std::is_same_v<std::remove_cvref_t<T>, char>;
};
template <typename T>
constexpr bool is_strictly_numeric_integral_v = is_strictly_numeric_integral<T>::value;

template <typename T>
struct is_string{
    static constexpr bool value = std::is_same_v<std::remove_cvref_t<T>, std::string>;
};

template <typename>
struct is_tuple : std::false_type
{};

template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type
{};

template <typename... T>
constexpr bool is_tuple_v = is_tuple<std::tuple<T...>>::value;

template <typename T>
constexpr bool is_string_v = is_string<T>::value;

template <typename T,
    typename Enable = std::enable_if_t<is_string_v<T> 
        || is_strictly_numeric_integral_v<T> 
        || is_vector_v<T>, //|| is_tuple<T>,
                                             void>>
void Print(T &&t)
{
    std::cout << __PRETTY_FUNCTION__ << '\n';
    if constexpr (is_string_v<T>){
        std::cout << "string - " << const_cast<std::string&>(t) << '\n';
    } else if constexpr (is_strictly_numeric_integral_v<T>){
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
    } else if constexpr (is_vector_v<T>){
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
    } else {
        std::cout << "tuple" << '\n';
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
        std::vector<int> v{1, 2, 3};
        std::cout << std::boolalpha << is_vector_v<std::vector<int>> << '\n';
    }
     std::cout << "OK\n";

     return 0;
}