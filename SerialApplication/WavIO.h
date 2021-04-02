#ifndef _WaveIOHeader_
#define _WaveIOHeader_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

class WaveIO
{
public:
   int NumChannels;    //Stores the number of channels (1-Mono, 2- Stereo)
   int SamplingRate;   //Stores the Sampling Rate of the audio file;
   int DataLength;     //Stores the number of Bytes of audio data ie. 2 * Number of samples.
   int BitsPerSample;  //Size of each sample in bit. Usually 16-bit
   vector<char> meta;  //For Storing The meta data of Wav File
   vector<short> data; //For storing the Audio Data in an array
   FILE *fp;

   //Constructor for reading a file
   //name is the name of the file to be read
   //The file should be in the current working directory
   WaveIO(char name[]);

   //Constructor for writing a file
   /*
      This Constructor stores the necessary information regarding the audio file
      in the global variables. So all data is stored at the time of Creation of object.
    */
   //name is the name of output file. A ".wav" extension is added.
   /*
   NC is the number of channels in the output audio file.
   SR is the sampling rate.
   BPS is the Bits Per Sample
   d is the data vector.
    */
   WaveIO(char name[], int NC, int SR, int BPS, vector<short> d);

   int stbin(int dat, int x);

   int readWav();
};