#include <raspicam/raspicam_cv.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/face.hpp>
#include <iostream>
 
using namespace std;
 
void detectFace(cv::Mat &frame,
        cv::CascadeClassifier &face_cascade,
        vector<cv::Rect> &faces,
        cv::Ptr<cv::face::FaceRecognizer> &model,
        int &pos_x,
        int &pos_y,
        string &text) {
  cv::Mat grayscale;
 
  // Convert frame to grayscale, normalize the brightness, and increase the contrast
  cv::cvtColor(frame, grayscale, cv::COLOR_BGR2GRAY);
  cv::equalizeHist(grayscale, grayscale);
 
  // Detect faces
  face_cascade.detectMultiScale(grayscale, faces, 2, 6, 0|CV_HAAR_SCALE_IMAGE, cv::Size(50, 50));
 
  for (size_t i = 0; i < faces.size(); i++) { cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 ); cv::ellipse( frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 255, 255 ), 2, 8, 0 ); // Recognize face // Get and resize detected face cv::Mat face_i = cv::Mat(frame, faces[i]); // Grab face from frame inside Rect cv::cvtColor(face_i, face_i, cv::COLOR_BGR2GRAY); cv::resize(face_i, face_i, cv::Size(100, 100), 0, 0, CV_INTER_NN); double predicted_confidence = 0.0; int prediction = -1; model->predict(face_i,prediction,predicted_confidence);
    cout << "Prediction " << prediction << " Confidence " << predicted_confidence << endl;
    if (prediction < 112.0) {
      text = "My gosh, you look familiar...";
      pos_x = max(faces[i].tl().x - 10, 0);
      pos_y = max(faces[i].tl().y - 10, 0);
      cv::putText( frame, text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1);
    } else {
      text = "";
    }
  }
}
 
 
int main(int argc, char **argv) {
  raspicam::RaspiCam_Cv Camera; // Camera Object
  cv::Mat frame; // Image
  cv::CascadeClassifier face_cascade; // Face Cascade Classifier
  cv::Ptr<cv::face::FaceRecognizer> model = cv::face::createLBPHFaceRecognizer();
  vector<cv::Rect> faces; // Vector of Rectangles of every face found in a frame
  string text = ""; // Text to be displayed on screen
  int pos_x = 0; // X position of recongized face
  int pos_y = 0; // Y position of recognized face
  size_t i = 0; // Used to keep track of iterations in the loop
 
  // Check argument count
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " <Detection File> <Recognition File>" << endl;
    return -1;
  }
 
  string classifier_file = argv[1];
  string face_model = argv[2];
 
  // Load face cascade
  cout << "Loading face cascade.." << endl;
  if (!face_cascade.load(classifier_file)) {
    cerr << "Error loading face cascade!" << endl;
    return -1;
  }
 
  // Load FaceRecognizer model
  cout << "Loading face recognition model.." << endl; model->load(face_model);
   
  // Set camera params
  Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3); // For color
 
  // Open camera
  cout << "Opening camera..." << endl;
  if (!Camera.open()) {
    cerr << "Error opening camera!" << endl;
    return -1;
  }
   
  // Start capturing
  cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
 
  for (;;i++) {
    Camera.grab();
    Camera.retrieve(frame);
 
    // Don't call detectFace on every iteration, it's too expensive
    if (i % 6 == 0) {
      detectFace(frame,
         face_cascade,
         faces,
         model,
         pos_x,
         pos_y,
         text);
    } else {
      for (size_t i = 0; i < faces.size(); i++) { cv::Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 ); cv::ellipse( frame, center, cv::Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, cv::Scalar( 255, 255, 255 ), 2, 8, 0 ); cv::putText( frame, text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,255,255), 1); } } cv::imshow("Display Window", frame); if (cv::waitKey(1) > 0) {
      break;
    }
  }
   
  cout << "Stopping camera.." << endl;
  Camera.release();
  return 0;
}
