#include "BasicString.hpp"

#include <iostream>
#include <cstring>
#include <sstream>

int main()
{
    // constructor - default
    {
        BasicString<char> str;

        assert( str.size() == 0u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.empty() == true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[]" );
    }

    // constructor - cstring
    {
        const char* cstr = "Hello";
        BasicString<char> str{cstr};

        assert( str.size() == 5u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.empty() != true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // constructor - array
    {
        const char carray[] = "Hello";
        BasicString<char> str{carray};

        assert( std::size(carray) == 6u );
        assert( str.size() == 5u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.empty() != true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // constructor - cstring with size
    {
        BasicString<char> str{"Hello World", 5};

        assert( str.size() == 5u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.empty() != true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // copy construct
    {
        BasicString<char> str0{"Hello"};
        str0.reserve(50);
        size_t srcCap = str0.capacity();

        BasicString<char> str = str0; // copy construct

        assert( str.size() == 5u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.capacity() < srcCap );
        assert( str.empty() != true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // move construct
    {
        BasicString<char> str0{"Hello"};
        str0.reserve(50);
        size_t srcCap = str0.capacity();

        BasicString<char> str = std::move(str0); // move construct

        assert( str.size() == 5u );
        assert( str.data() != nullptr );
        assert( str.capacity() >= str.size() );
        assert( str.capacity() == srcCap );
        assert( str.empty() != true );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // copy assign
    {
        BasicString<char> str0{"Hello New Copy World!"};
        BasicString<char> str{"Something big!Something big!Something big!Something big!Something big!"};
        assert( str.size() == 70 );

        size_t oldCap = str.capacity();
        str = str0; // copy assign

        assert( str.size() == 21u );
        assert( str.capacity() >= str.size() );
        assert( str.capacity() == oldCap );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[Hello New Copy World!]" );
        }

        {
            // check source object
            std::ostringstream oss;
            oss << "[" << str0 << "]";
            assert( oss.str() == "[Hello New Copy World!]" );
        }
    }

    // move assign
    {
        BasicString<char> str0{"Hello New Move World!"};
        BasicString<char> str{"Something big!Something big!Something big!Something big!Something big!"};
        assert( str.size() == 70 );

        size_t oldCap = str.capacity();
        str = std::move(str0); // move assign

        assert( str.size() == 21u );
        assert( str.capacity() == str.size() );
        assert( str.capacity() < oldCap );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[Hello New Move World!]" );
        }

        {
            // check source object
            // note: accessing a moved from object - but this is okay as this
            //       implementation ensures that it is in a valid state.
            std::ostringstream oss;
            oss << "[" << str0 << "]";
            assert( oss.str() == "[]" );
        }
    }

    // assign - no reallocation (reuse)
    {
        BasicString<char> str{"Something big!Something big!Something big!Something big!Something big!"};
        assert( str.size() == 70 );

        size_t oldCap = str.capacity();
        str.assign("New World!");

        assert( str.size() == 10u );
        assert( str.capacity() >= str.size() );
        assert( str.capacity() == oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[New World!]" );
    }

    // assign - reallocation
    {
        BasicString<char> str{"Something"};
        assert( str.size() == 9 );

        size_t oldCap = str.capacity();
        str.assign("New World! Where no man has gone before!");

        assert( str.size() == 40u );
        assert( str.capacity() == str.size() );
        assert( str.capacity() > oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[New World! Where no man has gone before!]" );
    }

    // append (multiple), push_back
    {
        size_t oldCap = 0u;
        BasicString<char> str = "hello";

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
            assert( str.size() == 11u );
            assert( str.capacity() >= str.size() );
            assert( str.capacity() > oldCap );

            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[hello world]" );
        }
    }

    // clear
    {
        BasicString<char> str{"Something big!Something big!Something big!Something big!Something big!"};
        assert( str.size() == 70u );

        size_t oldCap = str.capacity();
        str.clear();

        assert( str.size() == 0u );
        assert( str.capacity() > str.size() );
        assert( str.capacity() == oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[]" );
    }

    // shrink_to_fit
    {
        BasicString<char> str{"Something big!Something big!Something big!Something big!Something big!"};
        assert( str.size() == 70u );

        size_t oldCap = str.capacity();

        str = "Hello";

        assert( str.size() == 5u );
        assert( str.capacity() > str.size() );
        assert( str.capacity() == oldCap );

        str.shrink_to_fit();
        assert( str.size() == 5u );
        assert( str.capacity() == str.size() );
        assert( str.capacity() < oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[Hello]" );
    }

    // substr
    {
        BasicString<char> str{"This is big and that is small!"};
        assert( str.size() == 30u );

        {
            BasicString<char> str1 = str.substr(0u, 11u);
            assert( str1.size() == 11u );

            std::ostringstream oss;
            oss << "[" << str1 << "]";
            assert( oss.str() == "[This is big]" );
        }

        {
            BasicString<char> str1 = str.substr(16u, 4u);
            assert( str1.size() == 4u );

            std::ostringstream oss;
            oss << "[" << str1 << "]";
            assert( oss.str() == "[that]" );
        }

        {
            BasicString<char> str1 = str.substr(16u, 9000u);
            assert( str1.size() == 14u );

            std::ostringstream oss;
            oss << "[" << str1 << "]";
            assert( oss.str() == "[that is small!]" );
        }

        {
            BasicString<char> str1 = str.substr(16u);
            assert( str1.size() == 14u );

            std::ostringstream oss;
            oss << "[" << str1 << "]";
            assert( oss.str() == "[that is small!]" );
        }
    }

    // replace - no realloc, replacement str size < replaced str size
    {
        BasicString<char> str{"This is a very bad world!"};

        assert( str.size() == 25u );
        size_t oldCap = str.capacity();

        str.replace(10u, 8u, "great");

        assert( str.size() == 22u );
        assert( str.capacity() == oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[This is a great world!]" );
    }

    // replace - no realloc, replacement str size == replaced str size
    {
        BasicString<char> str{"This is a very bad world!"};

        assert( str.size() == 25u );
        size_t oldCap = str.capacity();

        str.replace(10u, 8u, "mindless");

        assert( str.size() == 25u );
        assert( str.capacity() == oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[This is a mindless world!]" );
    }

    // replace - no realloc, replacement str size > replaced str size
    {
        BasicString<char> str{"This is a very bad world!"};
        str.reserve(30);

        assert( str.size() == 25u );
        assert( str.capacity() == 30u );
        size_t oldCap = str.capacity();

        str.replace(10u, 8u, "multi-faceted");

        assert( str.size() == 30u );
        assert( str.capacity() == oldCap );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[This is a multi-faceted world!]" );
    }

    // replace - realloc, replacement str size > replaced str size
    {
        BasicString<char> str{"This is a very bad world!"};
        str.shrink_to_fit();

        assert( str.size() == 25u );
        assert( str.capacity() == 25u );

        str.replace(10u, 8u, "multi-faceted");

        assert( str.size() == 30u );
        assert( str.capacity() == 30u );

        std::ostringstream oss;
        oss << "[" << str << "]";
        assert( oss.str() == "[This is a multi-faceted world!]" );
    }

    // erase
    {
        BasicString<char> str{"This is a very good world!"};
        str.shrink_to_fit();

        assert( str.size() == 26u );
        assert( str.capacity() == 26u );

        str.erase(10u, 5u);

        assert( str.size() == 21u );
        assert( str.capacity() == 26u );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[This is a good world!]" );
        }

        str.erase(9u);
        assert( str.size() == 9u );
        assert( str.capacity() == 26u );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[This is a]" );
        }

        str.erase(7u, 3000u);
        assert( str.size() == 7u );
        assert( str.capacity() == 26u );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[This is]" );
        }

        str.erase(0u);
        assert( str.size() == 0u );
        assert( str.capacity() == 26u );

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[]" );
        }
    }

    // insert
    {
        BasicString<char> str{"This is a world!"};
        str.shrink_to_fit();

        str.insert(10u, "very good ");

        {
            std::ostringstream oss;
            oss << "[" << str << "]";
            assert( oss.str() == "[This is a very good world!]" );
        }
    }

    // find
    {
        BasicString<char> str{"This is a very good world! That is true!"};

        {
            size_t r = str.find("is", 0u);
            assert( r == 2u );
        }

        {
            size_t r = str.find("is", 1u);
            assert( r == 2u );
        }

        {
            size_t r = str.find("is", 2u);
            assert( r == 2u );
        }

        {
            size_t r = str.find("is", 3u);
            assert( r == 5u );
        }

        {
            size_t r = str.find("is", 4u);
            assert( r == 5u );
        }

        {
            size_t r = str.find("is", 5u);
            assert( r == 5u );
        }

        {
            size_t r = str.find("is", 6u);
            assert( r == 32u );
        }

        {
            size_t r = str.find("is", 32u);
            assert( r == 32u );
        }

        {
            size_t r = str.find("is", 33u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("is", 38u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("is", 39u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("is", 40u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("is", 5000u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("This", 0u);
            assert( r == 0u );
        }

        {
            size_t r = str.find("This", 38u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("true!", 0u);
            assert( r == 35u );
        }

        {
            size_t r = str.find("true!", 35u);
            assert( r == 35u );
        }

        {
            size_t r = str.find("true!", 36u);
            assert( r == str.npos );
        }

        {
            size_t r = str.find("This is a very good world! That is true!", 0u);
            assert( r == 0u );
        }
    }

    std::cout << "PASSED" << std::endl;
}
