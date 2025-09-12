#pragma once

#include <string>

class Texture
{
public:
    Texture(const std::string &filepath, unsigned int slot);
    ~Texture();

private:
    unsigned int m_renderer_id;
    std::string m_filepath;
    unsigned char *m_local_buffer;
    int m_width    = 0;
    int m_height   = 0;
    int m_channels = 0;
};
