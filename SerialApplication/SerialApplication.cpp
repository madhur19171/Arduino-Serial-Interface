// SerialApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <stdio.h>
#include "plplot/plstream.h"
using namespace std;



int main()
{
    std::cout << "Hello World!\n";
    HANDLE hComm;
    hComm = CreateFileA("\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE)
        printf("Error in opening Serial Port\n");
    else
        printf("Successfully opened Serial Port\n");

    DCB dcbSerialParams = { 0 };

    if (!GetCommState(hComm, &dcbSerialParams)) {
        //error getting state
        printf("Error in Getting Communication State\n");
    }

    dcbSerialParams.BaudRate = 1000000;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        printf("Error in Setting new Communication Settings\n");
    }

    const int n = 20000;
    const int buffSize = 1000;

    PLFLT* data = new PLFLT[n / 2];
    PLFLT* xaxis = new PLFLT[n / 2];

    byte* byte_buffer = new byte[n];

    PLFLT xmin = 0, ymin = 0, xmax = n / 2, ymax = 1024;

    byte szBuff[buffSize];
    DWORD dwBytesRead = 0;

    byte lower, higher;
    int last = 0;

    plspage(100, 100, 2000, 700, 0, 0);
    plsdev("wingcc");
    plsetopt("np", "");
    plinit();

    //To Implementing Parity Protocol during startup
    byte init_buffer;
    if (!ReadFile(hComm, &init_buffer, 1, &dwBytesRead, NULL)) {
        printf("Error in Reading init data\n");
    }
    if (init_buffer % 2 == 1)
        if (!ReadFile(hComm, &init_buffer, 1, &dwBytesRead, NULL)) {
            //error occurred. Report to user.
            printf("Error in setting up Parity Protocol\n");
        }

    while (1) {
        for (int i = 0; i < n / buffSize; i++) {
            if (!ReadFile(hComm, szBuff, buffSize, &dwBytesRead, NULL)) {
                printf("Error in Reading data\n");
            }
            for (int j = 0; j < buffSize; j++) {
                byte_buffer[i * buffSize + j] = szBuff[j];
            }
        }


        for (int i = 0; i < n && i + 1 < n; i = i + 2) {
            higher = byte_buffer[i];
            lower = byte_buffer[i + 1];
            //Parity Protocol
            if (higher % 2 == 0 && lower % 2 == 1) {
                data[i / 2] = ((higher & 0B11111000) << 2) | ((lower & 0B11111000) >> 3);
                xaxis[i / 2] = i / 2;
                last = data[i / 2];
            }
            else {
                data[i / 2] = 0;
                i--;
            }
        }

        plenv(xmin, xmax, ymin, ymax, 0, 0);
        pllab("Points", "Amplitude", "Plotting Serial Data");
        plline(n / 2, xaxis, data);
    }

    plend();
    CloseHandle(hComm);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
