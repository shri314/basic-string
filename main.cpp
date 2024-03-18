#include "BasicString.hpp"

#include <iostream>
#include <cstring>
#include <sstream>

int main()
{
    {
        BasicString<char> str;
        std::cout << str.size() << "," << str.capacity() << "\n";

        assert( str.size() == 0u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.empty() == true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[]" );
    }

    {
        char a[] = "hello";
        BasicString<char> str = a;
        std::cout << str.size() << "," << str.capacity() << "\n";

        assert( str.size() == 5u );
        assert( str.capacity() >= str.size() );
        assert( str.data() != nullptr );
        assert( str.empty() == false );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[hello]" );

        {
            str.append(" ");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 6u );
            assert( str.capacity() >= str.size() );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello ]" );
        }

        {
            str.append("world");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 11u );
            assert( str.capacity() >= str.size() );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world]" );
        }

        {
            str.append("!!");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 13u );
            assert( str.capacity() >= str.size() );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world!!]" );
        }

        {
            str.append(" This is the land of the greats");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 44u );
            assert( str.capacity() >= str.size() );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world!! This is the land of the greats]" );
        }

        {
            str.append("!!");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 46u );
            assert( str.capacity() >= str.size() );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world!! This is the land of the greats!!]" );
        }
    }
}
