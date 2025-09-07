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


void saveWav(const std::string &filename, const std::vector<int16_t> &samples, int sampleRate = 44100) {
    //I honestly couldnt be bothered figuring this out, i just looked it up frankly, its not something ive defined bruh
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open file " << filename << " for writing\n";
        return;
    }

    int numSamples = static_cast<int>(samples.size());
    int byteRate = sampleRate * 2; // 16-bit mono = 2 bytes per sample

    // --- WAV header ---
    out.write("RIFF", 4);
    int32_t chunkSize = 36 + numSamples * 2; 
    out.write(reinterpret_cast<const char*>(&chunkSize), 4);
    out.write("WAVE", 4);

    // fmt subchunk
    out.write("fmt ", 4);
    int32_t subChunk1Size = 16;
    out.write(reinterpret_cast<const char*>(&subChunk1Size), 4);
    int16_t audioFormat = 1; // PCM
    out.write(reinterpret_cast<const char*>(&audioFormat), 2);
    int16_t numChannels = 1; // mono
    out.write(reinterpret_cast<const char*>(&numChannels), 2);
    out.write(reinterpret_cast<const char*>(&sampleRate), 4);
    out.write(reinterpret_cast<const char*>(&byteRate), 4);
    int16_t blockAlign = 2; // numChannels * bitsPerSample/8
    out.write(reinterpret_cast<const char*>(&blockAlign), 2);
    int16_t bitsPerSample = 16;
    out.write(reinterpret_cast<const char*>(&bitsPerSample), 2);

    // data subchunk
    out.write("data", 4);
    int32_t dataSize = numSamples * 2;
    out.write(reinterpret_cast<const char*>(&dataSize), 4);

    // write actual samples
    out.write(reinterpret_cast<const char*>(samples.data()), dataSize);

    out.close();
    std::cout << "Saved WAV: " << filename << "\n";
}







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
    fs::create_directories("../Recordings/" + label);

    int fileIndex = 1;
    //we want to get the latest filename (theyre iterated integers so the largest number) in the labels folder in Recordings

    //auto is a placeholder type, this is changed to what the value given by fs::directory_iterator("Recordings/" + label)
        //auto p  →  std::filesystem::directory_entry p
    //auto& is a reference to the original file given by the iterator, if it was just auto, that would make a copy
    for (auto& p : fs::directory_iterator("../Recordings/" + label)) {
        //only deal with files that are .wav since those are recordings
        if (p.path().extension() == ".wav") {
            //p.path().stem().string() gets the filename without extension, stoi converts the name to an int
            int n = std::stoi(p.path().stem().string());
            if (n >= fileIndex) fileIndex = n+1;
        }
    }
    std::string filename = "../Recordings/" + label + "/" + std::to_string(fileIndex) + ".wav";


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

    saveWav(filename, data.samples, SAMPLE_RATE);

    std::cout << "Saved recording to " << filename << "\n";
}
