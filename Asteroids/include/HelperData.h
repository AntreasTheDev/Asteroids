/*
* Header file which purpose is to act as a centralized location for 
* commonly-used headers, variables, structs, enums and templates.
* 
* These could have been split up in their own header files respectively 
* (e.g., pch, EngineData, GameData etc.), however this is a 1-man project, 
* so scalability isn't a high priority in this case.  
*/


/*
*  HEADERS
*/   
#pragma once

// Windows Header Files
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
//https://docs.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>
#include <vector>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <ctime>
#include <string>
#include <sstream>
#include <chrono>

/*
*  Definitions
*/

// Excludes multiple rarely-used headers.
// More details can be found by accessing <windows.h>.
#define WIN32_LEAN_AND_MEAN 

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

/*
*  DATA
*/
//Header Data (Hdata) namespace created to avoid 
//accessing these data unintentionally. 
namespace Hdata
{
    const UINT ScreenWidth = 1280;
    const UINT ScreenHeight = 720;
    const int MaxWaves = 5;
    const int StartingLifes = 3;
    const int AsteroidVertices = 12;                            //Number of vertices per Asteroid
    const int BulletVertices = 2;                               //Number of vertices per Bullet
    const int AsteroidsPerWave = 4;                             //Current Wave * Asteroids Per Wave = Amount of Asteroids
    const int MaxAsteroids = MaxWaves * AsteroidsPerWave * 7;   //e.g., 3 * 5 * (1 + 2 + 4 (=7)) [1 Asteroid takes 7 spaces due it spliting]
    const int MaxBullets = 100;
    const int MaxParticles = 100;
    const float PI = 3.14159265f;

    struct Vec2
    {
        float m_X = 0.f;
        float m_Y = 0.f;
    };  //Simple 2D Vector

    struct InputAction
    {
        bool m_Move     = false;        //'W' Key (Pressed = True)     
        bool m_Shoot    = false;        //'Space' Key (Pressed = True) 
        bool m_LRotate  = false;        //'A' Key (Pressed = True) 
        bool m_RRotate  = false;        //'D' Key (Pressed = True) 
        bool m_Pause    = false;        //'Esc' Key (Pressed = True) 
    };  //Holds all keybinding actions

    enum class EGameState
    {
        Running = 0,
        Paused,
        Won,
        Lost,
        Quit
    };  //Holds all Game States

    enum class EBrushColor
    {
        White = 0,
        Blue,
        Yellow,
        Red,
        Green
    };  //Holds various colors. Can be expanded if needed.

    //Units/(Units/Time) => Time (seconds) * 1000 = milliseconds
    inline double ClockToMilliseconds(clock_t a_Ticks) 
    {
        return (a_Ticks / (double)CLOCKS_PER_SEC) * 1000.0;
    }

    //Generates a random number between a_Min and a_Max
    inline float RandomNumber(float a_Min, float a_Max)
    {
        return ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (a_Max - a_Min)) + a_Min;
    }
}

/*
*  TEMPLATES
*/
template<class Interface>
inline void SafeRelease(Interface** a_ppInterfaceToRelease)
{
    if (*a_ppInterfaceToRelease != NULL)
    {
        (*a_ppInterfaceToRelease)->Release();

        (*a_ppInterfaceToRelease) = NULL;
    }
}