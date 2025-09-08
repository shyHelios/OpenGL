#pragma once

class FrameBuffer
{
public:
    FrameBuffer(int InWidth, int InHeight);
    ~FrameBuffer();

    void Bind();
    void Unbind();

    unsigned int GetTexColorBufferID() const;
    int GetWidth() const;
    int GetHeight() const;

private:
    unsigned int RenderID;
    unsigned int TexColorBufferID;
    int Width;
    int Height;
};
