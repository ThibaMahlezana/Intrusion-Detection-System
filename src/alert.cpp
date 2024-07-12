#include <iostream>
#include "alert.h"
#include "irrKlang.h"
#include <windows.h>
#include <psapi.h>
#include <wininet.h>

#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif

using namespace irrklang;

#pragma comment(lib, "irrKlang.lib")
#pragma comment(lib, "Wininet.lib")

Alert::Alert()
{
}

Alert::~Alert() {}

int Alert::soundBuzzer()
{
    std::cout << "[INFO] Sounding alarm ..." << std::endl;
    ISoundEngine *engine = createIrrKlangDevice();

    if (!engine)
        std::cout << "[ERROR] Could not startup engine" << std::endl;

    do
    {
        std::cout << "[INFO] Press 'q' to quit." << std::endl;
        engine->play2D("../sounds/buzz.wav", true);
    } while (getch() != 'q');

    engine->drop(); // delete engine
    std::cout << "[INFO] Alarm ended." << std::endl;
    return 0;
}

void Alert::checkInternetConnection()
{
    std::cout << "[INFO] Checking internet conncetion..." << std::endl;
    char url[256];
    strcat(url, "http://170.187.134.184");
    bool isconnect = InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
    if (isconnect)
        std::cout << "[INFO] Internet is Connected" << std::endl;

    else
        std::cout << "[INFO] Internet is not connected!!" << std::endl;
}