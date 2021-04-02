#ifndef _WaveIOHeader_
#include "WavIO.h"
#endif // !1
using namespace std;

//Constructor for reading a file
//name is the name of the file to be read
//The file should be in the current working directory
WaveIO::WaveIO(char name[])
{
	int err = fopen_s(&fp, name, "rb"); // read binary data mode

	if (err != 0)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	//Initialization
	NumChannels = 0;
	SamplingRate = 0;
	DataLength = 0;
	BitsPerSample = 0;

	//Pushing the Metadata into an array for easy Indexed Access of each element
	for (int j = 0; j < 44; j++)
	{
		meta.push_back(fgetc(fp));
	}

	NumChannels = (meta[23] << 8) + meta[22]; //Number Of channels in audio file. Mono = 1, Stereo = 2.....etc.
	SamplingRate = (meta[27] << 24) + (meta[26] << 16) + (meta[25] << 8) + meta[24];
	DataLength = (meta[43] << 24) + (meta[42] << 16) + (meta[41] << 8) + meta[40]; //Length of Audio Data in bytes(2 * Number of Samples for 16-bit PCM)
	BitsPerSample = (meta[35] << 8) + meta[34];                                    //Size of each Sample. 8-bit or 16-bit
	printf("Number Of Channels: %d \nSampling Rate: %d \nLength Of Data: %d\nBits Per Sample: %d\n",
		NumChannels, SamplingRate, DataLength, BitsPerSample);
}

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
WaveIO::WaveIO(char name[], int NC, int SR, int BPS, vector<short> d)
{
	int err = fopen_s(&fp, name, "wb"); // write binary data mode

	if (err != 0)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	NumChannels = NC;
	SamplingRate = SR;
	BitsPerSample = BPS;
	DataLength = 2 * d.size();
	data = d;

	// For More Information on Wav File Format, Visit - http://soundfile.sapp.org/doc/WaveFormat/
	// Visit this site for details of all the fields.
	int Subchunk2Size = DataLength;
	int Subchunk2ID = stbin(0x64617461, 4);
	short BitsPerSample = BPS;
	short BlockAlign = NumChannels * BPS / 8;
	int ByteRate = SamplingRate * NumChannels * BitsPerSample / 8;
	short AudioFormat = 1;
	int Subchunk1Size = 16;
	int Subchunk1ID = stbin(0x666d7420, 4);
	int Format = stbin(0x57415645, 4);
	int ChunkSize = 36 + DataLength;
	int ChunkID = stbin(0x52494646, 4);
	short NCW = NumChannels;
	int SRW = SamplingRate;
	short BPSW = BitsPerSample;

	fwrite(&ChunkID, sizeof(ChunkID), 1, fp);
	fwrite(&ChunkSize, sizeof(ChunkSize), 1, fp);
	fwrite(&Format, sizeof(Format), 1, fp);
	fwrite(&Subchunk1ID, sizeof(Subchunk1ID), 1, fp);
	fwrite(&Subchunk1Size, sizeof(Subchunk1Size), 1, fp);
	fwrite(&AudioFormat, sizeof(AudioFormat), 1, fp);
	fwrite(&NCW, sizeof(NCW), 1, fp);
	fwrite(&SRW, sizeof(SRW), 1, fp);
	fwrite(&ByteRate, sizeof(ByteRate), 1, fp);
	fwrite(&BlockAlign, sizeof(BlockAlign), 1, fp);
	fwrite(&BPSW, sizeof(BPSW), 1, fp);
	fwrite(&Subchunk2ID, sizeof(Subchunk2ID), 1, fp);
	fwrite(&Subchunk2Size, sizeof(Subchunk2Size), 1, fp);

	for (short val : d)
	{
		fwrite(&val, sizeof(val), 1, fp);
	}
}

int WaveIO::stbin(int dat, int x)
{
	if (x == 2)
	{
		return (((dat & 255) << 8) + (dat >> 8));
	}
	else
	{
		/* code */
		int fir = dat & 255;
		dat = dat >> 8;
		int sec = dat & 255;
		dat = dat >> 8;
		int tir = dat & 255;
		dat = dat >> 8;
		int fou = dat;
		return ((fir << 24) + (sec << 16) + (tir << 8) + fou);
	}
}

int WaveIO::readWav()
{

	// For More Information on Wav File Format, Visit - http://soundfile.sapp.org/doc/WaveFormat/

	int count = 0;
	for (int j = 0; j < DataLength / 2; j++, count++)
	{
		short ch1;
		fread(&ch1, sizeof(short), 1, fp);
		data.push_back(ch1); /*Since this is a single channel audio file, both channels have to be combined.
											   Since this was a 16-bit audio, the first byte represented the left channel and
											   Second byte represented the right channel, so both channels had to be combined
											   with right channel as the MSB and left channel as LSB.
												   */
	}

	//Printing the Wav Data
	/*count = 0;
	for (int j = 0; j < 40; j++)
	{
	   count++;
	   printf("%d ", data[j]);
	   if (count % 10 == 0)
		  printf("\n");
	}*/

	if(fp)
		fclose(fp);

	return 0;
}