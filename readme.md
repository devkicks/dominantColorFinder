dominantColorFinder
===================

Prototype OpenCV application that extracts dominant color of objects imaged at the center of the image

Assumptions
===========
I have addressed this question based on the fact that for a given region there could be an object in the foreground with a fixed background.
Moreover, I have assumed that the object will occupy the most pixels in the center of the image. Using these assumptions, I have come up
with a simple yet robust solution that is based on KMeans clustering, where I cluster the colors into the background and foreground pixels.

Moreover, to make the solution a little more robust to different illumination conditions I have utilized only H channel in HSV space. H channel
encode the hue independent of the brightness/contrast, which is a true representation of the underlying color.

To extract the dominant color, I have averaged across all points from the foreground cluster. This is a very basic operation and can be further improved 
by using MEDIAN or RANSAC model, which are more robust to outliers. 

Possible Extensions
===================
- Use K> 2 in KMeans to describe more than one dominant color in a given object
- Use of RANSAC or MEDIAN for computing dominant color from a given set of points. This is to make it robust to outliers
- Apply Histogram Equalization to preprocess the image
- Perhaps a region growing-based algorithm can be used to extract regions with consistent color
- Possible to use depth sensors (e.g. Kinect) to improve segmentation
- If object is of known shape/size (e.g. hands, face?), we can utilize haar-like features to detect regions of interest (similar to Viola-Jones)



Examples
========

<img src="https://github.com/devkicks/dominantColorFinder/blob/master/DominantColorFinder/DominantColorFinder/output/1/imageGif.gif" alt="Color Image" width="400"/>
<img src="https://github.com/devkicks/dominantColorFinder/blob/master/DominantColorFinder/DominantColorFinder/output/2/imageGif.gif" alt="Color Image" width="400"/>
<img src="https://github.com/devkicks/dominantColorFinder/blob/master/DominantColorFinder/DominantColorFinder/output/3/imageGif.gif" alt="Color Image" width="400"/>
<img src="https://github.com/devkicks/dominantColorFinder/blob/master/DominantColorFinder/DominantColorFinder/output/4/imageGif.gif" alt="Color Image" width="400"/>
