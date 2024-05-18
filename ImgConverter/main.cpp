#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>
using namespace std;

enum class Format {
    JPEG,
    PPM,
    BMP,
    UNKNOWN
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class JPEGInterface : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const {
        return img_lib::SaveJPEG(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadJPEG(file);
    }

};

class PPMInterface : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const {
       return img_lib::SavePPM(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadPPM(file);
    }
};

class BMPInterface : public ImageFormatInterface {
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const {
        return img_lib::SaveBMP(file, image);
    }
    img_lib::Image LoadImage(const img_lib::Path& file) const {
        return img_lib::LoadBMP(file);
    }
};

static const JPEGInterface jpeg_interface;
static const PPMInterface ppm_interface;
static const BMPInterface bmp_interface;
static const ImageFormatInterface* GetFormatInterface(const img_lib::Path& path)  {
    switch (GetFormatByExtension(path)) {
    case Format::JPEG:
        return &jpeg_interface;
    case Format::PPM:
        return &ppm_interface;
    case Format::BMP:
        return &bmp_interface;
    case Format::UNKNOWN:
        return nullptr;
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];
    if (GetFormatByExtension(in_path) == Format::UNKNOWN) {
        cerr << "Unknown format of the input file" << endl;
        return 2;
    }
    if (GetFormatByExtension(out_path) == Format::UNKNOWN) {
        cerr << "Unknown format of the output file" << endl;
        return 3;
    }
    auto interface_in = GetFormatInterface(in_path);

    img_lib::Image image = interface_in->LoadImage(in_path);
    if (!image) {
        cerr << "Loading failed"sv << endl;
        return 4;
    }
    auto interface_out = GetFormatInterface(out_path);
    if (!interface_out->SaveImage(out_path, image)) {
        cerr << "Saving failed"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
}