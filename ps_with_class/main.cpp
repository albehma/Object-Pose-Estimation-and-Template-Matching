#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include <fstream>
#include "PoseEstimation.h"

using namespace std;
using namespace cv;

int main() {
	//can
	string can_models_path = "pose_estimation_dataset/can/models/model*";
	string can_tests_path = "pose_estimation_dataset/can/test_images/*jpg";
	PoseEstimation *can = new PoseEstimation(can_models_path, can_tests_path);

	vector<Mat> can_models = can->get_models();
	vector<Mat> can_test = can->get_tests();

	can->extract_best_ten("can_results.txt");
	can->test_results("can_results.txt", can_models_path, can_tests_path);

	//driller
	string driller_models_path = "pose_estimation_dataset/driller/models/model*";
	string driller_tests_path = "pose_estimation_dataset/driller/test_images/*jpg";
	PoseEstimation *driller = new PoseEstimation(driller_models_path, driller_tests_path);

	vector<Mat> driller_models = driller->get_models();
	vector<Mat> driller_test = driller->get_tests();

	driller->extract_best_ten("driller_results.txt");
	driller->test_results("driller_results.txt", driller_models_path, driller_tests_path);

	//duck
	string duck_models_path = "pose_estimation_dataset/duck/models/model*";
	string duck_tests_path = "pose_estimation_dataset/duck/test_images/*jpg";
	PoseEstimation *duck = new PoseEstimation(duck_models_path, duck_tests_path);

	vector<Mat> duck_models = duck->get_models();
	vector<Mat> duck_test = duck->get_tests();

	duck->extract_best_ten("duck_results.txt");
	duck->test_results("duck_results.txt", duck_models_path, duck_tests_path);

	waitKey(0);
	return 0;
}