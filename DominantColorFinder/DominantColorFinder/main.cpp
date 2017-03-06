#include "opencv2\opencv.hpp"
/*
%Task: 1. To create a simple prototype C++ Windows application that uses a 
% web-camera to detect the dominant colour(s) of any object imaged at 
% the centre of the camera view. The task should be undertaken in 
% Visual Studio along with the OpenCV library.
% 
% NB. The camera can be assumed to be fixed or movable - but please state
% assumptions wherever made.
% 
%  
% 
% 2. Finally, please can you also suggest methods or approaches that could be
% used to help overcome any environmental factors that may influence the 
% resulting colour(s) deduced.
*/


/*
I have addressed this question based on the fact that for a given region there could be an object in the foreground with a fixed background.
Moreover, I have assumed that the object will occupy the most pixels in the center of the image. Using these assumptions, I have come up
with a simple yet robust solution that is based on KMeans clustering, where I cluster the colors into the background and foreground pixels.

Moreover, to make the solution a little more robust to different illumination conditions I have utilized only H channel in HSV space. H channel
encode the hue independent of the brightness/contrast, which is a true representation of the underlying color.

To extract the dominant color, I have averaged across all points from the foreground cluster. This is a very basic operation and can be further improved 
by using MEDIAN or RANSAC model, which are more robust to outliers. 
*/

#define C_WINDOW_WIDTH 100
#define C_WINDOW_HEIGHT 100

// function for extract H component of all points in a specified image
cv::Mat extractHPts(const cv::Mat& inImage)
{
	// container for storing Hue Points
	cv::Mat listOfHPts;
	listOfHPts = cv::Mat::zeros(inImage.cols*inImage.rows, 1, CV_32FC1);

	// index for listOfHPts container
	int idx = 0;
	for(int j = 0; j < inImage.rows; j++)
	{
		for(int i = 0; i < inImage.cols; i++)
		{
			cv::Vec3b tempVec;
			tempVec = inImage.at<cv::Vec3b>(j, i);

			// extract the H channel and store in Hpts list
			listOfHPts.at<float>(idx++, 0) = float(tempVec[0]);
		}
	}

	return listOfHPts;
}

// function for extracting dominant color from foreground pixels
cv::Mat getDominantColor(const cv::Mat &inImage, const cv::Mat &ptsLabel)
{
	// first we determine which cluster is foreground
	// assuming the our object of interest is the biggest object in the image region
	
	cv::Mat fPtsLabel, sumLabel;
	ptsLabel.convertTo(fPtsLabel, CV_32FC1);
	
	cv::reduce(fPtsLabel, sumLabel, 0, CV_REDUCE_SUM, CV_32FC1);

	int numFGPts = 0;

	if(sumLabel.at<float>(0, 0) < ptsLabel.rows/2)
	{
		// invert the 0's and 1's where 1s represent foreground
		fPtsLabel = (fPtsLabel-1)*(-1);
		numFGPts = fPtsLabel.rows - sumLabel.at<float>(0, 0);
	}
	else
		numFGPts = sumLabel.at<float>(0, 0);

	// to find dominant color, I just average all points belonging to foreground
	cv::Mat dominantColor;
	dominantColor = cv::Mat::zeros(1, 3, CV_32FC1);

	int idx = 0; int fgIdx = 0;
	for(int j = 0; j < inImage.rows; j++)
	{
		for(int i = 0; i < inImage.cols; i++)
		{
			if(fPtsLabel.at<float>(idx++, 0) == 1)
			{
				cv::Vec3b tempVec;
				tempVec = inImage.at<cv::Vec3b>(j, i);
				dominantColor.at<float>(0, 0) += (tempVec[0]);
				dominantColor.at<float>(0, 1) += (tempVec[1]);
				dominantColor.at<float>(0, 2) += (tempVec[2]);

				fgIdx++;
			}
		}
	}

	dominantColor /= numFGPts;

	// convert to uchar so that it can be used directly for visualization
	cv::Mat dColor;
	dColor = cv::Mat::zeros(1, 3, CV_8UC1);

	dominantColor.convertTo(dColor, CV_8UC1);
	
	//std::cout << "Dominant Color is: " <<  dColor << std::endl;

	return dColor;
}

// show region rectangle in a display image
void drawBoundingBox( cv::Mat &inImage, const cv::Rect &cROI)
{
	cv::rectangle(inImage, cROI, cv::Scalar(255), 3);
}

int main()
{
	cv::VideoCapture cameraFeed(0);

	char ch = ' ';
	int cIdx = 0;
	while(ch != 27)
	{
		cv::Mat inImage;
		
		// grab image
		cameraFeed >> inImage;
		
		// make a copy for displaying
		cv::Mat dispImage = inImage.clone();

		// create a RECT for ROI extraction and displaying output
		cv::Rect centerROI = cv::Rect(inImage.cols/2-C_WINDOW_WIDTH/2, inImage.rows/2-C_WINDOW_HEIGHT/2, C_WINDOW_WIDTH, C_WINDOW_HEIGHT);
		cv::Rect displayROI = cv::Rect(inImage.cols-C_WINDOW_WIDTH, 0, C_WINDOW_WIDTH, C_WINDOW_HEIGHT);

		// show region on image
		drawBoundingBox(dispImage, centerROI);

		// extract the centre region
		cv::Mat centerRegion = inImage(centerROI);

		// convert to HSV so as to make invariant to illumination changes
		cv::Mat hsvCenter;
		cv::cvtColor(centerRegion, hsvCenter, CV_BGR2HSV);

		// extract Hue pts
		cv::Mat hPts;
		hPts = extractHPts(hsvCenter);

		// cluster into two clusters for foreground and background
		cv::Mat ptsLabel, kCenters;
		cv::kmeans(hPts, 2, ptsLabel, cv::TermCriteria(cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 1000, 0.00001)), 5, cv::KMEANS_PP_CENTERS, kCenters);

		// get dominant color and display on output window
		cv::Mat dColor;
		dColor = getDominantColor(centerRegion, ptsLabel);
		cv::Mat displayRegion = dispImage(displayROI);
		displayRegion.setTo(cv::Scalar(dColor.at<uchar>(0, 0), dColor.at<uchar>(0, 1), dColor.at<uchar>(0, 2)));

		cv::imshow("Dominant Color Display", dispImage);
		ch = cv::waitKey(10);

		// if user wants to capture
		//if(ch == 'c')
		//{
			char buffer[256];
			sprintf(buffer, "output\\%.5d_outputImage.png", cIdx++);
			cv::imwrite(buffer, dispImage);
		//}
	}
	cameraFeed.release();
	return 1;
}