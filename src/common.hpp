#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#define SCREEN_W 1280
#define SCREEN_H 720
#define ZOOM 3.0f
#define ZOOM_W SCREEN_W/ZOOM
#define ZOOM_H SCREEN_H/ZOOM

using uint = unsigned int;

enum : uint { MODE_CREATE, MODE_PLAY };

enum : uint { USE_DEFAULT, USE_WALK, USE_SPEECH };


using SpeechID = uint;




#endif // _COMMON_HPP_
