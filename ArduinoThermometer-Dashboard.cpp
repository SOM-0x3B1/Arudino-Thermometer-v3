#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include "SimpleSerial.h"
#include <math.h>
#include <ctime> 
#include <fstream>

HANDLE hConsole_c;
const int width = 150;
const int height = 50;
//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
int zeroaxis = 30;


void WriteAt(int x, int y, LPCWSTR c) {
    COORD pos = { x, y };
    DWORD dwBytesWritten = 0;
    WriteConsoleOutputCharacter(hConsole_c, c, 1, pos, &dwBytesWritten);
}

void WriteAt(int x, int y, string msg) {
    LPCWSTR c;
    LPCWSTR c1;
    std::string s;
    std::wstring stemp;
    char ch;


    for (int k = 0; k < msg.size(); k++)
    {
        ch = msg[k];
        s += ch;
        stemp = std::wstring(s.begin(), s.end());
        c1 = stemp.c_str();
        s = "";
        WriteAt(x + k, y, c1);
    }
}


int main()
{
    int mode;
    //SetConsoleTextAttribute(hConsole, 7);

    char com_port[] = "\\\\.\\COM5";    
    
    cout << "Please choose a mode (1:wide; 2:accurate): ";
    std::cin >> mode;

    DWORD COM_BAUD_RATE = CBR_9600;
    SimpleSerial Serial(com_port, COM_BAUD_RATE);

    char newinput;

    while (!Serial.connected_)
    {        
        cout << "Connection failed. Please try another port number (1-9): ";
        cin >> newinput;
        com_port[7] = newinput;

        SimpleSerial Serial(com_port, COM_BAUD_RATE);
    }
    cout << "Connecting to device...";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));


    hConsole_c = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    /*width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;*/
    SetConsoleActiveScreenBuffer(hConsole_c);


    if (Serial.connected_) {

        string incoming;


        int matrix[height][width];
        for (size_t i = 0; i < height; i++)
            for (size_t j = 0; j < width; j++)
                if ((i == 0 || j == 0) || (i == height - 1 || j == width - 1))
                    matrix[i][j] = 5;
                else if (i == zeroaxis)
                    matrix[i][j] = 1;
                else if (i % 5 == 0)
                    matrix[i][j] = 3;
                else if ((i + 2) % 5 == 0)
                    matrix[i][j] = 4;
                else
                    matrix[i][j] = 0;


        int cell;
        LPCWSTR c;
        LPCWSTR c1;
        LPCWSTR c2;
        std::string s;
        std::string fs;
        std::wstring stemp;
        char ch;

        vector<double> temps;
        vector<int> times;

        string value;
        string stimepassed;
        string sroomtmp;
        string ssamplerate;
        unsigned long long int timepassed = 0;
        double temp = 0;
        double roomtmp = 0;
        int samplerate = 0;

        std::chrono::duration<double> elapsed_seconds;

        bool secondtick = false;

        auto start = std::chrono::system_clock::now();
        std::time_t start_time = std::chrono::system_clock::to_time_t(start);
        ofstream output1("output_short_" + to_string(start_time) + ".txt");
        ofstream output2("output_long_" + to_string(start_time) + ".txt");

        int longrecords = 1;
        int shortrecords = 1;

        double sum = 0;


        while (true)
        {
            incoming = Serial.ReadSerialPort(1, "json");
            
            value = "";
            stimepassed = "";
            sroomtmp = "";
            ssamplerate = "";
            for (size_t i = 0; i < incoming.size() && incoming[i]!=';'; i++)
                value += incoming[i];
            

            if (value.size() > 0) {
                temps.push_back(std::stod(value));

                elapsed_seconds = std::chrono::system_clock::now() - start;
                times.push_back(elapsed_seconds.count());

                if (elapsed_seconds.count() > 10 * shortrecords) {
                    output1 << temps[temps.size() - 1] << "\t" << elapsed_seconds.count() << endl;
                    shortrecords++;
                }
                if (elapsed_seconds.count() > 60 * longrecords) {

                    for (size_t i = 1; i < 6; i++)
                        sum += temps[temps.size() - i];

                    output2 << sum / 5 << "\t" << elapsed_seconds.count() << endl;
                    longrecords++;
                    sum = 0;
                }


                for (int i = value.size() + 1; incoming[i] != ';'; i++)
                    stimepassed += incoming[i];
                timepassed += std::stoi(stimepassed);

                for (int i = value.size() + stimepassed.size() + 2; incoming[i] != ';'; i++)
                    sroomtmp += incoming[i];
                roomtmp = std::stod(sroomtmp);

                for (int i = value.size() + stimepassed.size() + sroomtmp.size() + 3; i < incoming.size(); i++)
                    ssamplerate += incoming[i];
                samplerate = std::stoi(ssamplerate);


                
                for (size_t i = 1; i < height - 1; i++)
                    for (size_t j = 1; j < width - 1; j++)
                        if (i == zeroaxis)
                            matrix[i][j] = 1;
                        else if (width - 1 - j < temps.size() && (height - temps[temps.size() - (width - 1 - j)] - (height - zeroaxis) <= i /* && (temps[temps.size() - (width - 1 - j)] < 0 || i < zeroaxis)*/)) {
                            matrix[i][j] = 2;
                        }
                        else if (i % 5 == 0)
                            matrix[i][j] = 3;
                        else if ((i + 2) % 5 == 0)
                            matrix[i][j] = 4;
                        else
                            matrix[i][j] = 0;


                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++) {
                        cell = matrix[i][j];

                        switch (cell)
                        {
                        case 0:
                            c = L" ";
                            break;
                        case 1:
                            c = L"═";
                            break;
                        case 2:
                            if (secondtick)
                                if (j % 2 == 0)
                                    c = L"▓";
                                else
                                    c = L"░";
                            else
                                if (j % 2 == 1)
                                    c = L"▓";
                                else
                                    c = L"░";
                            break;
                        case 3:
                            c = L"_";
                            break;
                        case 4:
                            c = L".";
                            break;
                        default:
                            c = L"█";
                            break;
                        }

                        WriteAt(j + 10, i, c);
                    }


                    if (i > 0 && i < height - 1) {
                        if (i - zeroaxis > 0) {

                            WriteAt(width + 11, i, L"-");
                            for (int k = 0; k < std::to_string(i - zeroaxis).size(); k++)                           
                            {                     
                                ch = std::to_string(i - zeroaxis)[k];
                                s += ch;
                                stemp = std::wstring(s.begin(), s.end());
                                c1 = stemp.c_str();
                                s = "";
                                WriteAt(width + 12 + k, i, c1);
                            }
                        }
                        else
                        {
                            /*fs = std::to_string(zeroaxis - i);
                            ch = fs[0];
                            s += ch;
                            stemp = std::wstring(s.begin(), s.end());
                            c1 = stemp.c_str();
                            s = "";
                            WriteAt(width + 12, i, c1);

                            if (fs.size() > 1) {
                                ch = fs[1];
                                s += ch;
                                stemp = std::wstring(s.begin(), s.end());
                                c2 = stemp.c_str();
                                s = "";
                                WriteAt(width + 13, i, c2);
                            }*/
                            for (int k = 0; k < std::to_string(zeroaxis - i).size(); k++)
                            {
                                ch = std::to_string(zeroaxis - i)[k];
                                s += ch;
                                stemp = std::wstring(s.begin(), s.end());
                                c1 = stemp.c_str();
                                s = "";
                                WriteAt(width + 11 + k, i, c1);
                            }
                        }
                        WriteAt(width + 20, i, L"█");
                    }

                    int index;
                    if (temps.size() > i) {
                        if(temps.size()>height)
                            index = i + (temps.size() - height);
                        else
                            index = i;
                        for (int k = 0; k < std::to_string(temps[index]).size(); k++)
                        {
                            ch = std::to_string(temps[index])[k];
                            s += ch;
                            stemp = std::wstring(s.begin(), s.end());
                            c1 = stemp.c_str();
                            s = "";
                            WriteAt(width + 23 + k, i, c1);
                        }   
                        
                        WriteAt(width + 35, i, to_string(times[index]) + "s");
                    }
                }
                secondtick = !secondtick;

                /*for (int k = 0; k < std::to_string(timepassed).size(); k++)
                {
                    ch = std::to_string(timepassed)[k];
                    s += ch;
                    stemp = std::wstring(s.begin(), s.end());
                    c1 = stemp.c_str();
                    s = "";
                    WriteAt(width + 35 + k, 0, c1);
                }

                for (int k = 0; k < std::to_string(roomtmp).size(); k++)
                {
                    ch = std::to_string(roomtmp)[k];
                    s += ch;
                    stemp = std::wstring(s.begin(), s.end());
                    c1 = stemp.c_str();
                    s = "";
                    WriteAt(width + 35 + k, 5, c1);
                }

                for (int k = 0; k < std::to_string(samplerate).size(); k++)
                {
                    ch = std::to_string(samplerate)[k];
                    s += ch;
                    stemp = std::wstring(s.begin(), s.end());
                    c1 = stemp.c_str();
                    s = "";
                    WriteAt(width + 35 + k, 6, c1);
                }*/

                //WriteAt(width + 48, 0, std::to_string(timepassed)+"ms");
                WriteAt(width + 48, 5, sroomtmp);
                WriteAt(width + 48, 6, ssamplerate);
            }
        }
    }
}
