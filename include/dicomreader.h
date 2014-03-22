#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <QtCore/QObject>

#include <gdcmImage.h>
#include <gdcmFile.h>

#include <opencv2/highgui/highgui.hpp>

#include "ctprocessing.hpp"

#define DICOM_ALL_OK 0
#define DICOM_FILE_NOT_READABLE 1

#define OPENCL_ALL_OK 0
#define OPENCL_NOT_INITIALIZED 1

#define CANNY_LOWER 200

class DicomReader : public QObject {
  Q_OBJECT
public:
  explicit DicomReader(QObject * parent = 0);
  explicit DicomReader(const QString & dicomFile, Images & images, QObject * parent = 0);

  ~DicomReader();

  int readImage(gdcm::File & dFile, const gdcm::Image & dImage, Images & images);

  QImage dQImage();
  cv::Mat dCImage();

  void decImageNumber();
  void incImageNumber();

  void reset(Images & images, const int & newSize = 0);
  void resetV(std::vector<cv::Mat*> & vec, const int & newSize = 0);

private:
  size_t _imageNumber;

  Images * _images;

  cv::ocl::Context * _context;

  int initOpenCL();

  void showImageWithNumber(const size_t & imageNumber);

signals:

public slots:
  int readFile(const QString & dicomFile, Images & images);
};

#endif // DICOMREADER_H
