#include <iostream>
#include <map>
#include <string>

enum eStatus
{
    _100_CONTINUE = 100,
    _200_OK = 200,
    _201_CREATED,
    _202_ACCEPTED,
    _204_NO_CONTENT = 204,
    _301_MOVED_PERMANENTLY = 301,
    _302_FOUND,
    _303_SEE_OTHER,
    _304_NOT_MODIFIED,
    _307_TEMPORARY_REDIRECT = 307,
    _308_PERMANENT_REDIRECT,
    _400_BAD_REQUEST = 400,
    _401_UNAUTHORIZED,
    _403_FORBIDDEN = 403,
    _404_NOT_FOUND,
    _405_METHOD_NOT_ALLOWED,
    _406_NOT_ACCEPTABLE,
    _408_REQUEST_TIMEOUT = 408,
    _409_CONFLICT,
    _413_REQUEST_ENTITY_TOO_LARGE = 413,
    _414_URI_TOO_LONG,
    _415_UNSUPPORTED_MEDIA_TYPE,
    _417_EXPECTION_FAILED = 417,
    _418_IM_A_TEAPOT,
    _500_INTERNAL_SERVER_ERROR = 500,
    _501_NOT_IMPLEMENTED,
    _502_BAD_GATEWAY,
    _503_SERVICE_UNAVAILABLE,
    _504_GATEWAY_TIMEOUT,
    _505_HTTP_VERSION_NOT_SUPPORTED
};

int main()
{
    int statuscode = 301;
    eStatus _code;

    _code = static_cast<eStatus>(statuscode);

    std::cout << _code << std::endl;
    return 0;
}
