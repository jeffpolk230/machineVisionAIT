/* 
 * Some part of this source code are from the facerec_demo.cpp of OpenCV examples.
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 */

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

/* get a list of filenames from directory */
int getFilenamesFromDir(string dir, vector<string> &files);
int getSubDirsFromDir(string dir, vector<string> &files);
static Mat toGrayscale(InputArray _src);

int main(int argc, char *argv[]) {
  string dir="orl_faces/";

  vector<string> subdirs;

  if (getSubDirsFromDir(dir, subdirs) == -1) {
    cout << "Cannot get subdirs from directory : " << dir << endl;
    exit(-1);
  }

  vector<vector<string> > filesInSubdir;
  vector<int> labelsSubdir;

  for(unsigned int i=0;i<subdirs.size();i++){
    string subdir=dir+subdirs[i]+"/";
    string label=subdirs[i].substr(1,subdirs[i].length()-1);
    int iLabel = atoi(label.c_str());
    vector<string> files;
    if (getFilenamesFromDir(subdir, files) == -1) {
      cout << "Cannot get subdirs from directory : " << subdir << endl;
      exit(-1);
    }

    filesInSubdir.push_back(files);
    labelsSubdir.push_back(iLabel);
  }
  // === Start working from here ===
  // Thanks :P
  vector<Mat> images;
  vector<int> labels; //correspoding labels of image? name?
  vector<Mat> testSamples;
  vector<int> testLabels;
  for (unsigned int i = 0; i<filesInSubdir.size();i++){
    for(unsigned int j = 0; j< filesInSubdir[i].size();j++){
      Mat image = imread(filesInSubdir[i][j],0);
      //first image for testing
      //Better than j = 0 ?
      if (j!=0){
        images.push_back(image);
        labels.push_back(labelsSubdir[i]);
      }
      else{
        testSamples.push_back(image);
        testLabels.push_back(labelsSubdir[i]);
      }
    }
  }
  //
  // Show some eigenfaces here
  //
  Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
  model -> train(images,labels);
  for(unsigned int i = 0 ; i < testSamples.size(); i++){
    int predictedLabel = model -> predict(testSamples[i]);
    string result_message = format("predicted class = %d / Actual class = %d ",
                                  predictedLabel, testLabels[i]);
    cout << result_message;
    if(predictedLabel == testLabels[i])
      cout << ": Bingo :D\n";
    else{
      cout << ": incorrect \n";
      imshow("incorrect image", testSamples[i]);
      waitKey(0);
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

static Mat toGrayscale(InputArray _src) {
    Mat src = _src.getMat();
    // only allow one channel
    if(src.channels() != 1) {
        CV_Error(CV_StsBadArg, "Only matrices with one channel are supported");
    }
    // create and return normalized image
    Mat dst;
    cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
    return dst;
}

