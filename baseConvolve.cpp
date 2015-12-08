#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <iostream>
#include <vector>
#include "utils.cpp"

using namespace std;

vector<double> soundData;
vector<double> irData;
vector<double> resultData;

SNDFILE *sf;
SF_INFO soundInfo;
SF_INFO irInfo;
int num_channels;
int num, num_items;
short *buf;
int f,sr,c;
int i,j;
FILE *out;

void getWavData(SF_INFO &info, vector<double> &data, const char *filename, const char *outfile) {
	/* Open the WAV file. */
	info.format = 0;
	sf = sf_open(filename, SFM_READ, &info);
	if (sf == NULL) {
		printf("Failed to open the file.  ");
		printf(filename);
		printf("\n");
		exit(-1);
	}
	/* Print some of the info, and figure out how much data to read. */
	f = info.frames;
	sr = info.samplerate;
	c = info.channels;
	printf("frames=%d\n", f);
	printf("samplerate=%d\n", sr);
	printf("channels=%d\n", c);
	num_items = f * c;
	printf("num_items=%d\n", num_items);
	/* Allocate space for the data to be read, then read it. */
	buf = (short*) (malloc(num_items * sizeof(short)));
	num = sf_read_short(sf, buf, num_items);
	sf_close(sf);

	printf("Read %d items\n", num);
	/* Write the data to filedata.out. */
	out = fopen(outfile, "w");
	for (i = 0; i < num; i += c) {
		for (j = 0; j < c; ++j)
		{
			fprintf(out, "%d ", buf[i + j]);
			data.push_back((double) buf[i + j] / 32767);
		}
		fprintf(out, "\n");
	}
	fclose(out);
}

int main()
{
    
    /* Open the WAV file. */
	cout << "\n";
	getWavData(soundInfo, soundData, "fluteDry.wav", "data.out");
	cout << "\n";
	getWavData(irInfo, irData, "irBIGHALL.wav", "imp.out");
	cout << "\n";

    /*  Open a binary output file stream for writing */
	char *filename = "out.wav";
    FILE *outputFileStream = fopen(filename, "wb");
    if (outputFileStream == NULL) {
        fprintf(stderr, "File %s cannot be opened for writing\n", filename);
        return 1;
    }

    int resultSize = soundData.size() + irData.size();
    int frames = soundInfo.frames + irInfo.frames - 1;
    int samplerate = soundInfo.samplerate;
    int channels = soundInfo.channels;

    for ( int i=0; i<resultSize; i++)
    {
    	resultData.push_back(0);
    }

    // one channel at a time?
    for (int i=0; i<500000; i++)
    {
//    	resultData[i] = soundData[i] * 1.0;
//    	resultData[i] = irData[i] * 1.0;
    	for (int j=0; j<irData.size(); j++)
    	{
    		resultData[i+j] += soundData[i] * irData[j];//(irData[j] / (float) 32767.0);
//    		for (int k=0; k<channels; k++)
//    		{
//    			resultData[i+j+k] += soundData[i+k] * irData[j+k];
//    		}
    	}
    }

	writeWaveFileHeader(channels, frames, samplerate, outputFileStream);
    cout << "resultSize" << resultSize << endl;

    cout << "resultData.size()" << resultData.size() << endl;
    for (int i=0; i<resultSize; i++)// soundInfo.channels)
    {
    	if (resultData[i] < -1) resultData[i] = -1;
		if (resultData[i] > 1) resultData[i] = 1;
        fwriteShortLSB((short) (resultData[i]*32767), outputFileStream);
    }

    fclose(outputFileStream);
    return 0;
}
