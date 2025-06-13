#include <iostream>
#include <string>
#include "SessionCommandProcessor.h"
#include "Image.h"

int main()
{
    SessionCommandProcessor processor;
    processor.run(std::cin);
     
    
    //Image* image = new ImagePGM("C:\\Users\\mitko\\Downloads\\sample_640x426.pgm");
    //image->flipLeft();
    //image->flipTop();
    //image->monochrome();
    //image->negative();
    //image->rotateRight();
    //image->save("");
    //delete image;

    //Image* img1 = new ImagePBM("gerrymander.ascii.pbm");
    //Image* img2 = new ImagePBM("circle.pbm");
    //img1->paste(img2, 10, 20);
    //img1->save("");

    return 0;
}
