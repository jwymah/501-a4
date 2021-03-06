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

// http://www.drdobbs.com/cpp/a-simple-and-efficient-fft-implementatio/199500857
void four1(double* data, unsigned long nn)
{
	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	// reverse-binary reindexing
	n = nn<<1;
	j=1;
	for (i=1; i<n; i+=2)
	{
		if (j>i)
		{
			swap(data[j-1], data[i-1]);
			swap(data[j], data[i]);
		}
		m = nn;
		while (m>=2 && j>m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	};

	// here begins the Danielson-Lanczos section
	mmax=2;
	while (n>mmax)
	{
		istep = mmax<<1;
		theta = -(2*M_PI/mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m=1; m < mmax; m += 2)
		{
			for (i=m; i <= n; i += istep)
			{
				j=i+mmax;
				tempr = wr*data[j-1] - wi*data[j];
				tempi = wr * data[j] + wi*data[j-1];

				data[j-1] = data[i-1] - tempr;
				data[j] = data[i] - tempi;
				data[i-1] += tempr;
				data[i] += tempi;
			}
			wtemp=wr;
			wr += wr*wpr - wi*wpi;
			wi += wi*wpr + wtemp*wpi;
		}
		mmax=istep;
	}
}

void four2(double* data, unsigned long nn)
{
	unsigned long n, mmax, m, j, istep, i;
	double wtemp, wr, wpr, wpi, wi, theta;
	double tempr, tempi;

	// reverse-binary reindexing
	n = nn<<1;
	j=1;
	for (i=1; i<n; i+=2)
	{
		if (j>i)
		{
			swap(data[j-1], data[i-1]);
			swap(data[j], data[i]);
		}
		m = nn;
		while (m>=2 && j>m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	};

	// here begins the Danielson-Lanczos section
	mmax=2;
	while (n>mmax)
	{
		istep = mmax<<1;
		theta = (2*M_PI/mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m=1; m < mmax; m += 2)
		{
			for (i=m; i <= n; i += istep)
			{
				j=i+mmax;
				tempr = wr*data[j-1] - wi*data[j];
				tempi = wr * data[j] + wi*data[j-1];

				data[j-1] = data[i-1] - tempr;
				data[j] = data[i] - tempi;
				data[i-1] += tempr;
				data[i] += tempi;
			}
			wtemp=wr;
			wr += wr*wpr - wi*wpi;
			wi += wi*wpr + wtemp*wpi;
		}
		mmax=istep;
	}
}

int main()
{
    
    /* Open the WAV file. */
	cout << "\n";
	getWavData(soundInfo, soundData, "tabla.wav", "data.out");
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
//    for (int i=0; i<500000; i++)
//    {
//    	for (int j=0; j<irData.size(); j++)
//    	{
//    		resultData[i+j] += soundData[i] * irData[j];
//    	}
//    }

    //TODO use four1()
    int maxSize = max(soundData.size(), irData.size());
    int pow2 = pow(2, (int) log2(maxSize) + 1); // how does not casting this to int cause four1(hcomplex to segfault?
    											// oh must be a rounding thing
    int thatSize = 2 * pow2;

    double *xcomplex = new double[thatSize];
    double *hcomplex = new double[thatSize];

    for (int i=0; i<thatSize; i++)
    {
//    	 memset() causes segfault? should be more efficient
    	xcomplex[i] = 0.0;
    	hcomplex[i] = 0.0;
    }

    //even elements are real part, odds are imaginary part
    for (int i=0; i<soundData.size(); i++)
    {
    	xcomplex[2*i] = soundData[i];
    }
    for (int i=0; i<irData.size(); i++)
    {
    	hcomplex[2*i] = irData[i];
    }
    cout << pow2 << endl;
    cout << thatSize << endl;

    four2(hcomplex, pow2);
    four2(xcomplex, pow2);

    double *ycomplex = new double[thatSize];
    for(int i=0; i<pow2; i++)
    {
    	ycomplex[2*i] = (xcomplex[i] * hcomplex[i] - xcomplex[i+1] * hcomplex[i+1]);
    	ycomplex[2*i+1] = (xcomplex[i+1] * hcomplex[i] - xcomplex[i] * hcomplex[i+1]);
    }

    four1(ycomplex, pow2);

	writeWaveFileHeader(channels, frames, samplerate, outputFileStream);
    cout << "resultSize" << resultSize << endl;

    cout << "resultData.size()" << resultData.size() << endl;
//    for (int i=0; i<resultSize; i++)// soundInfo.channels)
	for (int i=0; i<pow2; i++)// soundInfo.channels)
    {
//    	if (resultData[i] < -1) resultData[i] = -1;
//		if (resultData[i] > 1) resultData[i] = 1;
//        fwriteShortLSB((short) (resultData[i]*32767), outputFileStream);
    	if (ycomplex[i] < -1) ycomplex[i] = -1;
		if (ycomplex[i] > 1) ycomplex[i] = 1;
        fwriteShortLSB((short) (ycomplex[i]*32767), outputFileStream);
    }

    fclose(outputFileStream);
    return 0;
}
