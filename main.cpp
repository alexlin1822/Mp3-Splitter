/*
* Mp3 Splitter
* this program can create a bat file for mp3splt.exe
* use the bat file to split a mp3 music
* this program use in Windows 
* Created by Alex Lam
* Jan/10/2021
*
* time file [default: ztime.txt] content format
* support UTF-8
*
* [00:00:00]  -  music title1
* [00:01:00]  -  music title2
* [00:03:00]  -  music title3
* ........
*/

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <direct.h>
#include <stringapiset.h>
#include "windows.h"

using namespace std;

const string sBATfile="Zmp3splitter.bat";		//output bat file name
string sPath;		//current path

//Time File content struct after format
struct TTimeFile{
    wstring sgStartTime=L"0.0";
    wstring sgEndTime=L"EOF";
    wstring sgTitle=L"";
};

/*String handle functions*/

//change string to wstring
wstring StringToWString(const string& str) {
    int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t *wide = new wchar_t[num];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
    wstring w_str(wide);
    delete[] wide;
    return w_str;
}

//left trim
wstring& ltrim(wstring& str, const wstring& chars = L"\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

//right trim
wstring& rtrim(wstring& str, const wstring& chars = L"\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}
 
//both size trim
//*use this for a wstring
wstring& trim(wstring& str, const wstring& chars = L"\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}


/* application functions */

//format and save the text to TTimeFile struct
TTimeFile setValue(wstring sLine){
    TTimeFile tTmp;

    if (sLine==L"")
        return tTmp;

    wstring sHour=sLine.substr(1,2);
    wstring sMin=sLine.substr(4,2);
    wstring sSec=sLine.substr(7,2);
   
    int iHour,iMin;
    iHour=stoi(sHour);
    iMin=stoi(sMin);

    if (iHour>0){
        iMin=iMin+iHour*60;
        sMin=to_wstring(iMin);
    }
    tTmp.sgStartTime=sMin+L"."+sSec;
    
	size_t found=sLine.find_first_of(L"-");
    if (found<256) {
    	wstring sTmpTitle=sLine.substr(found+1);
		tTmp.sgTitle=trim(sTmpTitle);
	}
    else{
    	tTmp.sgTitle=L"song_"+sLine;	
	}
    return tTmp;
}

//get the line number of time file 
int CountLines(string filename){
	int numLines = 0;
	ifstream in(filename);
	string unused;
	while ( getline(in, unused) )
	   ++numLines;
	   in.close();
	return numLines;
}

// output the BAT file
int split(string sfMp3, string sfTime,string sOutput){
    int iTotal=CountLines(sfTime);
    if (iTotal<1){
        printf("Time file is emtpy!");
        return 1;
    }

    TTimeFile *tArray=new TTimeFile[iTotal];
	wstring sTmp;
	int i=0;
	
	//read the time file	
	wifstream TimeFile;
    TimeFile.open(sfTime,ios::in);

    while (getline(TimeFile,sTmp))
    {
        TTimeFile tmpTF;
        tmpTF=setValue(sTmp);
        tArray[i]=tmpTF;

        if (i>0){
            tArray[i-1].sgEndTime=tArray[i].sgStartTime;
        }   
        i++;
    }
	TimeFile.close();
	  
	string sFullBat=sPath+"\\"+sBATfile;  
    //output to bat file
    wofstream OutFile(sFullBat); 
    
    wstring sCmd;
    
    OutFile<<"echo off\n";
    OutFile<<"chcp 65001\n";		//very important let bat support Chinese name
    
    for (int j=0; j<iTotal;j++){
    	
		sCmd=StringToWString(sPath)+
			L"\\mp3splt\\mp3splt.exe "+
			StringToWString(sfMp3)+L" "+
			tArray[j].sgStartTime+L" "+
			tArray[j].sgEndTime+L" "+
			L"-o \"" + tArray[j].sgTitle+L"\" "+
			L"-d \""+StringToWString(sOutput)+L"\" \n";

		OutFile <<sCmd;
	}

	OutFile<<"echo -------------------------------------------------------------------------------\n";
	OutFile<<"echo *******************************************************************************\n";
	OutFile<<"echo "<<StringToWString(sfMp3)<<" had been split to ["<<to_wstring(iTotal)<<"] music files.\n";
	OutFile<<"echo Please check out your new music files at ["<<StringToWString(sOutput)<<"].\n";
	OutFile<<"echo *******************************************************************************\n";
    OutFile<<"echo Thank you for your using. You can close this windows. \n";
	OutFile<<"echo on\n";
	OutFile<<"pause\n";
	
    OutFile.close();      
	cout<<"\nFinished!\n";
	cout<<"\nThe BAT file is in ["<<sFullBat <<"]\n";
	
	//run the command
	LPTSTR long_string = new TCHAR[sFullBat.size() + 1]; 
   	strcpy(long_string, sFullBat.c_str());
	
	WinExec(long_string,SW_SHOWNORMAL);
	
    return 0;
} 

int main(int argc, char** argv) {
	char *buffer;
    buffer = getcwd(NULL, 0);
    sPath=string(buffer);
    free(buffer);

    string sCinTmp="p";    
    string sfMp3="D:\\Downloads\\zmusic.mp3";      //mp3 file
    string sfTime="ztime.txt";     //time file
    string sOutput="D:\\Downloads\\zoutput";     //output dir
   
    //show welcome
    cout<<"Welcome to use the mp3 splitter BAT file maker.\n\n"<<endl;
    cout<<"Please enter informat below. English only.\n\n";
	
	//enter mp3 tile
	cout<<"Please Enter the music file path and name. Default : [D:\\Downloads\\zmusic.mp3]\n";
	cout<<"[Use default press 'p' and enter]\n";
	cin >> sCinTmp; // Get user input from the keyboard
	if (sCinTmp!="p") {sfMp3=sCinTmp;}
	cout<<"The music file :"<<sfMp3<<endl;
	
	//enter time file
	cout<<"\nPlease Enter the time split file path and name. Default : [<Current Dir>\\ztime.txt]\n";
	cout<<"You can content UTF-8 text in this file. but the file name is English only.\n";
	cout<<"[Use default press 'p' and enter]\n";
	cin >> sCinTmp; // Get user input from the keyboard
	if (sCinTmp!="p") {sfTime=sCinTmp;}
	cout<<"The time split file :"<<sfTime<<endl;
	
	//enter output direction
	//get the same dir with music file
	size_t found=sfMp3.find_last_of("/\\");
    if (found<256) {sOutput=sfMp3.substr(0,found+1)+"zoutput";}
	cout<<"\nPlease Enter the output direction. Default : ["<<sOutput<<"]\n";
	cout<<"[Use default press 'p' and enter]\n";
	cin >> sCinTmp; // Get user input from the keyboard
	if (sCinTmp!="p") {sfTime=sCinTmp;}
	cout<<"The output direction :"<<sOutput<<endl;

	cout<<"\nThank you for your information.\nThe BAT file is creating..."<<endl;   
    split(sfMp3, sfTime,sOutput); 
	return 0;
}
