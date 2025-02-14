#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <csignal>

std::ofstream outFile;
CURL* curl = nullptr;

void cleanup(int signum) {
    if (curl) {
        curl_easy_cleanup(curl);
        std::cout << "\nCleanup done. Exiting gracefully..." << std::endl;
    }
    if (outFile.is_open()) {
        outFile.close();
    }
    exit(signum);
}

size_t WriteCallback(void* ptr, size_t size, size_t nmemb, void* userdata) {
    std::ofstream* outFile = static_cast<std::ofstream*>(userdata);
    outFile->write(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

int main() {
    std::string url;
    std::string outputFilename;

    std::cout << "Enter the URL: ";
    std::cin >> url;
    std::cout << "Enter the output file name: ";
    std::cin >> outputFilename;

    outFile.open(outputFilename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file." << std::endl;
        return 1;
    }

    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl." << std::endl;
        return 1;
    }

    std::signal(SIGINT, cleanup);
    std::cout << "Recording started. Press CTRL+C to stop and save." << std::endl;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }

    cleanup(0);
    return 0;
}
