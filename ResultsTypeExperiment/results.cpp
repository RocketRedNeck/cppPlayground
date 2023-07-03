
#include <cstdint>
#include <cstdio>

#define CASE_STRING(x) case x: return #x
#define DEFAULT_STRING default: return "UNKNOWN"

/// @brief Basic enumeration of status for use with the \ref Result type
enum class StatusCode : uint32_t
{
    Ok, ///< All is well. Every user defined enumeration must provide this to be used by \ref Result
    Err ///< Something is wrong. User-defined enumerations should create variations with more details, but this is the minimum
};

const char* toString(const StatusCode& code)
{
    switch(code)
    {
        CASE_STRING(StatusCode::Ok);
        CASE_STRING(StatusCode::Err);
        DEFAULT_STRING;
    }
}

/// @brief A simple result encapsulation that provides status and/or value
/// @tparam PRIMITIVE_TYPE The value type to be represented in the result
/// @tparam STATUS_ENUM_TYPE The status type (i.e., enum type) to be represented in the result, must contain `Ok`
/// @details A simple result encapsulation that allows functions to package a result value with a result status
/// as a single object and allows the consumer to test and/or use as appropriate. The results can be
/// used as a local (stack) Boolean test before use, or the `isOk`, `value`, or `status` functions may
/// be called explicitly.
/// @attention the intention is to use this class on the stack to return a `value` and `status` together.
/// It is **NOT** recommended to use this type persistently, as that defeats the purpose of an ephemeral status
template <typename PRIMITIVE_TYPE, class STATUS_ENUM_TYPE = StatusCode>
class Result
{
public:
    using Result_t = Result<PRIMITIVE_TYPE, STATUS_ENUM_TYPE>;
    using Value_t = PRIMITIVE_TYPE;
    using Status_t = STATUS_ENUM_TYPE;

private:
    Value_t value_;
    Status_t status_;

public:
    /// @brief Constructor to pass back value with Ok status
    /// @param value the value being returned
    /// @details Contains the return `value` with status set to `STATUS_ENUM_TYPE::Ok`
    Result(const PRIMITIVE_TYPE& value)
        : value_(value)
        , status_(STATUS_ENUM_TYPE::Ok)
    {
    }

    /// @brief Constructor to pass back a status with a default value equivalent of 0
    /// @param status the status being returned
    /// @details Contains the return `status` with a default value of 0 (typed to `PRIMITIVE_TYPE`)
    Result(const STATUS_ENUM_TYPE& status)
        : value_(static_cast<PRIMITIVE_TYPE>(0))
        , status_(status)
    {   
    }

    /// @brief Assignment to pass back value with Ok status
    /// @param value the value being returned
    /// @return this object with the value and status set
    Result_t& operator=(const PRIMITIVE_TYPE& value)
    {
        value_ = value;
        status_ = STATUS_ENUM_TYPE::Ok;

        return *this;
    }

    /// @brief Assignment to pass back status with a default 0 value
    /// @param status the status being returned
    /// @return this object with the value and status set
    Result_t& operator=(const STATUS_ENUM_TYPE& status)
    {
        value_ = static_cast<PRIMITIVE_TYPE>(0);
        status_ = status;

        return *this;
    }

    /// @brief Returns `true` if result is okay to use
    /// @return `true` when `status` is `STATUS_ENUM_TYPE::Ok`, `false` otherwise
    bool isOk() const
    {
        return (STATUS_ENUM_TYPE::Ok == status_);
    }

    /// @brief Returns a copy of the `value`
    /// @return A copy of the `value`
    const PRIMITIVE_TYPE value() const
    {
        return value_;
    }

    /// @brief Returns a copy of the `value` if `status` is `Ok`, otherwise a user-defined default
    /// @param defaultValue The value to return when the `status` is **NOT** `Ok`
    /// @return Copy of `value` or `defaultValue`
    const PRIMITIVE_TYPE valueOr(const PRIMITIVE_TYPE& defaultValue)
    {
        if (isOk())
        {
            return value_;
        }
        else{
            return defaultValue;
        }
    }

    /// @brief Returns copy of `status`
    /// @return Copy of `status`
    STATUS_ENUM_TYPE status() const
    {
        return status_;
    }

    /// @brief Boolean type converter
    /// @details Allows a result to be branched on as-if it was a Boolean
    operator bool() const
    {
        return isOk();
    }

};

/// @brief Example user-defined status codes
enum class UserStatusCode : uint32_t
{
    Ok,
    Err,
    NewError,
    AnotherError
};
const char* toString(const UserStatusCode& code)
{
    switch(code)
    {
        CASE_STRING(UserStatusCode::Ok);
        CASE_STRING(UserStatusCode::Err);
        CASE_STRING(UserStatusCode::NewError);
        CASE_STRING(UserStatusCode::AnotherError);
        DEFAULT_STRING;
    }
}

/// @brief Example class that returns \ref Result
/// @tparam PRIMITIVE_TYPE A type to demonstrate the \ref Result type mutability
template <typename PRIMITIVE_TYPE>
class SomeClass
{
public:
    /// @brief Constructor
    SomeClass() = default;

    /// @brief Destructor
    ~SomeClass() = default;

    /// @brief Simplification of result type semantics
    using Result_t = Result<PRIMITIVE_TYPE,UserStatusCode>;

    /// @brief Demonstration function that returns a \ref Result
    /// @param x An input value to trigger a \ref Result response
    /// @return a \ref SomeClass::Result_ with either the value or status
    /// @details Status is `Err` for `x == 0`, `NewError` for `x == 123`, `Ok` otherwise
    Result_t f(const PRIMITIVE_TYPE& x)
    {
        Result_t retval(UserStatusCode::AnotherError);

        if (x)
        {
            if (static_cast<int>(x) == 123)
            {
                retval = UserStatusCode::NewError;
            }
            else if (x < static_cast<PRIMITIVE_TYPE>(0))
            {
                ; // nothing else to do
            }
            else
            {
                retval = x;;
            }
        }
        else
        {
            retval = UserStatusCode::Err;
        }

        return retval;
    }    
};

int main()
{
    printf("Hello, Results! %ld\n",__cplusplus);
    SomeClass<int> a;
    SomeClass<double> b;
    SomeClass<uint32_t> c;

    auto ar = a.f(0);
    if (ar)
    {
        printf("ar is valid\n");
    }
    else
    {
        printf("ar is invalid\n");
    }
    printf("ar: value = %d status = %s\n",ar.valueOr(24), toString(ar.status()));
    ar = a.f(1);
    if (ar)
    {
        printf("ar is valid\n");
    }
    else
    {
        printf("ar is invalid\n");
    }
    printf("ar: value = %d status = %s\n",ar.valueOr(24), toString(ar.status()));


    auto br = b.f(0.0);
    printf("br: value = %f status = %s\n",br.valueOr(42.0), toString(br.status()));
    br = b.f(1.0);
    printf("br: value = %f status = %s\n",br.valueOr(42.0), toString(br.status()));

    br = b.f(123.0);
    printf("br: value = %f status = %s\n",br.value(), toString(br.status()));

    decltype(c)::Result_t cr = c.f(12345);

    printf("cr: value = %u status = %s\n",cr.value(), toString(cr.status()));

    br = b.f(-1);
    printf("br: value = %f status = %s\n",br.value(), toString(br.status()));


    return 0;
}
