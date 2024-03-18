#include <utility>
#include <stdexcept>
#include <type_traits>
#include <limits>
#include <cassert>

template<class T>
T add_sat_(T a, T b)
{
    //
    // Emulate C++26 - std::add_sat();
    // FIXME: incomplete impl - a poor man's version
    //
    static_assert( std::is_unsigned_v<T> );

    T res = a + b;
    if(res < a)
        res = static_cast<T>(-1);
    return res;
}

template<typename CharT>
class BasicString
{
public:
    BasicString() = default;

    BasicString(const CharT* buf, size_t sz)
        : BasicString(reserve_t{sz}, buf, sz)
    {
    }

    template<size_t N>
    BasicString(const CharT(&buf)[N])
        : BasicString(buf, N - 1)
    {
    }

    BasicString(const CharT* buf)
        : BasicString(buf, strlen_(buf))
    {
    }

    BasicString(const BasicString& rhs)
        : BasicString(rhs.m_data, rhs.m_size)
    {
    }

    BasicString(BasicString&& rhs) noexcept
        : BasicString()
    {
        using std::swap;

        swap(*this, rhs);
    }

    friend void swap(BasicString& lhs, BasicString& rhs) noexcept
    {
        using std::swap;

        swap(lhs.m_size, rhs.m_size);
        swap(lhs.m_capacity, rhs.m_capacity);
        swap(lhs.m_data, rhs.m_data);
    }

    BasicString& operator=(const BasicString& rhs)
    {
        BasicString tmp = rhs;

        swap(*this, tmp);
        return *this;
    }

    BasicString& operator=(BasicString&& rhs)
    {
        BasicString tmp = std::move(rhs);

        swap(*this, tmp);
        return *this;
    }

    ~BasicString()
    {
        delete[] m_data; // TODO: use allocator
    }

    //////////////////////////

    void reserve(size_t new_cap)
    {
        if(new_cap > capacity())
        {
            BasicString tmp{reserve_t{new_cap}, *this};

            swap(tmp, *this);
        }
    }

    BasicString& append(const CharT* buf, size_t sz)
    {
        if(sz > spare_capacity_())
        {
            size_t new_cap = std::max(
                                    add_sat_(size(), size()), // geometric progression
                                    add_sat_(size(), sz)      // linear progression
                                );

            if(new_cap > max_size())
                throw std::length_error("size too big");

            reserve( new_cap );
        }

        *std::copy(buf, buf + sz, data() + size()) = *ptr_to_null();
        m_size += sz;

        return *this;
    }

    BasicString& append(const BasicString& rhs)
    {
        return append(rhs.data(), rhs.size());
    }

    BasicString& append(CharT ch)
    {
        return append(std::addressof(ch), 1u);
    }

    //////////////////////////

    size_t size() const
    {
        return m_size;
    }

    size_t capacity() const
    {
        return m_capacity;
    }

    size_t max_size() const
    {
        return (std::numeric_limits<size_t>::max() / sizeof(CharT)) / 2u - sizeof(CharT);
    }

    bool empty() const
    {
        return size() == 0u;
    }

    //////////////////////////

    const CharT* data() const
    {
        return m_data != nullptr ? m_data : ptr_to_null();
    }

    CharT* data()
    {
        return m_data != nullptr ? m_data : ptr_to_null();
    }

    const CharT* c_str() const
    {
        return data();
    }

    const CharT& operator[](size_t index) const
    {
        assert(index < m_size);

        return m_data[index];
    }

    CharT& operator[](size_t index)
    {
        assert(index < m_size);

        return m_data[index];
    }

    const CharT& at(size_t index) const
    {
        if(index >= m_size)
            throw std::out_of_range{"bad index"};

        return m_data[index];
    }

    CharT& at(size_t index)
    {
        if(index >= m_size)
            throw std::out_of_range{"bad index"};

        return m_data[index];
    }

    const CharT& front() const
    {
        return (*this)[0];
    }

    CharT& front()
    {
        return (*this)[0];
    }

    const CharT& back() const
    {
        return (*this)[size() - 1];
    }

    CharT& back()
    {
        return (*this)[size() - 1];
    }

    //////////////////////////

private:
    struct reserve_t{ size_t sz; };

    explicit
    BasicString(reserve_t rz)
    {
        m_data = new CharT[rz.sz + 1]; // TODO: use allocator
        m_capacity = rz.sz;
    }

    explicit
    BasicString(reserve_t rz, const CharT* buf, size_t sz)
        : BasicString(rz)
    {
        append(buf, sz);
    }

    explicit
    BasicString(reserve_t rz, const BasicString& rhs)
        : BasicString(rz)
    {
        append(rhs);
    }

    size_t spare_capacity_() const
    {
        return capacity() - size();
    }

    static CharT* ptr_to_null()
    {
        static CharT ch = CharT{}; // TODO: use trait
        return std::addressof(ch);
    }

    static size_t strlen_(const CharT* buf)
    {
        const CharT* e = buf;
        while(*e != *ptr_to_null()) ++e;
        return e - buf;
    }

private:
    CharT* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
};


#include <ostream>

template<typename CharT>
std::ostream& operator<<(std::ostream& os, const BasicString<CharT>& rhs)
{
    for(size_t i = 0; i < rhs.size(); ++i)
    {
        os << rhs[i];
    }

    return os;
}
