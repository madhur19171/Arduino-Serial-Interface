// SerialApplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifndef _WaveIOHeader_
#include "WavIO.h"
#endif // !1

#pragma once
#include <iostream>
#include <windows.h>
#include "plplot/plstream.h"
#include "FFT.h"
#include <cmath>
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

    dcbSerialParams.BaudRate = 2000000;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        printf("Error in Setting new Communication Settings\n");
    }

    const int n = (1 << 14);
    const int buffSize = (1 << 10);

    PLFLT* data = new PLFLT[n / 2];
    PLFLT* xaxis = new PLFLT[n / 2];


    short* fftData_in = new short[n / 2];
    short* fftTemp_in = new short[(1 << rep_bits)];
    PLFLT* fftData_out_real = new PLFLT[n / 2];
    PLFLT* fftTemp_out_real = new PLFLT[(1 << rep_bits)];
    PLFLT* fftData_out_imag = new PLFLT[n / 2];
    PLFLT* fftTemp_out_imag = new PLFLT[(1 << rep_bits)];
    PLFLT* fftData_out_abs = new PLFLT[n / 2];

    byte* byte_buffer = new byte[n];

    PLFLT xmin = 0, ymin = 0, xmax = n / 2, ymax = 1024;

    byte szBuff[buffSize];
    DWORD dwBytesRead = 0;

    byte lower, higher;

    vector <short> wavWrite;
    int samplingRate = 90000, bitsPerSample = 16, audioLength = 60, numberChannels = 1;
    int whileCount = 0, whileTill = audioLength * samplingRate / n * 2;

    plspage(100, 100, 2000, 700, 0, 0);
    plsdev("wingcc");
    plsetopt("np", "");
    plinit();
    plssub(1, 2);

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

    while (whileCount++ < whileTill) {
        for (int i = 0; i < n / buffSize; i++) {
            if (!ReadFile(hComm, szBuff, buffSize, &dwBytesRead, NULL)) {
                printf("Error in Reading data\n");
            }
            for (int j = 0; j < buffSize; j++) {
                byte_buffer[i * buffSize + j] = szBuff[j];
            }
        }

        short temp_data, last = 0;
        for (int i = 0; i < n && i + 1 < n; i = i + 2) {
            higher = byte_buffer[i];
            lower = byte_buffer[i + 1];
            //Parity Protocol
            if (higher % 2 == 0 && lower % 2 == 1) {
                temp_data = ((higher & 0B11111000) << 2) | ((lower & 0B11111000) >> 3);
                xaxis[i / 2] = i / 2;
                last = temp_data;
            }
            else {
                temp_data = last;
                i--;
            }
            wavWrite.push_back(temp_data * 32);
            data[i / 2] = temp_data;
        }

        for (int i = 0; i < (n / 2) / (1 << rep_bits); i++) {
            for (int j = 0; j < (1 << rep_bits); j++) {
                fftTemp_in[j] = (short)data[i * (1 << rep_bits) + j];
            }

            FFT(fftTemp_in, fftTemp_out_real, fftTemp_out_imag);

            for (int j = 0; j < (1 << rep_bits); j++) {
                fftData_out_real[i * (1 << rep_bits) + j] = fftTemp_out_real[j];
                fftData_out_imag[i * (1 << rep_bits) + j] = fftTemp_out_imag[j];
                fftData_out_abs[i * (1 << rep_bits) + j] = sqrt(fftTemp_out_real[j] * fftTemp_out_real[j] + fftTemp_out_imag[j] * fftTemp_out_imag[j]);
            }
        }

        pladv(1);
        plenv0(xmin, xmax, ymin, ymax, 0, 0);
        pllab("Points", "Amplitude", "Plotting Serial Data");
        plline(n / 2, xaxis, data);

        pladv(2);
        plenv0(xmin, xmax, 0, 25000, 0, 0);
        pllab("Points", "FFT", "Plotting FFT of Serial Data");
        plline(n / 2, xaxis, fftData_out_abs);
    }
    plend();
    CloseHandle(hComm);

    char name[] = "audio_input.wav";
    //WaveIO(name, numberChannels, samplingRate, bitsPerSample, wavWrite);

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
