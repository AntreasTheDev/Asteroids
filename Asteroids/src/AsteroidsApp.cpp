// Asteroids.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "Engine.h"
#include "Game.h"

void GameLoop(Engine* m_Engine);

int main()
{
    printf("[LOG] App Initialized \n");

    Engine* m_pEngine = new Engine();
   
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            if (SUCCEEDED(m_pEngine->Initialize()))
            {
                GameLoop(m_pEngine);
            }
        }
        CoUninitialize();
    }

    printf("[LOG] Shutting Down \n");
    delete m_pEngine;
    return 0;
}

void GameLoop(Engine* a_Engine)
{
    printf("[LOG] App Running \n");

    //Random seed that helps with Generating Asteroids
    srand(static_cast<unsigned int>(std::time(0))); 

    //Init Variables
    Game* m_pGame = new Game(a_Engine);

    MSG Message;   
    clock_t BeginFrame, EndFrame;
    double FPS = 0, MS = 0, DisplayTimer = 0;
    float DeltaTime = 0.1f;
    bool Quit = false;

    while (!Quit)
    {    

        //Start Timer
        BeginFrame = clock();

        //Dispatches a message in case of WM_CREATE, WM_SIZE, 
        //WM_DISPLAYCHANGE, WM_DESTROY
        if(PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
        {
            //Exit the GameLoop in case the window has closed
            if (Message.message == WM_QUIT)
            {
                Quit = true;
                break;
            }
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }
        
        //Update & Render
        m_pGame->Update(DeltaTime);

        if (SUCCEEDED(a_Engine->StartDraw()) && (m_pGame->GetGameState() != Hdata::EGameState::Quit))
        {
            a_Engine->DrawPlayer(m_pGame->m_pPlayer);
            a_Engine->DrawSaucer(m_pGame->m_pSaucer);
            a_Engine->DrawAsteroids(m_pGame->m_pAsteroids);
            a_Engine->DrawBullets(m_pGame->m_pBullets);
            a_Engine->DrawParticles(m_pGame->m_pParticles);
            a_Engine->DrawUI(m_pGame->GetScore(), m_pGame->GetRemainingLifes(),  m_pGame->GetGameState());
        }
        a_Engine->EndDraw();

        if (m_pGame->GetGameState() == Hdata::EGameState::Quit)
        {
            Quit = true;
        }

        //End Timer
        EndFrame = clock();

        //Calculate FPS and MS (MS/1000 = DeltaTime)
        MS = Hdata::ClockToMilliseconds(EndFrame) - Hdata::ClockToMilliseconds(BeginFrame);
        DeltaTime = static_cast<float>(MS / 1000);
        DisplayTimer += MS;

        if (MS > 0)
        {                   
            FPS = CLOCKS_PER_SEC / MS;
        }

        if (DeltaTime > 0.1)
        {
            DeltaTime = 0.1f;           //Ensures that the game still plays smoothly in case we move/resize the window 
        }

        if (DisplayTimer > 1000.0)      //Refresh window text every 1 second
        {
           
            //Display Info on Window
            WCHAR Frames[32];
            swprintf(Frames, 32, L"Asteroids - %.f fps, %.1f ms", FPS, MS);
            SetWindowText(a_Engine->GetHWND(), Frames);

            //Reset Display Timer
            DisplayTimer = 0;
        }
    }  

    delete m_pGame;
}

