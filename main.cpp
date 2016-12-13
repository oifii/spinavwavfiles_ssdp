/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <string>
#include <fstream>
#include <vector>

#include <iostream>
#include <sstream>
using namespace std;
//#include <stdlib.h>

#include "WavFile.h"
#include "SoundTouch.h"
using namespace soundtouch;

#include <ctime>

int main(int argc, char *argv[])
{
	///////////////////
	//read in arguments
	///////////////////
	double fMaxSecondsPerWavFile = 30;
	double fSecondsPerSegment = 0.5;
	//double fSecondsPerSegment = 1.0;
	int numSecondsPlay = 20;
	bool boolRandomNavigation = 1; //0=sequentialy or 1=randomly
	if(argc>1)
	{
		//first argument is the maximum wav file length in seconds, i.e. longer wav files shall be ignored
		fMaxSecondsPerWavFile = atof(argv[1]);
	}
	if(argc>2)
	{
		//second argument is the segment length in seconds
		fSecondsPerSegment = atof(argv[2]);
	}
	if(argc>3)
	{
		//third argument is the time it will shuffle
		numSecondsPlay = atoi(argv[3]);
	}
	if(argc>4)
	{
		//fourth argument defines how to browse through filenames, 0=sequentialy or 1=randomly?
		boolRandomNavigation = atoi(argv[4]);
	}

	////////////////////////////////////////////
	//execute cmd line to get all .wav filenames
	////////////////////////////////////////////
	system("DIR *.wav /B /O:N > filenames.txt");


	///////////////////////////////////////
	//load filenames.txt into string vector
	///////////////////////////////////////
	vector<string> filenames;
	ifstream ifs("filenames.txt");
	string temp;
	while(getline(ifs,temp))
		filenames.push_back(temp);


	////////////////////////////////////////////////////////////////////////////////////////////
	//browse throught filenames, filter out wav files too large and call spisplitshuffleplay.exe
	////////////////////////////////////////////////////////////////////////////////////////////
	double fSecondsPerWavFile = -1.0;
	string quote;
	quote = "\"";
	string cmdbuffer;
	string param1;
	string param2;
	stringstream ss1 (stringstream::in | stringstream::out);
	stringstream ss2 (stringstream::in | stringstream::out);
	ss1 << fSecondsPerSegment;
	param1 = ss1.str();
	ss2 << numSecondsPlay;
	param2 = ss2.str();
	vector<string>::iterator it;
	cout << "filenames.txt contains:";


	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));
	int random_integer=-1;
	int lowest=1, highest=filenames.size();
	int range=(highest-lowest)+1;
	
	it=filenames.begin();
	while(1)
	//for ( it=filenames.begin() ; it < filenames.end(); it++ )
	{
		cout << *it << endl; //*it is a .wav filename

		WavInFile* pWavInFile = NULL;
		try
		{
			pWavInFile = new WavInFile((*it).c_str());
		}
		catch(char* str) 
		{
			cout << "Exception raised within new WavInFile(): " << str << '\n';
		}
		if(pWavInFile)
		{
			//fSecondsPerWavFile = pWavInFile->getLengthMS()/1000.0; //provoques assert failure for large files
			fSecondsPerWavFile = pWavInFile->getNumSamples()/pWavInFile->getSampleRate();
			int numChannels = pWavInFile->getNumChannels();
			delete pWavInFile;

			if(fSecondsPerWavFile <= fMaxSecondsPerWavFile && numChannels==2)
			{
				//cmdbuffer = "spisplitshuffleplay.exe " + quote + *it + quote + " " + param1 + " " + param2;
				cmdbuffer = "spisplitdisperseplay.exe " + quote + *it + quote + " " + param1 + " " + param2;
				cout << cmdbuffer << endl;
				system(cmdbuffer.c_str()); 
			}
			else
			{
				cout << "skipping file " << *it << endl;
			}
		}
		else
		{
			cout << "skipping file " << *it << "(new WavInFile() returns NULL)" << endl;
		}
		//sequential access or random access
		if(boolRandomNavigation==0)
		{
			//sequential
			it++;
			if(it==filenames.end())it=filenames.begin();
		}
		else
		{
			//random
			random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
			it = filenames.begin() + random_integer-1;
		}
	}
	cout << endl;


	
	return 0;
}