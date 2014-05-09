#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/flann.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

/* get a list of filenames from directory */
int getFilenamesFromDir(string dir, vector<string> &files);
int getSubDirsFromDir(string dir, vector<string> &files);
double myDistanceSqr(Mat v1, Mat v2);

//For Comparison
double histogram_diff(vector<float> h1, vector<float> h2){
  double diff = 0.0;
  for(unsigned int i = 0 ;i < h1.size(); i++){
    diff += (h1[i]-h2[i])*(h1[i]-h2[i]);
  }
  return diff;
}

int main(int argc, char *argv[]) {
  string dir="data/";

  vector<vector<string> > filesInSubdir;
  vector<string> labelSubdirs;

  if (getSubDirsFromDir(dir, labelSubdirs) == -1) {
    cout << "Cannot get subdirs from directory : " << dir << endl;
    exit(-1);
  }

  for(unsigned int i=0;i<labelSubdirs.size();i++){
    string subdir=dir+labelSubdirs[i]+"/";

    vector<string> files;
    if (getFilenamesFromDir(subdir, files) == -1) {
      cout << "Cannot get subdirs from directory : " << subdir << endl;
      exit(-1);
    }

    filesInSubdir.push_back(files);
  }
  // === Start working from here ===

  Mat src;
  Mat src_gray;
  vector<vector<Mat> > allFeatures;
  for(unsigned int i = 0; i<filesInSubdir.size(); i++){
    vector<Mat> features;
    for(unsigned int j = 0 ; j < filesInSubdir[i].size();j++){
      src = imread(filesInSubdir[i][j]);
      namedWindow("source_window", CV_WINDOW_NORMAL);
      imshow("source_window", src);
      waitKey(1);
      cvtColor(src, src_gray, CV_BGR2GRAY);
      SurfFeatureDetector detector(400);
      vector<KeyPoint> keypoints;
      detector.detect(src_gray, keypoints);
      SurfDescriptorExtractor extractor;
      Mat descriptors;
      extractor.compute(src_gray, keypoints, descriptors);

      cout << "Shoes: " << labelSubdirs[i]
           << "Image: " << j
           << ", #KeyPoint: " << keypoints.size() << endl;
      // namedWindow("keypoint_window", CV_WINDOW_NORMAL);
      // Mat imgKeypoint;
      // drawKeypoints(src, keypoints, imgKeypoint);
      // imshow("keypoint_window", imgKeypoint);
      features.push_back(descriptors);
      // waitKey(10);
    }
    allFeatures.push_back(features);
  }
  // Okay, they're clearly gonna abuse the var names
  Mat features; 
  for (unsigned int i = 0; i < allFeatures.size(); i++){
    for (unsigned int j = 0; j < allFeatures[i].size()-1; j++){
      features.push_back(allFeatures[i][j]);
      //last image is used for testing 
    }
  }
  Mat clusterCenters;
  cvflann::KMeansIndexParams fln_idx =
    cvflann::KMeansIndexParams(2, 200, cvflann::CENTERS_RANDOM, 0.0 );
  int nClusters = 100;
  clusterCenters = Mat::zeros(nClusters, features.cols, CV_32F);
  nClusters = flann::hierarchicalClustering<flann::L2<float> >(features, clusterCenters, fln_idx);

  vector<vector<vector<float> > > allHistograms;
  for(unsigned int i =0; i<allFeatures.size(); i++){
    vector<vector<float> >histograms;
    for(unsigned int j=0; j<allFeatures[i].size();j++){
      Mat fea=allFeatures[i][j];
      vector<float> hist;
      hist.assign(nClusters, 0.0);
      for (int n = 0; n <fea.rows; n++){
        Mat f=fea.row(n);
        double min = FLT_MAX;
        int min_idx=-1;
        for (int c = 0 ;c < nClusters; c++){
          double tmp=myDistanceSqr(f,clusterCenters.row(c));
          if(tmp<min){
            min = tmp;
            min_idx = c;
          }
        }
        hist[min_idx] += 1.0;
        }
        for(int h = 0;h < nClusters;h++){
        hist[h]/=fea.rows;
      }
      histograms.push_back(hist);
    }
    allHistograms.push_back(histograms);
  }

  vector<vector<float> > histClasses;
  vector<vector<float> > occurances;
  for(unsigned int i = 0 ; i < labelSubdirs.size(); i++){
    vector<float> hist;
    hist.assign(nClusters, 0.0);
    vector<float> occur;
    occur.assign(nClusters, 0.0);
    occurances.push_back(occur);
    histClasses.push_back(hist);
  }
  for(unsigned int i =0; i< labelSubdirs.size();i++){
    for(unsigned int j = 0; j< filesInSubdir[i].size()-1 ; j++){
      for(int c = 0 ; c < nClusters; c++){
        histClasses[i][c] += allHistograms[i][j][c];
        if (allHistograms[i][j][c] > 0)
          occurances[i][c] += 1;
      }
    }
    cout << labelSubdirs[i] << ":";
    for (int c =0; c < nClusters ; c++){
      histClasses[i][c] /= filesInSubdir[i].size(); //TF
      occurances[i][c] = (log(filesInSubdir[i].size()) > log((occurances[i][c]+1))) ? (log(filesInSubdir[i].size()) - log((occurances[i][c]+1))) : 0.0;

       // histClasses[i][c] *= occurances[i][c];
       // cout << histClasses[i][c] << ": " << occurances[i][c] << "\n";
    }
     // cout << endl;
  }

  for(unsigned int i=0;i<allHistograms.size();i++){
         for(unsigned int j=0;j<allHistograms[i].size();j++){
           double dist1,dist2;
           dist1 = histogram_diff(allHistograms[i][j],histClasses[0]);
           dist2 = histogram_diff(allHistograms[i][j],histClasses[1]);
           int predictedLabel;
           if(dist1 < dist2)
             predictedLabel = 0;
           else
             predictedLabel = 1;
           cout << "Shoes: " << labelSubdirs[i]
                << ", image " << j << " is recognized as "
                << labelSubdirs[predictedLabel] << endl;
           if(predictedLabel != (int)i){
             src = imread(filesInSubdir[i][j]);
             namedWindow("Incorrect recognition", CV_WINDOW_NORMAL);
             imshow("Incorrect recognition", src);
             waitKey(0);
            } 
          }
  }

  return 0;
}

int getFilenamesFromDir(string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    cout << "Error : cannot open directory : " << dir << endl;
    return -1;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type == DT_REG)
      files.push_back(dir+string(dirp->d_name));
  }
  closedir(dp);
  return 0;
}

int getSubDirsFromDir(string dir, vector<string> &files) {
  DIR *dp;
  struct dirent *dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    cout << "Error : cannot open directory : " << dir << endl;
    return -1;
  }

  while ((dirp = readdir(dp)) != NULL) {
    if (dirp->d_type == DT_DIR){
      if( strcmp(dirp->d_name,".") && strcmp(dirp->d_name,"..") )
	files.push_back(string(dirp->d_name));
    }
  }
  closedir(dp);
  return 0;
}

double myDistanceSqr(Mat v1, Mat v2) {
  if (v1.rows != v2.rows || v1.cols != v2.cols) {
    //cout << "Dimension is mismatch!!" << endl;
    return -1;
  }

  int i;
  double sum = 0.0;
  for (i = 0; i < v1.cols; i++) {
    double tmp = v1.at<float>(i) - v2.at<float>(i);
    sum += tmp * tmp;
  }
  return sum;
}

