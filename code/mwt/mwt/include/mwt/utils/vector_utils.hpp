#ifndef VECTOR_UTILS_HPP
#define VECTOR_UTILS_HPP
namespace utils::vector
{
    template <typename T>
    std::pair<bool, int> find_in_vector(const std::vector<T>  &search_vector, const T  &element)
    {
        std::pair<bool, int > result;

        // Find given element in vector
        auto it = std::find(search_vector.begin(), search_vector.end(), element);

        if (it != search_vector.end())
        {
            result.second = distance(search_vector.begin(), it);
            result.first = true;
        }
        else
        {
            result.first = false;
            result.second = -1;
        }

        return result;
    }

    template <typename T>
    bool has_and_remove_duplicates(std::vector<T> &vec)
    {
        auto it = std::unique(vec.begin(), vec.end());
        bool duplicates =  it != vec.end();

        vec.erase(it, vec.end());
        return duplicates;
    }

    template <typename T>
    void remove_duplicates(std::vector<T> &vec)
    {
        std::sort( vec.begin(), vec.end() );
        vec.erase( std::unique( vec.begin(), vec.end() ), vec.end() );
    }

    template <typename T>
    bool has_duplicates(std::vector<T> &vec, bool copy = true)
    {
        (void)copy; // supress unused-parameter warning
        std::sort(vec.begin(), vec.end());
        return std::adjacent_find(vec.begin(), vec.end()) != vec.end();
    }

    template<class ForwardIt, class T, class Compare=std::less<>>
    ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
    {
        // Note: BOTH type T and the type after ForwardIt is dereferenced
        // must be implicitly convertible to BOTH Type1 and Type2, used in Compare.
        // This is stricter than lower_bound requirement (see above)

        first = std::lower_bound(first, last, value, comp);
        return first != last && !comp(value, *first) ? first : last;
    }
}
#endif