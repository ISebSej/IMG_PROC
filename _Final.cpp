#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <conio.h>

struct objectInfo {
	float xbar, ybar, M00;
};


int max(int hist[256]) {
	int maximum, size, c, location = 1;
	maximum = hist[0];
	for (c = 1; c < 256; c++)
	{
		if (hist[c] > maximum)
		{
			maximum = hist[c];
			location = c + 1;
		}
	}
	return maximum;
}

void cvThresh(IplImage *Image, IplImage *binImage, unsigned char threshold) {
	// Performs image thresholding. The user sets a threshold and the function checks whether the pixel value is above or below, 
	// it then draws a "binary" image from the thresholding results.
	// there are some issues for some fucking reason where it does not like 0x00 and 0xFF on paint images and I have no clue why, might be fixed later.

	if (!Image) {
		printf("Input image is NULL");
		system("pause");
	}
	else {
		int wideStep = Image->widthStep;

		char *locOriginal, *locBin;
		locOriginal = Image->imageData;              //find starting location of raw pixel data in original
		locBin = binImage->imageData;

		for (int pixel = 0; pixel < Image->width - 1; pixel++) {
			for (int row = 0; row < Image->height - 1; row++) {
				unsigned char val = *(locOriginal + pixel + row * wideStep);

				if (val > threshold) {
					*(locBin + pixel + row * wideStep) = 255; // Write black to pixel above threshold
				}
				else {
					*(locBin + pixel + row * wideStep) = 0; // write white to pixel below threshold
				}

			}
		}
	}
}

IplImage * cvHist(IplImage *originalImage) {
	if (originalImage->nChannels == 1) {

		IplImage *Hist;
		int histogram[256];
		float newhist[256];
		int k = 256;
		unsigned char value = 0;
		int width = originalImage->width;            // get image size of original image
		int height = originalImage->height;
		char *loc;              // Init location pointer variables
		loc = originalImage->imageData;      //find starting location of raw pixel data in original
		while (k-- > 0) histogram[k] = 0;

		for (int i = 0; i < width*height; i++) {
			value = *(loc + i);
			histogram[value] += 1;
		}
		// Create empty palet for histogram
		Hist = cvCreateImage(cvSize(276, 130), IPL_DEPTH_8U, 3);
		// Set background colour
		cvSet(Hist, CV_RGB(234, 13, 52));

		// draw axis
		cvLine(Hist, { 10,10 }, { 10, 111 }, { 0,0,0 }, 1);
		cvLine(Hist, { 10, 111 }, { 266, 111 }, { 0,0,0 }, 1);
		// "normalize" histogram data
		for (int i = 0; i < 256; i++) {
			newhist[i] = (float)histogram[i] * 100 / max(histogram);
		}

		for (int i = 0; i < 256; i++) {
			cvLine(Hist, { 10 + i + 1, 110 - (int)newhist[i] }, { 10 + i + 1, 110 }, { 255,255,255 }, 1);
		}
		return Hist;
	}
	else {
		printf("Input image is not a single channel image");
		return NULL;
	}
}

IplImage * cvGray(IplImage *originalImage) {
	// Checking if image exists
	if (!originalImage) {
		printf("Input image is NULL");
		system("pause");
		return NULL;
	}
	// Init variables
	IplImage *grayImage;                        // Init new gray image variable
	int width = originalImage->width;            // get image size of original image
	int heigth = originalImage->height;
	char gray;
	grayImage = cvCreateImage(cvSize(originalImage->width, originalImage->height), IPL_DEPTH_8U, 1);
	// Create empty single channel image for grayscale
	int wideStep = originalImage->widthStep; // get variable for widthStep, used to step through rows of image
	int wideStep2 = grayImage->widthStep;
	char *locOriginal, *locGray;                // Init location pointer variables
	locOriginal = originalImage->imageData;      //find starting location of raw pixel data in original
	locGray = grayImage->imageData;              //find starting location of raw pixel data in grayscale

	// Start of pixel manipulation
	// Start by access raw RGB data of the original image per pixel coordinate
	// Average these values
	// Save new pixel into the empty grayScale image at the same pixel coordinate
	for (int pixel = 0; pixel < originalImage->width - 1; pixel++) {
		for (int row = 0; row < originalImage->height - 1; row++) {
			char B = *(locOriginal + 0x00 + pixel * 0x03 + row * wideStep);
			char G = *(locOriginal + 0x01 + pixel * 0x03 + row * wideStep);
			char R = *(locOriginal + 0x02 + pixel * 0x03 + row * wideStep);
			//acces individual colour channels in [pixel, row]
			gray = (R + G + B) / 3;             //average pixel to find grayscale

			*(locGray + pixel + row * wideStep2) = gray;
		}
	}

	return grayImage;                           // Return Original Image
}

IplImage * cvSplit(IplImage *originalImage, IplImage *Red, IplImage *Green, IplImage *Blue) {
	// Checking if image exists
	if (!originalImage) {
		printf("Input image is NULL");
		system("pause");
		return NULL;
	}
	// Init variables                      
	int width = originalImage->width;            // get image size of original image
	int heigth = originalImage->height;
	char gray;

	// Create empty single channel image for grayscale
	int wideStep = originalImage->widthStep; // get variable for widthStep, used to step through rows of image
	int wideStep2 = Blue->widthStep;
	char *locOriginal, *locR, *locG, *locB;                // Init location pointer variables
	locOriginal = originalImage->imageData;      //find starting location of raw pixel data in original
	locG = Green->imageData;                 //find starting location of raw pixel data in grayscale
	locR = Red->imageData;
	locB = Blue->imageData;
	// Start of pixel manipulation
	// Start by access raw RGB data of the original image per pixel coordinate
	// Average these values
	// Save new pixel into the empty grayScale image at the same pixel coordinate
	for (int pixel = 0; pixel < originalImage->width - 1; pixel++) {
		for (int row = 0; row < originalImage->height - 1; row++) {
			char B = *(locOriginal + 0x00 + pixel * 0x03 + row * wideStep);
			char G = *(locOriginal + 0x01 + pixel * 0x03 + row * wideStep);
			char R = *(locOriginal + 0x02 + pixel * 0x03 + row * wideStep);

			*(locG + pixel + row * wideStep2) = G;
			*(locR + pixel + row * wideStep2) = R;
			*(locB + pixel + row * wideStep2) = B;
		}
	}

	return Green, Blue, Red;                           // Return Original Image
}

int  cvArea(IplImage *originalImage) {
	int wideStep = originalImage->widthStep;
	char *locOriginal;
	locOriginal = originalImage->imageData;
	int area = 0;
	for (int pixel = 0; pixel < originalImage->width - 1; pixel++) {
		for (int row = 0; row < originalImage->height - 1; row++) {
			char val = *(locOriginal + pixel + row * wideStep);
			if (val == (char)0) area++;
		}
	}
	return area;
}

struct objectInfo cvCenterOfMass(IplImage *originalImage, IplImage *drawImage) {
	int M00 = cvArea(originalImage);

	int wideStep = originalImage->widthStep;
	char *locOriginal;
	locOriginal = originalImage->imageData;
	int M10 = 0;
	int M01 = 0;

	for (int pixel = 0; pixel < originalImage->width - 1; pixel++) {
		for (int row = 0; row < originalImage->height - 1; row++) {
			char val = *(locOriginal + pixel + row * wideStep);

			if (val == (char)0) {
				M10 += pixel;
				M01 += row;
			}
		}
	}
	float xbar = (float)M10 / (float)M00;
	float ybar = (float)M01 / (float)M00;
	printf("COM located at (%f, %f)\n", xbar, ybar);
	cvCircle(drawImage, cvPoint(xbar, ybar), 3, cvScalar(0, 128, 255));
	struct objectInfo myobject = { xbar, ybar, M00 };
	return myobject;
}

float cvCentralMoment(IplImage *originalImage, IplImage *drawImage) {
	struct objectInfo myobject = cvCenterOfMass(originalImage, drawImage);
	float xbar = myobject.xbar;
	float ybar = myobject.ybar;
	//float M00 = myobject.M00;
	int wideStep = originalImage->widthStep;
	char *locOriginal;
	locOriginal = originalImage->imageData;
	float mu11 = 0;
	float mu20 = 0;
	float mu02 = 0;

	for (int pixel = 0; pixel < originalImage->width - 1; pixel++) {
		for (int row = 0; row < originalImage->height - 1; row++) {
			char val = *(locOriginal + pixel + row * wideStep);

			if (val == (char)0) {
				mu11 += (pixel - xbar)  * (row - ybar);
				mu20 += (pixel - xbar)  * (pixel - xbar);
				mu02 += (row - ybar)    * (row - ybar);
			}
		}
	}
	float theta = atan(2 * mu11 / (mu20 - mu02)) / 2;
	printf("Central Moment is %f", theta);
	cvLine(drawImage, cvPoint(xbar - 100, ybar - tan(theta - 3.14 / 2) * 100), cvPoint(xbar + 100, ybar + tan(theta - 3.14 / 2) * 100), cvScalar(0, 128, 255));
	cvCircle(drawImage, cvPoint(xbar, ybar), 3, cvScalar(0, 128, 255));

	//calculating the invariant moment
	int M00 = cvArea(originalImage);

	float mu00 = 0;
	float ni20 = 0;
	float ni02 = 0;
	float inv = 0;

	mu00 = M00;
	ni20 = mu20 / (mu00 * mu00);
	ni02 = mu02 / (mu00 * mu00);
	inv = ni20 + ni02;
	printf("Moment Invariance is %f", inv);

	return xbar, ybar;
}

IplImage * cvNewWindow(IplImage *frame_gray, int a) {
	if (a == 1) {
		const char* newPhoto1 = "First captured image"; // window name
		cvNamedWindow(newPhoto1, 0);            // create simple window
		cvShowImage("newPhoto1.jpg", frame_gray);
		cvSaveImage("newPhoto1.jpg", frame_gray);
		a++;
	}
	else if (a == 2) {
		const char* newPhoto2 = "Second captured image";    // window name
		cvNamedWindow(newPhoto2, 0);            // Creat image window
		cvShowImage("newPhoto2.jpg", frame_gray);
		cvSaveImage("newPhoto2.jpg", frame_gray);
		a = 0;
	}
	return frame_gray;
}

IplImage * cvLaplacianContour(IplImage *originalImage) {
	IplImage * lcfiltered = cvCreateImage(cvSize(originalImage->width, originalImage->height), IPL_DEPTH_8U, 1);

	int wideStep = originalImage->widthStep;
	char *locOriginal, *newLoc;
	locOriginal = originalImage->imageData;
	newLoc = lcfiltered->imageData;
	char lp = 0;
	int newWideStep = lcfiltered->widthStep;
	int prevRow = 0;
	int prevCol = 0;
	int imagewidth = originalImage->width;
	int imageheight = originalImage->height;
	int counter;

	int LM[] = { 1, 4, 1, 4, -20, 4, 1, 4, 1 };

	for (int pixel = 1; pixel < imagewidth - 1; pixel++) {
		for (int row = 1; row < imageheight - 1; row++) {

			counter = 0;
			for (signed int lcol = -1; lcol < 2; lcol++) {
				for (signed int lrow = -1; lrow < 2; lrow++) {

					if (pixel + lrow < 0 || pixel + lrow > imagewidth || row + lcol < 0 || row + lcol > imageheight) {
						lp += 0;
						counter++;
					}
					else {
						lp += *(locOriginal + (pixel + lrow) + (row + lcol) * wideStep)
							* LM[counter] * (1 / (6 * ((pixel*wideStep - prevCol)*(pixel*wideStep - prevCol)) + ((row - prevRow)*(row - prevRow))));

						counter++;
						prevCol = pixel;
						prevRow = row;
					}
				}
			}
		}
	}
	return lcfiltered;
}

void cvContourSearch(IplImage *originalImage, IplImage *pic) {
	//Init function variables
	int rimx[32767], rimy[32767];
	int x = 0;
	int y = 0;
	int draw_type = 0;
	int pos, newpos = 0;

	// Init variables for return picture
	cvSet(pic, cvScalar(0));
	char * locPic = pic->imageData;
	
	// Init variables of original binary image
	char * locOriginal = originalImage->imageData;
	int wideStep = originalImage->widthStep;
	int imagewidth = originalImage->width;
	int imageheight = originalImage->height;
	// start location x and y, should be 255 value and along the border of the image! Make sure object does not exceed image borders
	// TO DO: make function that finds start position
	x = 369;
	y = 70;
	pos = y * wideStep + x;
	newpos = pos;
	unsigned int local_tresh = 120;
	int count = 0;
	int imagelength = imagewidth * imageheight;

	while (newpos >= 0 && newpos < imagelength) // while inside the image, should break on closed contour or exceeded contour length
	{
		// save contour positions 
		rimx[count] = newpos % wideStep;
		rimy[count] = newpos / wideStep;
		count++;

		// determine new search direction
		draw_type = (draw_type + 6) % 8;    
		switch (draw_type)
		{
		case 0: // right
			if (*(locOriginal + newpos + 1) > local_tresh) {
				newpos += 1;
				draw_type = 0;
				break;
			}
		case 1: // right and down
			if (*(locOriginal + newpos + 1 + wideStep) > local_tresh) {
				newpos += wideStep + 1;
				draw_type = 1;
				break;
			}
		case 2: // down
			if (*(locOriginal + newpos + wideStep) > local_tresh) {
				newpos += wideStep;
				draw_type = 2;
				break;
			}
		case 3: // down and left
			if (*(locOriginal + newpos + wideStep - 1) > local_tresh) {
				newpos += wideStep - 1;
				draw_type = 3;
				break;
			}
		case 4: // left
			if (*(locOriginal + newpos - 1) > local_tresh) {
				newpos -= 1;
				draw_type = 4;
				break;
			}
		case 5: // left and up
			if (*(locOriginal + newpos - wideStep - 1) > local_tresh) {
				newpos -= wideStep + 1;
				draw_type = 5;
				break;
			}
		case 6: // up 
			if (*(locOriginal + newpos - wideStep) > local_tresh) {
				newpos -= wideStep;
				draw_type = 6;
				break;
			}
		case 7: // up and right
			if (*(locOriginal + newpos - wideStep + 1) > local_tresh) {
				newpos -= wideStep - 1;
				draw_type = 7;
				break;
			}
		case 8: // right
			if (*(locOriginal + newpos + 1) > local_tresh) {
				newpos += 1;
				draw_type = 0;
				break;
			}
		case 9: // right and down
			if (*(locOriginal + newpos + wideStep + 1) > local_tresh) {
				newpos += wideStep + 1;
				draw_type = 1;
				break;
			}
		case 10: // down
			if (*(locOriginal + newpos + wideStep) > local_tresh) {
				newpos += wideStep;
				draw_type = 2;
				break;
			}
		case 11:// down and left
			if (*(locOriginal + newpos + wideStep - 1) > local_tresh) {
				newpos += wideStep - 1;
				draw_type = 3;
				break;
			}
		case 12: // left 
			if (*(locOriginal + newpos - 1) > local_tresh) {
				newpos -= 1;
				draw_type = 4;
				break;
			}
		case 13: // left and up
			if (*(locOriginal + newpos - wideStep - 1) > local_tresh) {
				newpos -= wideStep + 1;
				draw_type = 5;
				break;
			}
		case 14: // up
			if (*(locOriginal + newpos - wideStep) > local_tresh) {
				newpos -= wideStep;
				draw_type = 6;
				break;
			}

		}
		if (newpos == pos) break; // check if contour is closed
		if (count >= 32767) break; // check if index is exceded 
	}

	// draw contour to pic image
	int i = 0;
	while (rimx[i] > 0 && i < 32767 ) {
		*(locPic + rimx[i] + rimy[i] * wideStep) = 255;
		i++;
	}
	printf("end");
}

IplImage * cvhighPassFlter(IplImage *originalImage, IplImage *grayhpfiltered) {

	grayhpfiltered = cvCreateImage(cvSize(originalImage->width, originalImage->height), IPL_DEPTH_8U, 1);

	int wideStep = originalImage->widthStep;
	char *locOriginal, *newLoc;
	locOriginal = originalImage->imageData;
	newLoc = grayhpfiltered->imageData;

	int newWideStep = grayhpfiltered->widthStep;

	int imagewidth = originalImage->width;
	int imageheight = originalImage->height;
	for (int pixel = 0; pixel < imagewidth - 1; pixel++) {
		for (int row = 0; row < imageheight - 1; row++) {

			char hp = ((pixel - 1)*imagewidth + row - 1) * 0 + ((pixel - 1)*imagewidth + row) * (-1 / 4) + ((pixel - 1)*imagewidth + row + 1) * 0
				+ (pixel * imagewidth + row - 1) * (-1 / 4) + (pixel * imagewidth + row) * 2 + (pixel * imagewidth + row + 1) * (-1 / 4)
				+ ((pixel + 1) * imagewidth + row - 1) * 0 + ((pixel + 1) * imagewidth + row) * (-1 / 4) + ((pixel + 1)* imagewidth + row + 1) * 0;
			*(newLoc + pixel + row * newWideStep) = hp;

		}
	}
	return grayhpfiltered;
}

IplImage * cvLaplaceGaussian(IplImage *originalImage) {
	IplImage *Result;
	Result = cvCreateImage(cvSize(originalImage->width, originalImage->height), IPL_DEPTH_8U, 1);

	int wideStep = originalImage->widthStep;
	char *locOriginal, *newLoc;
	locOriginal = originalImage->imageData;
	newLoc = Result->imageData;

	int newWideStep = Result->widthStep;

	int imagewidth = originalImage->width;
	int imageheight = originalImage->height;

	char Gij[] = {
		0, 0, 1, 2, 2, 2, 1, 0, 0,
		0, 1, 5, 10, 12, 10, 5, 1, 0,
		1, 5, 15, 19, 16, 19, 15, 5, 1,
		2, 10, 19, -19, -64, -19, 19, 10, 2,
		2, 12, 16, -64, -148, -64, -19, 19, 10, 2,
		2, 10, 19, -19, -64, -19, 19, 10, 2,
		1, 5, 15, 19, 16, 19, 15, 5, 1,
		0, 1, 5, 10, 12, 10, 5, 1, 0,
		0, 0, 1, 2, 2, 2, 1, 0, 0
	};
	int counter;
	for (int pixel = 0; pixel < imagewidth - 1; pixel++) {
		for (int row = 0; row < imageheight - 1; row++) {
			counter = 0;
			char val = 0;

			for (signed int gauscol = -4; gauscol < 5; gauscol++) {
				for (signed int gauspix = -4; gauspix < 5; gauspix++) {
					if (pixel + gauspix < 0 || pixel + gauspix > imagewidth || row + gauscol < 0 || row + gauscol > imageheight) {
						val += 0;
						counter++;
					}
					else {
						val += (*(locOriginal + (pixel + gauspix) + (row + gauscol) * wideStep)) * Gij[counter];
						counter++;
					}
				}
			}

			*(newLoc + pixel + row * newWideStep) = val;
		}
	}
	return Result;
}

IplImage * cvLaplacianMethod(IplImage * originalImage, IplImage *laplaceImage) {        //sth is wrong

	laplaceImage = cvCreateImage(cvSize(originalImage->width, originalImage->height), IPL_DEPTH_8U, 1);

	int newWideStep = laplaceImage->widthStep;
	int wideStep = originalImage->widthStep;
	int prevCol = 0, prevRow = 0;

	int imagewidth = originalImage->width;
	int imageheight = originalImage->height;
	char *locOriginal, *newLoc;
	locOriginal = originalImage->imageData;
	newLoc = laplaceImage->imageData;
	char lm = 0;
	for (int col = 1; col < imagewidth - 1; col++) {
		for (int row = 1; row < imageheight - 1; row++) {


			lm += *locOriginal + (1 / (6 * ((col - prevCol)*(col - prevCol)) + ((row - prevRow)*(row - prevRow))))
				*(((col - 1)*imagewidth + row - 1) + ((col - 1)*imagewidth + row) * 4 + ((col - 1)*imagewidth + row + 1)
					+ (col * imagewidth + row - 1) * 4 + (col * imagewidth + row) * (-20) + (col * imagewidth + row + 1) * 4
					+ ((col + 1) * imagewidth + row - 1) + ((col + 1) * imagewidth + row) * 4 + ((col + 1)* imagewidth + row + 1));

			prevCol = col;
			prevRow = row;

			*(newLoc + col + row * newWideStep) = lm;
		}
	}
	return laplaceImage;
}


void cvCorrespond(IplImage * IMG1, IplImage * IMG2, int numPoints) {
	// Init image properties
	char* loc1	= IMG1->imageData; 
	int width1	= IMG1->width;	
	int height1 = IMG1->height;
	int widestep1 = IMG1->widthStep;
	char* loc2	= IMG2->imageData;
	int width2	= IMG2->width;
	int height2 = IMG2->height;
	int widestep2 = IMG2->widthStep;
	// initialize random number generator
	srand(time(NULL));
	int x1max = width1 - 5; // maximum start location for 10x10 matrix
	int y1max = height1 - 5;
	// Init result variables
	int Points[300][3]; // Corresponding points. //layout [x1, y1, x2, y2]
	int count = 0; // length of Points
	int itter = 0; // itteration in while loop
	// continues until desired number of points is achieved or a timeout is reached
	while (count < numPoints ) {
		bool match = false; // stores wether a match has been found this itteration
		// generate random initial conditions inside the image frame
		int x1 = 0;
		while (x1 < 5 or x1 > x1max) {
			x1 = rand(); // generate point between 0 and RAND_MAX
		}  // check if inside image
		int y1 = 0;
		while (y1 < 5 or y1 > y1max) {
			y1 = rand(); // generate point between 0 and RAND_MAX
		}  // check if inside image
		// initialize feature of IMG1
		unsigned char feature1[9][9];
		// load feature matrix from image
		for (int col = -4; col < 5; col++) {
			for (int row = -4; row < 5; row++) {
				unsigned char data = *(loc1 + (x1 + row) + (y1 + col) * widestep1);
				feature1[row + 4][col + 4] = data; // 9x9 matrix contain the feature to be found
			}
		}

		// Loop through second image and find matches
		// initialize matrix to save results
		int MatchResults[30000][3]; // savematch results // notation x2, y2, score
		int matchcount = 0; 
		// go through IMG2 and search for features
		for (int pixel = 5; pixel < width2 - 5; pixel += 3) {
			for (int column = 5; column < height2 - 5; column+= 2) {
				unsigned int score = 0;
				// go through 9x9 pixels surounding current location and compare to original
				for (int col = -4; col < 5; col++) {
					for (int row = -4; row < 5; row++) {
						unsigned char data1 = *(loc2 + (pixel + row) + (column + col) * widestep2);
						score += abs(feature1[row + 4][col + 4]  - data1) * abs(feature1[row + 4][col + 4] - data1); // 9x9 matrix contain the feature to be found
					}
				}
				// save match results
				MatchResults[matchcount][0] = pixel;
				MatchResults[matchcount][1] = column;
				MatchResults[matchcount][2] = score;
				matchcount++;
			}
		}
		
		// Analyse match results to find best match
		// go through scores and find the best match
		int lowestMatchScore = MatchResults[0][2];;
		int x2 = MatchResults[0][0];;
		int y2 = MatchResults[0][1]; ;
		for (int i = 1; i < matchcount; i++) {
			if (MatchResults[i][2] < lowestMatchScore) {
				lowestMatchScore = MatchResults[i][2];
				x2 = MatchResults[i][0];
				y2 = MatchResults[i][1];
			}
			
		}
		//TODO: test statistical significance of match
		match = true;

		// check results for significant match
		if (match) {
			// draw circles on features
			cvCircle(IMG1, cvPoint(x1, y1), 7, cvScalar(0));
			cvCircle(IMG2, cvPoint(x2, y2), 7, cvScalar(0));
			cvCircle(IMG1, cvPoint(x1, y1), 8, cvScalar(255));
			cvCircle(IMG2, cvPoint(x2, y2), 8, cvScalar(255));
			count++;
		}
		
	}

	return;
}

int main(int argc, char* argv[])
{
		cv::setBreakOnError(true);
		// Init variables
		const char* wName = "IMG1";   // window name
		cvNamedWindow(wName, 0);                // Creat image window
		const char* eName = "IMG2";       // window name
		cvNamedWindow(eName, 0);                // Creat image window
		IplImage *Image, *IMG1, *IMG2, *IMG1G, *IMG2G;      // Init image variables


		// Original Image
		IMG1 = cvLoadImage("PIC1_L.png"); //load image
		IMG2 = cvLoadImage("PIC1_R.png"); //load image
		IMG1G = cvCreateImage(cvSize(IMG1->width, IMG1->height), IPL_DEPTH_8U, 1);
		IMG2G = cvCreateImage(cvSize(IMG2->width, IMG2->height), IPL_DEPTH_8U, 1);


		// Gray Scale Image 
		IMG1G = cvGray(IMG1);               //create gray scale image
		IMG2G = cvGray(IMG2);
		// Histogram of grayscale
		//Hist = cvHist(grayImage);


		//Binairy of grayscale
		//cvThresh(grayImage, binImage, (char)0x01);
		//cvCentralMoment(binImage, Image);

		// Gausian filter
		//Hist = cvLaplaceGaussian(cvLowPassFilter(grayImage));

		cvCorrespond(IMG1G, IMG2G, 40);

		// Display images on screen
		cvShowImage(wName, IMG1G);              // diplay Image in image window
		cvShowImage(eName, IMG2G);          // display gray scale

		// resize image windows to original window size
		cvResizeWindow(wName, IMG1->width, IMG1->height);
		cvResizeWindow(eName, IMG2->width, IMG2->height);

		// Close program
		printf("\n Finished \n");
		cvWaitKey(0);
		system("pause");
}
