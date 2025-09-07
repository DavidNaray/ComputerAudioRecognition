#include <iostream>
#include "portaudio.h"
#include "Recording.h"

int main(int argc, char* argv[]) {//argc is number of params, char* argv is an array of pointers to characters ie, a word
    if (argc == 3 && std::string(argv[1]) == "--record") {
        std::string label = argv[2];
        recordAudio(label);
        std::cout << "recording!!!" << std::endl;
    } else if (argc == 2 && std::string(argv[1]) == "--train") {
        // trainModel();
        std::cout << "training!!!!" << std::endl;
    } else {
        // listenAndRecognize();
        std::cout << "listening!!!!!!" << std::endl;
    }
    // std::cout << "Hello, audio world! poopy!aaaaa" << std::endl;
    return 0;
}