// Takes one input image and construct image pyramids for further use

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

//Global vars
Mat src, dst, tmp;
char* window_name = "pyramids";


int main( int argc, char** argv )
{
	src = imread(argv[1]);
	if (!src.data)
	{
		printf("Wrong input, no data");
		return -1;
	}
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	imshow( window_name, src );
	
}