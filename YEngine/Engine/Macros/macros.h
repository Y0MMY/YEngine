// error exception helper macro
#include <cassert>
#include <Log/Log.h>

#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )

#define CHWND_ASSERT(expression) assert(expression)

using namespace std::string_literals;

#define LOG_INFO(text, ...)     { Log::log().Info ( ("[ "s + std::string(__FUNCTION__) + " ] "s + ": "s + std::string( text ) ).c_str(), __VA_ARGS__); }
#define LOG_DEBUG(text, ...)  { Log::log().Debug( (std::string(__FUNCTION__) + ": "s + std::string( text ) ).c_str(), __VA_ARGS__); }
#define LOG_ERROR(text, ...)    { Log::log().Err( (std::string(__FUNCTION__) + ": "s + std::string( text ) ).c_str(), __VA_ARGS__); }

#define noxnd noexcept (!IS_DEBUG)

#define MODELSURL "Engine\\assets\\models\\"