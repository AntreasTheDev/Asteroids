#include "Engine.h"
#include "Player.h"
#include "Asteroid.h"
#include "Saucer.h"
#include <iostream>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

Engine::Engine() :
	m_pWhiteBrush(nullptr),     m_Hwnd(nullptr), 
	m_pBlueBrush(nullptr),      m_pDWriteFactory(nullptr),
	m_pYellowBrush(nullptr),    m_pRedBrush(nullptr),
	m_pGreenBrush(nullptr),     m_pDirect2dFactory(nullptr),
    m_pRenderTarget(nullptr),   m_pTextFormat(nullptr)
{
    printf("[LOG] Engine Constructed \n");
}

Engine::~Engine()
{   
    SafeRelease(&m_pWhiteBrush);
    SafeRelease(&m_pBlueBrush);
    SafeRelease(&m_pYellowBrush);
    SafeRelease(&m_pGreenBrush);
    SafeRelease(&m_pRedBrush);
    SafeRelease(&m_pTextFormat);
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
}

HRESULT Engine::Initialize()
{
    printf("[LOG] Initializing Engine \n");

    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Engine::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"AsteroidsApplication";



        RegisterClassEx(&wcex);

        // Create the application window.
        // In terms of using the correct DPI, to create a window at a specific size
        // like this, the procedure is to first create the window hidden. Then we get
        // the actual DPI from the HWND (which will be assigned by whichever monitor
        // the window is created on). Then we use SetWindowPos to resize it to the
        // correct DPI-scaled size, then we use ShowWindow to show it.

        m_Hwnd = CreateWindow(
            L"AsteroidsApplication",
            L"Asteroids",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<int>(Hdata::ScreenWidth),
            static_cast<int>(Hdata::ScreenHeight),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this);

        if (m_Hwnd)
        {
            SetWindowPos(
                m_Hwnd,
                NULL,
                NULL,
                NULL,
                static_cast<int>(Hdata::ScreenWidth),
                static_cast<int>(Hdata::ScreenHeight),
                SWP_NOMOVE);
            ShowWindow(m_Hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_Hwnd);
            hr = CreateDeviceResources();
        }
        else
        {
            printf("[ERROR] Failed to Initialize Window \n");
        }
        printf("[LOG] Engine Initialized \n");
    }
    else
    {
        printf("[ERROR] Failed to Initialize Engine \n");
    }

    return hr;
}

HRESULT Engine::StartDraw()
{
    HRESULT Result = S_OK;

    Result = CreateDeviceResources();

    if (SUCCEEDED(Result))
    {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    }
    else if (Result == D2DERR_RECREATE_TARGET)
    {
        Result = S_OK;
        DiscardDeviceResources();
    }

    return Result;
}

void Engine::DrawPlayer(Player* a_Player)
{
    //Does Player Exist and is it Active
    if ((a_Player) && ((a_Player->GetState() == GameObject::EObjectState::Active) || (a_Player->GetState() == GameObject::EObjectState::Invulnerable)))
    {
        //Connect all Ship Vertices
        for (size_t i = 0; i < a_Player->GetVertices().size() - 1; i++)
        {
            m_pRenderTarget->DrawLine(
                a_Player->GetVertices().at(i),
                a_Player->GetVertices().at(i + 1),
                a_Player->GetBrush(),
                0.5f);
        }
        //Connects Last with First Vertex
        m_pRenderTarget->DrawLine(
            a_Player->GetVertices().at(a_Player->GetVertices().size() - 1),
            a_Player->GetVertices().at(0),
            a_Player->GetBrush(),
            0.5f);

        //Draw Thrust when accelerating/moving
        if (a_Player->GetThrustOn())
        {
            //Connect all Thrust/Acceleration Vertices
            for (size_t i = 0; i < a_Player->GetThrustVertices().size() - 1; i++)
            {
                m_pRenderTarget->DrawLine(
                    a_Player->GetThrustVertices().at(i),
                    a_Player->GetThrustVertices().at(i + 1),
                    a_Player->GetBrush(),
                    0.5f);
            }
            //Connects Last with First Vertex
            m_pRenderTarget->DrawLine(
                a_Player->GetThrustVertices().at(a_Player->GetThrustVertices().size() - 1),
                a_Player->GetThrustVertices().at(0),
                a_Player->GetBrush(),
                0.5f);
        }
    }
}

void Engine::DrawSaucer(Saucer* a_Saucer)
{
    //Does Player Exist and is it Active
    if ((a_Saucer) && (a_Saucer->GetState() == GameObject::EObjectState::Active))
    {
        //Connect all Ship Vertices
        for (size_t i = 0; i < a_Saucer->GetVertices().size() - 1; i++)
        {
            m_pRenderTarget->DrawLine(
                a_Saucer->GetVertices().at(i),
                a_Saucer->GetVertices().at(i + 1),
                a_Saucer->GetBrush(),
                0.5f);
        }
        //Connects Last with First Vertex
        m_pRenderTarget->DrawLine(
            a_Saucer->GetVertices().at(a_Saucer->GetVertices().size() - 1),
            a_Saucer->GetVertices().at(0),
            a_Saucer->GetBrush(),
            0.5f);
    }
}

void Engine::DrawAsteroids(Asteroid* a_Asteroids[])
{
    //Go over all Asteroids
    for (int index = 0; index < Hdata::MaxAsteroids; index++)
    {
        //Does Asteroid Exist and is it Active
        if ((a_Asteroids[index]) && (a_Asteroids[index]->GetState() == GameObject::EObjectState::Active))
        {
            //Connect all Vertices
            for (size_t i = 0; i < a_Asteroids[index]->GetVertices().size() - 1; i++)
            {
                m_pRenderTarget->DrawLine(
                    a_Asteroids[index]->GetVertices().at(i),
                    a_Asteroids[index]->GetVertices().at(i + 1),
                    a_Asteroids[index]->GetBrush(),
                    0.5f);
            }
            //Connects Last with First Vertex
            m_pRenderTarget->DrawLine(
                a_Asteroids[index]->GetVertices().at(a_Asteroids[index]->GetVertices().size() - 1),
                a_Asteroids[index]->GetVertices().at(0),
                a_Asteroids[index]->GetBrush(),
                0.5f);
        }
    }
}

void Engine::DrawBullets(Bullet* a_Bullets[])
{
    //Go over all Bullets
    for (int index = 0; index < Hdata::MaxBullets; index++)
    {
        //Does Bullet Exist and is it Active
        if ((a_Bullets[index]) && (a_Bullets[index]->GetState() == GameObject::EObjectState::Active))
        {
            //Connect all Vertices
            for (size_t i = 0; i < a_Bullets[index]->GetVertices().size() - 1; i++)
            {
                m_pRenderTarget->DrawLine(
                    a_Bullets[index]->GetVertices().at(i),
                    a_Bullets[index]->GetVertices().at(i + 1),
                    a_Bullets[index]->GetBrush(),
                    0.5f);
            }
            //Connects Last with First Vertex
            m_pRenderTarget->DrawLine(
                a_Bullets[index]->GetVertices().at(a_Bullets[index]->GetVertices().size() - 1),
                a_Bullets[index]->GetVertices().at(0),
                a_Bullets[index]->GetBrush(),
                0.5f);
        }
    }
}

void Engine::DrawParticles(Particle* a_Particles[])
{
    //Go over all Particles
    for (int index = 0; index < Hdata::MaxParticles; index++)
    {
        //Does Particle Exist (Particle exists only in Active State)
        if (a_Particles[index])
        {
            //Connect all Vertices
            for (size_t i = 0; i < a_Particles[index]->GetVertices().size() - 1; i++)
            {
                m_pRenderTarget->DrawLine(
                    a_Particles[index]->GetVertices().at(i),
                    a_Particles[index]->GetVertices().at(i + 1),
                    a_Particles[index]->GetBrush(),
                    0.5f);
            }
            //Connects Last with First Vertex
            m_pRenderTarget->DrawLine(
                a_Particles[index]->GetVertices().at(a_Particles[index]->GetVertices().size() - 1),
                a_Particles[index]->GetVertices().at(0),
                a_Particles[index]->GetBrush(),
                0.5f);
        }
    }
}

void Engine::DrawUI(int a_Score, int a_Lifes, Hdata::EGameState a_GameState)
{
    //Shown at all times according to original game
    DrawScore(a_Score);
    DrawLifes(a_Lifes);

    //Create a Rectangle at the center of the screen
    float Offset = 200;
    float Width = static_cast<float>(Hdata::ScreenWidth) / 2.f;
    float Height = static_cast<float>(Hdata::ScreenHeight) / 2.f;

    D2D1_RECT_F Rectangle = D2D1::RectF(Width - Offset, Height - Offset, Width + Offset, Height + Offset);

    switch (a_GameState)
    {
    case Hdata::EGameState::Paused:
    {
        m_pRenderTarget->DrawText(
            L"Game Paused \n \n Press Space to Restart \n Press Esc to Continue",
            63,
            m_pTextFormat,
            Rectangle,
            m_pWhiteBrush
        );
        break;
    }
    case Hdata::EGameState::Won:
    {
        m_pRenderTarget->DrawText(
            L"You Win! \n \n Press Space to Restart \n Press Esc to Exit ",
            57,
            m_pTextFormat,
            Rectangle,
            m_pWhiteBrush
        );
        break;
    }
    case Hdata::EGameState::Lost:
    {
        m_pRenderTarget->DrawText(
            L"You Lose! \n \n Press Space to Restart \n Press Esc to Exit",
            57,
            m_pTextFormat,
            Rectangle,
            m_pWhiteBrush
        );
        break;
    }

    }
}

void Engine::DrawScore(int a_Score)
{
    //Create Rectangle at the top left corner of the window
    D2D1_RECT_F Rectangle = D2D1::RectF(0, 0, Hdata::ScreenWidth, 50);

    //Create a WCHAR* of "Score: value of a_Score"
    std::string Text = "Score: " + std::to_string(a_Score);
    std::wstring Widestr = std::wstring(Text.begin(), Text.end());
    const wchar_t* Widecstr = Widestr.c_str();

    m_pRenderTarget->DrawText(   
        Widecstr,
        static_cast<UINT32>(Text.length()),
        m_pTextFormat,
        Rectangle,
        m_pWhiteBrush
    );

    //Delete pointer created
    Widecstr = nullptr;
    delete Widecstr;
}

void Engine::DrawLifes(int a_Lifes)
{
    //Create Rectangle below Score Rectangle
    D2D1_RECT_F Rectangle = D2D1::RectF(0, 0, Hdata::ScreenWidth, 100);

    //Create a WCHAR* of "Score: value of a_Score"
    std::string Text = "Lifes: " + std::to_string(a_Lifes);
    std::wstring Widestr = std::wstring(Text.begin(), Text.end());
    const wchar_t* Widecstr = Widestr.c_str();

    m_pRenderTarget->DrawText(
        Widecstr,
        static_cast<UINT32>(Text.length()),
        m_pTextFormat,
        Rectangle,
        m_pWhiteBrush,
        D2D1_DRAW_TEXT_OPTIONS_CLIP
    );

    //Delete pointer created
    Widecstr = nullptr;
    delete Widecstr;
}

HRESULT Engine::EndDraw()
{
    HRESULT hr = S_OK;

    hr = m_pRenderTarget->EndDraw();

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

LRESULT Engine::WndProc(HWND a_Hwnd, UINT a_Message, WPARAM a_wParam, LPARAM a_lParam)
{
    LRESULT result = 0;

    if (a_Message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)a_lParam;
        Engine* pEngine = (Engine*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            a_Hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pEngine)
        );

        result = 1;
    }
    else
    {
        Engine* pEngine = reinterpret_cast<Engine*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                a_Hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pEngine)
        {
            switch (a_Message)
            {
            /*case WM_SIZE:
            {
                UINT width = LOWORD(a_lParam);
                UINT height = HIWORD(a_lParam);
                pEngine->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;*/

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(a_Hwnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(a_Hwnd, a_Message, a_wParam, a_lParam);
        }
    }

    return result;
}

HRESULT Engine::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    if (SUCCEEDED(hr))
    {
        printf("[LOG] Created D2D1 Factory \n");

        // Initialize text writing factory
        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(m_pDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
        );

        if (SUCCEEDED(hr))
        {
            printf("[LOG] Created DWrite Factory \n");

            //Create Text Format
            hr = m_pDWriteFactory->CreateTextFormat(
                L"Verdana",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                25,
                L"", //locale
                &m_pTextFormat
            );

            if (SUCCEEDED(hr))
            {
                printf("[LOG] Created Text Format \n");

                //Set Text and Paragraph Alignment
                m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
            else
            {
                printf("[ERROR] Failed to Create TextFormat \n");
            }
        }
        else
        {
            printf("[ERROR] Failed to Create DWriteFactory \n");
        }
    }
    else
    {
        printf("[ERROR] Failed to Create D2D1Factory \n");
    }
   

    return hr;
}

HRESULT Engine::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    // Check if we already have a Render Target assigned
    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_Hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_Hwnd, size),
            &m_pRenderTarget
        );

        if (SUCCEEDED(hr))
        {
            hr = InitBrushes(hr);
        }
        else
        {
            printf("[ERROR] Couldn't Initialize Brushes \n");
        }
    }

    return hr;
}

void Engine::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pWhiteBrush);
    SafeRelease(&m_pBlueBrush);
    SafeRelease(&m_pYellowBrush);
    SafeRelease(&m_pRedBrush);
    SafeRelease(&m_pGreenBrush);
}

void Engine::OnResize(UINT a_NewWidth, UINT a_NewHeight)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(a_NewWidth, a_NewHeight));
    }
}

HRESULT& Engine::InitBrushes(HRESULT& a_hr)
{
    //White
    a_hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &m_pWhiteBrush
    );
    

	//Blue
    if (SUCCEEDED(a_hr))
    {
        a_hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Blue),
            &m_pBlueBrush
        );
    }

    //Yellow
    if (SUCCEEDED(a_hr))
    {
        a_hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Yellow),
            &m_pYellowBrush
        );
    }

    //Red
    if (SUCCEEDED(a_hr))
    {
        a_hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Red),
            &m_pRedBrush
        );
    }

    //Green
    if (SUCCEEDED(a_hr))
    {
        a_hr = m_pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Green),
            &m_pGreenBrush
        );
    }

    return a_hr;
}

const HWND& Engine::GetHWND() const
{
    return m_Hwnd;
}

ID2D1SolidColorBrush* Engine::GetBrush(Hdata::EBrushColor a_Color) const
{
    switch (a_Color)
    {
    case Hdata::EBrushColor::White:
        return m_pWhiteBrush;
        break;
    case Hdata::EBrushColor::Blue:
        return m_pBlueBrush;
        break;
    case Hdata::EBrushColor::Yellow:
        return m_pYellowBrush;
        break;
    case Hdata::EBrushColor::Red:
        return m_pRedBrush;
        break;
    case Hdata::EBrushColor::Green:
        return m_pGreenBrush;
        break;
    default:
        return m_pWhiteBrush;
        break;
    }
}





