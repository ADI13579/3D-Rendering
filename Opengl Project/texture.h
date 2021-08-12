#pragma once
#include<stb/stb.h>
#include<string>
#include<vector>
#include"coordinate.h"

class texture
{
public:
    std::string filename;
    int width, height, channel;
    int desired_channel;
    std::vector <std::vector<coordinate3f>> imagedata;
    texture(){
        width = height = channel = 0;
        desired_channel = 3;
        filename = "";
    };
    
    texture(std::string _filename)
    {
        width = height = channel = 0;
        desired_channel = 3;
        filename = _filename;
    }

    bool load()
    {
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data;
        data = stbi_load(filename.c_str(), &width, &height, &channel, desired_channel);
        
        if (!data)
        {
            std::cout << filename << "  couldn't be loaded" << std::endl;
            return 0;
        }

        std::cout << filename << " loaded" << std::endl;
        convert(data);
        stbi_image_free(data);
        return 1;
    }
    void convert(stbi_uc* data)
    {
        stbi_uc* tdata = data;
        std::vector<std::vector<coordinate3f>> imaged(height, std::vector<coordinate3f>(width,coordinate3f()));

        for (int i = 0; i < height ; i++)
        {
            for (int j = 0; j < width; j++)
            {
                coordinate3f temp;
                temp.x = float(*tdata); tdata++;
                temp.y = float(*tdata); tdata++;
                temp.z = float(*tdata); tdata++;
                imaged[i][j] = temp / 255;
            }
        }
        imagedata.resize(imaged.size());
        imagedata = imaged;
    }
    void print()
    {
        std::cout<<"filename"<<filename<<std::endl;
        std::cout<<"width"<<width<<std::endl;
        std::cout<<"height"<<height<<std::endl;
        std::cout<<"height"<<height<<std::endl;
    }
};