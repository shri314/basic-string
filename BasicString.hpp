#include <utility>
#include <stdexcept>
#include <type_traits>
#include <limits>
#include <algorithm>
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
    static const size_t npos = -1;

    BasicString() = default;

    BasicString(const CharT* buf, size_t sz)
        : BasicString()
    {
        append(buf, sz);
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
        : m_data(std::exchange(rhs.m_data, nullptr))
        , m_size(std::exchange(rhs.m_size, 0u))
        , m_capacity(std::exchange(rhs.m_capacity, 0u))
    {
    }

    friend void swap(BasicString& lhs, BasicString& rhs) noexcept
    {
        using std::swap;

        swap(lhs.m_size, rhs.m_size);
        swap(lhs.m_capacity, rhs.m_capacity);
        swap(lhs.m_data, rhs.m_data);
    }

    BasicString& assign(BasicString&& rhs)
    {
        using std::swap;

        BasicString tmp = std::move(rhs);
        swap(*this, tmp);
        return *this;
    }

    void clear()
    {
        // std::destroy(data(), data() + size());
        m_size = 0;
    }

    BasicString& assign(const CharT* buf, size_t sz)
    {
        auto assign_exsafe_ = [&]()
        {
            using std::swap;

            BasicString tmp{buf, sz};
            swap(*this, tmp);
        };

        if constexpr(!std::is_nothrow_constructible_v<CharT>)
        {
            assign_exsafe_();
        }
        else if(sz > capacity())
        {
            assign_exsafe_();
        }
        else
        {
            clear();
            append(buf, sz);
        }

        return *this;
    }

    BasicString& assign(const CharT* buf)
    {
        return assign(buf, strlen_(buf));
    }

    BasicString& assign(const BasicString& rhs)
    {
        return assign(rhs.data(), rhs.size());
    }

    //////////////////////////

    BasicString& operator=(const BasicString& rhs)
    {
        return assign(rhs.data(), rhs.size());
    }

    BasicString& operator=(const CharT* buf)
    {
        return assign(buf);
    }

    BasicString& operator=(BasicString&& rhs)
    {
        return assign(std::move(rhs));
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
            set_capacity_exsafe_(new_cap);
        }
    }

    void shrink_to_fit()
    {
        if(size() < capacity())
        {
            set_capacity_exsafe_(size());
        }
    }

    BasicString& append(const CharT* buf, size_t sz)
    {
        if(sz == 0) return *this; // quick return (minor optimization)

        if(sz > spare_capacity_())
        {
            size_t new_cap = std::max(
                                    add_sat_(size(), size()), // geometric progression
                                    add_sat_(size(), sz)      // arithmetic progression
                                );

            set_capacity_exsafe_(new_cap);
        }

        *std::copy(buf, buf + sz, data() + size()) = *ptr_to_null(); // std::uninitialized_copy
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

    BasicString& push_back(CharT ch)
    {
        return append(ch);
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
        assert(index < size());

        return m_data[index];
    }

    CharT& operator[](size_t index)
    {
        assert(index < size());

        return m_data[index];
    }

    const CharT& at(size_t index) const
    {
        if(index >= size())
            throw std::out_of_range{"bad index"};

        return m_data[index];
    }

    CharT& at(size_t index)
    {
        if(index >= size())
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

    BasicString substr(size_t index = 0, size_t count = npos) const
    {
        if(index >= size())
            throw std::out_of_range{"bad index"};

        return BasicString{data() + index, std::min(size() - index, count)};
    }

    BasicString& replace(size_t index, size_t count, const CharT* buf, size_t sz)
    {
        if(index >= size())
            throw std::out_of_range{"bad index"};

        size_t eff_count = std::min(size() - index, count);
        size_t new_size = add_sat_(size() - eff_count, sz);

        if(new_size > max_size())
            throw std::length_error("size too big");

        auto replace_exsafe_ = [&]()
        {
            using std::swap;

            BasicString tmp{reserve_t{new_size}};
            tmp.append(data(), index);
            tmp.append(buf, sz);
            tmp.append(data() + index + eff_count, size() - index - eff_count);

            swap(*this, tmp);
        };

        if constexpr (!std::is_nothrow_copy_assignable_v<CharT>
                   || !std::is_nothrow_move_assignable_v<CharT>
                   || !std::is_nothrow_constructible_v<CharT>)
        {
            replace_exsafe_();
        }
        else if(new_size > capacity())
        {
            replace_exsafe_();
        }
        else
        {
            if(new_size < size())
            {
                // slide left
                std::move(
                        data() + index + eff_count,
                        data() + size(),
                        data() + index + sz
                    );

                // std::destroy(data() + new_size, data() + size());

                *(data() + new_size) = *ptr_to_null(); // std::uninitialized_copy
            }
            else if(new_size > size())
            {
                size_t excess_size = new_size - size();

                // slide right (part1: uninitialized section)
                std::copy(
                        std::reverse_iterator<CharT*>(data() + size()),
                        std::reverse_iterator<CharT*>(data() + size() - excess_size),
                        std::reverse_iterator<CharT*>(data() + new_size)
                    ); // std::uninitialized_copy

                // slide right (part2: overwrite section)
                std::move(
                        std::reverse_iterator<CharT*>(data() + size() - excess_size),
                        std::reverse_iterator<CharT*>(data() + index + eff_count),
                        std::reverse_iterator<CharT*>(data() + size())
                    );

                *(data() + new_size) = *ptr_to_null(); // std::uninitialized_copy
            }

            // do the remaining replacement in-place
            std::copy(buf, buf + sz, data() + index); // std::copy assign
            m_size = new_size;
        }

        return *this;
    }

    BasicString& replace(size_t index, size_t count, const CharT* buf)
    {
        return replace(index, count, buf, strlen_(buf));
    }

    BasicString& erase(size_t index, size_t count = npos)
    {
        return replace(index, count, "");
    }

    BasicString& insert(size_t index, const CharT* buf, size_t sz)
    {
        return replace(index, 0, buf, sz);
    }

    BasicString& insert(size_t index, const CharT* buf)
    {
        return insert(index, buf, strlen_(buf));
    }

    BasicString& insert(size_t index, const BasicString& str)
    {
        return insert(index, str.data(), str.size());
    }

    size_t find(const CharT* buf, size_t index, size_t sz) const
    {
        // move to trait

        if(sz > size() || index > size() - sz)
            return npos;

        const CharT* beg = data() + index;
        const CharT* end = data() + size() - sz;
        while(true)
        {
            bool match = true;
            for(size_t i = 0u; i < sz; ++i)
            {
                if (!std::equal_to{}(*(beg + i), *(buf + i))) // move to trait
                {
                    match = false;
                    break;
                }
            }

            if(match)
            {
                return beg - data();
            }

            if(beg++ == end)
                break;
        }

        return npos;
    }

    size_t find(const CharT* buf, size_t index = 0) const
    {
        return find(buf, index, strlen_(buf));
    }

    size_t find(const BasicString& str, size_t index = 0) const
    {
        return find(str.data(), index, str.size());
    }

    //////////////////////////

private:
    struct reserve_t
    {
        explicit reserve_t(size_t value)
            : value(value)
        {
        }

        size_t value;
    };

    explicit BasicString(reserve_t rz)
    {
        m_data = new CharT[rz.value + 1]; // TODO: use allocator
        m_capacity = rz.value;
    }

    size_t spare_capacity_() const
    {
        return capacity() - size();
    }

    void set_capacity_exsafe_(size_t new_cap)
    {
        if(new_cap > max_size())
            throw std::length_error("size too big");

        using std::swap;

        BasicString tmp{reserve_t{new_cap}};
        tmp.append(*this);
        swap(tmp, *this);
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

using String = BasicString<char>;
using wString = BasicString<wchar_t>;
