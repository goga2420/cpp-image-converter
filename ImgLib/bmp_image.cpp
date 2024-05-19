#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {


PACKED_STRUCT_BEGIN BitmapFileHeader {
    // поля заголовка Bitmap File Header
    unsigned char sign[2];
    unsigned int header_data;
    unsigned char reserved_space[4];
    unsigned int padding;
    
}
PACKED_STRUCT_END

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    // поля заголовка Bitmap Info Header
    unsigned int header_size;
    int w;
    int h;
    unsigned short plane_num;
    unsigned short bpp;
    unsigned int merge;
    unsigned int bpd;
    int hor_res;
    int vert_res;
    unsigned int used_colors;
    unsigned int important_colors;
}
PACKED_STRUCT_END

int ALIGNMENT = 4;
int COLORS = 3;

// функция вычисления отступа по ширине
static int GetBMPStride(int w) {
    return ALIGNMENT * ((w * COLORS + COLORS) / ALIGNMENT);
}

bool SaveBMP(const Path& file, const Image& image) {
    ofstream ofs(file, ios::binary);
    if (!ofs) {
        return false;
    }
    
    BitmapFileHeader bitmap_file_header;
    bitmap_file_header.sign[0] = 'B';
    bitmap_file_header.sign[1] = 'M';
    bitmap_file_header.header_data = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader) + GetBMPStride(image.GetWidth()) * image.GetHeight();
    bitmap_file_header.padding = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    
    BitmapInfoHeader bitmap_info_header;
    bitmap_info_header.header_size = sizeof(BitmapInfoHeader);
    bitmap_info_header.w = image.GetWidth();
    bitmap_info_header.h = image.GetHeight();
    bitmap_info_header.plane_num = 1;
    bitmap_info_header.bpp = 24;
    bitmap_info_header.merge = 0;
    bitmap_info_header.bpd = GetBMPStride(image.GetWidth()) * image.GetHeight();
    bitmap_info_header.hor_res = 11811;
    bitmap_info_header.vert_res = 11811;
    bitmap_info_header.used_colors = 0;
    bitmap_info_header.important_colors = 0x1000000;
    
    ofs.write(reinterpret_cast<char*>(&bitmap_file_header), sizeof(BitmapFileHeader));
    if (!ofs) {
        return false;
    }
    
    ofs.write(reinterpret_cast<char*>(&bitmap_info_header), sizeof(BitmapInfoHeader));
    if (!ofs) {
        return false;
    }
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    int padding = GetBMPStride(width);
    vector<char> buffer(padding);
    for (int y = height-1; y >= 0; --y) {
       const Color* line = image.GetLine(y);
        for (int x = 0; x < bitmap_info_header.w; ++x) {
            buffer[x * 3 + 0] = static_cast<char>(line[x].b);
            buffer[x * 3 + 1] = static_cast<char>(line[x].g);
            buffer[x * 3 + 2] = static_cast<char>(line[x].r);
        }
        ofs.write(buffer.data(), padding);
        if (!ofs) {
            return false;
        }
    }
    
    ofs.close();
    
    return true;
}

// напишите эту функцию
Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    BitmapFileHeader bitmap_file_header;
    ifs.read(reinterpret_cast<char*>(&bitmap_file_header), sizeof(BitmapFileHeader));
    if (!ifs) {
        return {};
    }
    
    BitmapInfoHeader bitmap_info_header;
    ifs.read(reinterpret_cast<char*>(&bitmap_info_header), sizeof(BitmapInfoHeader));
    if (!ifs) {
        return {};
    }
    
    Image image(bitmap_info_header.w, bitmap_info_header.h, Color::Black());
    int padding = GetBMPStride(bitmap_info_header.w);
    vector<char> buffer(padding);
    
    for (int y = bitmap_info_header.h-1; y >=0; --y) {
        Color* line = image.GetLine(y);
        ifs.read(buffer.data(), padding);
        if (!ifs) {
            return {};
        }
        for (int x = 0; x < bitmap_info_header.w; ++x) {
            line[x].b = static_cast<byte>(buffer[x * 3 + 0]);
            line[x].g = static_cast<byte>(buffer[x * 3 + 1]);
            line[x].r = static_cast<byte>(buffer[x * 3 + 2]);
        }
        
    }

    return image;
}


}  // namespace img_lib
