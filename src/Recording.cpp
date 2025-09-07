#include <iostream>
#include <cstdint>      // for int16_t
#include <string>
#include "recording.h"
#include "portaudio.h"
#include <filesystem>
#include <fstream>
#include <vector>

//namespaces organise code, a class may need an implementation whereas a namespace just contains stuff
namespace fs = std::filesystem;

constexpr int SAMPLE_RATE = 44100;
constexpr int FRAMES_PER_BUFFER = 256;
constexpr PaSampleFormat SAMPLE_FORMAT = paInt16; // 16-bit PCM

struct RecordingData {
    std::vector<int16_t> samples;// a big array of the data from the mic, can make int32_t for paInt32
    bool recording = false;
};

//PortAudio requires recordCallback to have this signature
static int recordCallback(
    const void *input,
    void *output,
    unsigned long frameCount,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
){
    //must declare userData as our RecordingData because portAudio demands that the signature treat it as void
    RecordingData *data = static_cast<RecordingData*>(userData);

    // Cast the input buffer to the right type (16-bit samples)
    const int16_t *in = static_cast<const int16_t*>(input);

    if (data->recording && in != nullptr) {
        // Append new samples to our vector
        data->samples.insert(data->samples.end(), in, in + frameCount);//mono input its fine, n input, framCount * n
    }

    // Return paContinue to keep stream running, it is part of the PortAudio api, a constant declared there, needed here
    //can return paComplete which will stop the stream eventually basically like Pa_StopStream but Pa_StopStream is more abrupt
    return paContinue;
}

void recordAudio(const std::string &label) {
    fs::create_directories("Recordings/" + label);

    int fileIndex = 1;
    //we want to get the latest filename (theyre iterated integers so the largest number) in the labels folder in Recordings

    //auto is a placeholder type, this is changed to what the value given by fs::directory_iterator("Recordings/" + label)
        //auto p  →  std::filesystem::directory_entry p
    //auto& is a reference to the original file given by the iterator, if it was just auto, that would make a copy
    for (auto& p : fs::directory_iterator("Recordings/" + label)) {
        //only deal with files that are .wav since those are recordings
        if (p.path().extension() == ".wav") {
            //p.path().stem().string() gets the filename without extension, stoi converts the name to an int
            int n = std::stoi(p.path().stem().string());
            if (n >= fileIndex) fileIndex = n+1;
        }
    }
    std::string filename = "Recordings/" + label + "/" + std::to_string(fileIndex) + ".wav";


    Pa_Initialize();
    RecordingData data;
    data.recording = false;

    PaStream *stream;
    Pa_OpenDefaultStream(
        &stream, //points to the stream (idk what the docs said)
        1,       //input because mic
        0,       //not outputing anywhere
        SAMPLE_FORMAT, //16 bit output, can make paInt32 for 32 bit ouput
        SAMPLE_RATE,//samples per second
        FRAMES_PER_BUFFER, //how big the buffer is per callback
        recordCallback, //function that portaudio calls whenever new audio arrives
        &data   //pointing to the struct thats recording the data, is the userData in the callback function
    );

    //this starts calling the recordCallback and makes the recording basically,
    Pa_StartStream(stream);

    std::cout << "Press ENTER to start recording...\n";
    std::cin.get();
    data.recording = true;
    std::cout << "Recording... Press ENTER again to stop.\n";
    std::cin.get();
    data.recording = false;

    Pa_StopStream(stream); //stops the stream
    Pa_CloseStream(stream);//releases the resources that were making the stream possible
    Pa_Terminate();//terminates portaudio, its no longer needed



    std::cout << "Saved recording to " << filename << "\n";
}
