#pragma once
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include <fstream>

using namespace std;
using namespace cv;

struct MValues {
	string name;
	double s;
	cv::Point y;
};

class PoseEstimation {

public:vector<Mat> models_data, tests_data;
public:vector<String> models_name;
public:string path1, path2;

public:PoseEstimation(string path1, string path2) {
	models_data = add_models_images(path1);
	tests_data = add_tests_images(path2);
	models_name = add_models_name(path1);
}
	   vector<Mat> get_models() { return models_data; }
	   vector<Mat> get_tests() { return tests_data; }
	   vector<String> get_models_name() { return models_name; }

	   vector<String> add_models_name(string);
	   vector<Mat> add_models_images(string);
	   vector<Mat> add_tests_images(string);
	   MValues Matching_Method(int, void*, string);
	   void extract_best_ten(string);
	   void test_results(string, string path1, string path2);
};