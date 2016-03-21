#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#define SCREEN_W 1280
#define SCREEN_H 720
#define ZOOM 3.0f
#define ZOOM_W SCREEN_W/ZOOM
#define ZOOM_H SCREEN_H/ZOOM
#define ZOOM_WH ZOOM_W/2
#define ZOOM_HH ZOOM_H/2
#define ZOOM_WQ ZOOM_WH/2
#define ZOOM_HQ ZOOM_HH/2

#define WALK_DISTANCE 2.0f
#define DIALOGUE_DISTANCE 50

using uint = unsigned int;

enum : uint { USE_DEFAULT, USE_WALK, USE_SPEECH };
enum : uint { CURSOR_DEFAULT, CURSOR_USEABLE, CURSOR_WALK, CURSOR_SPEECH };


using SpeechID = uint;




#endif // _COMMON_HPP_
