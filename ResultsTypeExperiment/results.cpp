
#include <cstdio>
#include <cstdint>

enum class StatusCode
{
    Ok,
    Err
};


template <typename PRIMITIVE_TYPE, class STATUS_TYPE = StatusCode>
class Result
{
public:
    using Value_t = PRIMITIVE_TYPE;
    using Status_t = STATUS_TYPE;

private:
    Value_t value_;
    Status_t status_;

public:
    Result(const PRIMITIVE_TYPE& value)
        : value_(value)
        , status_(STATUS_TYPE::Ok)
    {
    }

    Result(const STATUS_TYPE& status)
        : value_(static_cast<PRIMITIVE_TYPE>(0))
        , status_(status)
    {   
    }

    bool isOk() const
    {
        return (STATUS_TYPE::Ok == status_);
    }

    const PRIMITIVE_TYPE value() const
    {
        return value_;
    }

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

    STATUS_TYPE status() const
    {
        return status_;
    }

    operator bool() const
    {
        return isOk();
    }
};

enum class ExtendedResultCode 
{
    Ok,
    Err,
    NewError,
    AnotherError
};

template <typename PRIMITIVE_TYPE>
class SomeClass
{
public:
    SomeClass() = default;

    using Result_t = Result<PRIMITIVE_TYPE,ExtendedResultCode>;

    Result_t f(const PRIMITIVE_TYPE& x)
    {
        if (x)
        {
            if (static_cast<int>(x) == 123)
            {
                return Result_t(ExtendedResultCode::NewError);
            }
            else
            {
                return Result_t(x);
            }
        }
        else
        {
            return Result_t(ExtendedResultCode::Err);
        }
    }    
};

int main()
{
    printf("Hello, Results!\n");
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
    printf("ar: value = %d status = %d\n",ar.valueOr(24), ar.status());
    ar = a.f(1);
    if (ar)
    {
        printf("ar is valid\n");
    }
    else
    {
        printf("ar is invalid\n");
    }
    printf("ar: value = %d status = %d\n",ar.valueOr(24), ar.status());


    auto br = b.f(0.0);
    printf("br: value = %f status = %d\n",br.valueOr(42.0), br.status());
    br = b.f(1.0);
    printf("br: value = %f status = %d\n",br.valueOr(42.0), br.status());

    br = b.f(123.0);
    printf("br: value = %f status = %d\n",br.value(), br.status());

    decltype(c)::Result_t cr = c.f(12345);

    printf("cr: value = %u status = %d\n",cr.value(), cr.status());



    return 0;
}
