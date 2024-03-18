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
        size_t oldCap = 0u;
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
            oldCap = str.capacity();

            str.append(" ");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 6u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() > oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello ]" );
        }

        {
            oldCap = str.capacity();

            str.append("wor");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 9u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() == oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello wor]" );
        }

        {
            oldCap = str.capacity();

            str.append("ld");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 11u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() > oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world]" );
        }

        {
            oldCap = str.capacity();

            str.shrink_to_fit();
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 11u );
            assert( str.capacity() == str.size() );
            assert( str.capacity() < oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world]" );
        }

        {
            oldCap = str.capacity();

            str.push_back('!');
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 12u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() > oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world!]" );
        }

        {
            oldCap = str.capacity();

            str.assign("New World!");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 10u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() == oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[New World!]" );
        }

        {
            oldCap = str.capacity();

            str.assign("New World! Where no man has gone before!");
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 40u );
            assert( str.capacity() == str.size() );
            assert( str.capacity() > oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[New World! Where no man has gone before!]" );
        }

        {
            oldCap = str.capacity();

            str.clear();
            std::cout << str.size() << "," << str.capacity() << "\n";
            assert( str.size() == 0u );
            assert( str.capacity() > str.size() );
            assert( str.capacity() == oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[]" );
        }
    }
}
