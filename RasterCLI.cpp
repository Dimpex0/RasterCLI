#include <iostream>
#include "CommandProcessor.h"
#include <string>
#include "ImagePBM.h"
#include "ImagePGM.h"

int main()
{
    //CommandProcessor processor;
    //processor.run();
    Image* image = new ImagePGM("C:\\Users\\mitko\\Downloads\\sample_640x426.pgm");
    //image->flipLeft();
    //image->flipTop();
    image->negative();
    image->rotateRight();
    image->save();
    delete image;
}
