#include "PoseEstimation.h"

Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";
int match_method = 4;

//Variabili di edge detection
Mat tmp, src_gray;
Mat test, test_gray;
int lowThreshold = 11;
const int kernel_size = 3;
//const char* window_name = "Edge Map";

//Variabili di template matching
Mat sfondo_matching;

MValues matching_values;

/*
--------------------AUSILIAR FUNCTION--------------------
*/

bool sort_by_s(const MValues &m1, const MValues &m2) {
	return m1.s > m2.s;
}

/*
--------------------FUNCTIONS DEFINED IN THE HEADER FILE--------------------
*/

vector<Mat> PoseEstimation::add_models_images(string path1)
{
	vector<String> string_can_models;
	vector<Mat> models_data;
	glob(path1, string_can_models, true);
	for (size_t k = 0; k < string_can_models.size(); ++k)
	{
		Mat im = imread(string_can_models[k], IMREAD_COLOR);
		if (im.empty()) continue; //only proceed if sucsessful
		models_data.push_back(im);
	}
	return models_data;
}

vector<Mat> PoseEstimation::add_tests_images(string path2)
{
	vector<String> string_can_tests;
	vector<Mat> tests_data;
	glob(path2, string_can_tests, true);
	for (size_t k = 0; k < string_can_tests.size(); ++k)
	{
		Mat im = imread(string_can_tests[k], IMREAD_COLOR);
		if (im.empty()) continue; //only proceed if sucsessful
		tests_data.push_back(im);
	}
	return tests_data;
}

vector<String> PoseEstimation::add_models_name(string path1)
{
	vector<String> models_string;
	vector<Mat> tests_data;
	glob(path1, models_string, true);
	return models_string;
}

void PoseEstimation::extract_best_ten(string filename)
{
	vector<MValues> all_the_values;

	//Loading images in a data vector;
	vector<String> string_models = PoseEstimation::get_models_name();
	vector<Mat> models_data = PoseEstimation::get_models();
	vector<Mat> tests_data = PoseEstimation::get_tests();

	ofstream fout(filename, ios::app);

	for (int j = 0; j < tests_data.size(); j++) {
		for (int i = 0; i < models_data.size(); i++) {
			tmp = models_data[i]; // Load an image
			test = tests_data[j];
			if (tmp.empty() || test.empty())
			{
				std::cout << "Could not open or find the image!\n" << std::endl;
				return;
			}

			cvtColor(tmp, src_gray, COLOR_BGR2GRAY);
			blur(src_gray, src_gray, Size(3, 3));
//			namedWindow(window_name, WINDOW_AUTOSIZE);
            Canny(src_gray, src_gray, 10, 50);

			cvtColor(test, test_gray, COLOR_BGR2GRAY);
			blur(test_gray, test_gray, Size(3, 3));
			Canny(test_gray, test_gray, 60, 80);
//			imshow("edge template", src_gray);

			cvtColor(src_gray, src_gray, COLOR_GRAY2BGR);
			cvtColor(test_gray, test_gray, COLOR_GRAY2BGR);
			sfondo_matching = test_gray;

			if (sfondo_matching.empty() || src_gray.empty())
			{
				cout << "Can't read one of the images" << endl;
				return;
			}

			// Create windows
//			namedWindow(image_window, WINDOW_AUTOSIZE);
//			namedWindow(result_window, WINDOW_AUTOSIZE);

			matching_values = Matching_Method(0, 0, string_models[i]);
			all_the_values.push_back(matching_values);

//			waitKey(0);
//			destroyAllWindows();
		}
		sort(all_the_values.begin(), all_the_values.end(), sort_by_s);
		fout << "test" + to_string(j) + ".jpg" << " ";
		for (int t = 0; t < 10; t++) {
			fout << all_the_values[t].name << " " << all_the_values[t].y.x << " " << all_the_values[t].y.y <<  " ";
		}
		fout << endl;
		all_the_values.clear();
	}
	fout.close();
	return;
}

MValues PoseEstimation::Matching_Method(int, void*, string s)
{
	MValues mv;
	//! [copy_source]
	/// Source image to display
	Mat img_display;
	test.copyTo(img_display);
	//! [copy_source]

	//! [create_result_matrix]
	/// Create the result matrix
	int result_cols = sfondo_matching.cols - src_gray.cols + 1;
	int result_rows = sfondo_matching.rows - src_gray.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);
	//! [create_result_matrix]

	//! [match_template]
	/// Do the Matching
	matchTemplate(sfondo_matching, src_gray, result, match_method);
	//! [match_template]

	//! [best_match]
	/// Localizing the best match with minMaxLoc
	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	//! [best_match]

	//! [match_loc]
	matchLoc = maxLoc;
	//! [match_loc]

	//show all the values
	cout << matchLoc << endl;
	std::cout << "Max correlation=" << maxVal << std::endl;

	//extract the model's name and assign all the corrsponding values
	string toFind = "\model";
	size_t a = s.find(toFind);
	mv.name = s.substr(a + 7);
	mv.s = maxVal;
	mv.y = matchLoc;

	/// Show me what you got
/*	rectangle(img_display, matchLoc, Point(matchLoc.x + dst.cols, matchLoc.y + dst.rows), Scalar(0, 255, 0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + dst.cols, matchLoc.y + dst.rows), Scalar(0, 255, 0), 2, 8, 0);

	imshow(image_window, img_display);
	imshow(result_window, result);*/

	return mv;
}

void PoseEstimation::test_results(string filename, string path1, string path2)
{
	//Variabili per estrarre gli interi e lo shift dal file .txt
	int find, pos;
	vector<int> shift_values;
	int shift_x, shift_y;
	int i = 0;
	//Variabili per estrarre l'immagine di test e i 10 migliori template dal file .txt
	Mat test, model;

	ifstream fin(filename);
	string line, word;
	while (getline(fin, line))
	{
		stringstream ss(line);
		while (ss >> word) {

			if (word.rfind("test", 0) == 0) {
				test = imread((path2.substr(0, path2.length() - 4) + word));
			}

			else if (word.rfind("model", 0) == 0)
			{
				model = imread((path1.substr(0, path1.length() - 6) + word));
				imshow("model", model);
			}

			else if (stringstream(word) >> find) {
				//shift contiene tutti gli spostamenti
				shift_values.push_back(find);
				i++;
				if (i % 2 == 0 && i > 1) {
					Mat img_display;
					test.copyTo(img_display);
					shift_x = model.cols + shift_values[i - 2];
					shift_y = model.rows + shift_values[i - 1];
					rectangle(img_display, Point(shift_values[i - 2], shift_values[i - 1]), Point(shift_x, shift_y), Scalar(0, 255, 0), 2, 8, 0);
					imshow("match", img_display);
					waitKey(0);
					destroyAllWindows();
				}
			}
		}
	}
}