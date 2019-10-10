// this probably breaks like every coding convention ever but whatever

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <codecvt>
#include <locale>

using namespace std;

int frame_num = 22;
int digit_num = 2;
int fps = 30;
int sample_rate = 44100;

int main(int argc, char *argv[]){
    if (argc != 3){
        cout << "USAGE : \"baby <input> <output>";

        return 0;
    }

    ofstream outputFile;
    outputFile.open(argv[2]);

    system("touch list.txt");
    ofstream tempFile;
    tempFile.open("list.txt");

    string pref = "ffmpeg -i ";
    string input = argv[1];
    string suff = " -f s16le -acodec pcm_s16le temp.raw";
    string cmdline = pref + input + suff;
    system(cmdline.c_str());

    fstream rawFile("temp.raw", ios::in | ios::binary);
    int i = 0;

    int framecount = 0;
    int samplecount = 0;
    char buffer[int(sample_rate/fps * 4) - 1];

    float smoothy[4];
    while (rawFile.read(buffer, sizeof(buffer))){
        int size = sizeof(buffer)/4;
        samplecount += size;
        float value = 0.0;
        int bigest = 0, second_bigest = 0;
        for (int i = 0; i < size; i++){
            int sample= buffer[4*i];

            if (abs(sample) > bigest){
                second_bigest = bigest;
                bigest = abs(sample);
            }
        }
        value = (bigest + second_bigest)/2;
        smoothy[0] = smoothy[1];
        smoothy[1] = smoothy[2];
        smoothy[2] = smoothy[3];
        smoothy[3] = fmin((((value + 1)*(value + 1)*(value + 1)/(SCHAR_MAX*SCHAR_MAX*SCHAR_MAX)) * (frame_num - 1) + 1)*2, 22);
        int val = int((smoothy[0] + smoothy[1] + smoothy[2] + smoothy[3])/4);
        i = 0;
        tempFile << "file clips/";
        tempFile << setw(digit_num) << setfill('0') << val;
        tempFile << ".mp4" << endl;

        framecount++;

        if (samplecount >= sample_rate){
            cout << "sample : " << samplecount << endl;
            samplecount -= sample_rate;
            cout << "frame : " << framecount << endl;
            framecount -= fps;
            cout << "value : " << value << endl;
        }
    }

    pref = "ffmpeg -f concat -i list.txt -auto_convert 1 -c copy temp_";
    input = argv[2];
    cmdline = pref + input;
    system(cmdline.c_str());

    system("rm temp.raw");
    system("rm list.txt");

    pref = "ffmpeg -i ";
    string pref2 = " -i temp_";
    string input2 = argv[1];
    suff = " ";
    cmdline = pref + input2 + pref2 + input + suff + input;
    system(cmdline.c_str());

    pref = "rm temp_";
    cmdline = pref + input;
    system(cmdline.c_str());

    return 0;
}
