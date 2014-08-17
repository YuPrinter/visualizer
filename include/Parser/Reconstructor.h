#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include <QtCore/QStringList>

#ifdef __APPLE__
    #include <OpenCL/opencl.h>
#else
    #include <CL/cl.h>
#endif

#include "Parser/Helpers.hpp"

#include "Info/SliceInfo.h"

namespace Parser {
    class Reconstructor : public QQuickItem {
        Q_PROPERTY(QVariant imgFiles READ imgFiles WRITE setImgFiles NOTIFY imgFilesChanged)

        Q_OBJECT
    public:
        explicit Reconstructor();
        ~Reconstructor();

        void reset();

        QVariant imgFiles() const;

    private:
        int _sliceNumber;

        QVariant _imgFiles;

        QVector<cv::Mat>_src;
        QVector<cv::Mat *>_slicesOCL;

        cl_context _context;
        cl_device_id _device_id;
        cl_program _programSlice;

        cl_kernel _gauss1dKernel;
        cl_kernel _calcTablesKernel;
        cl_kernel _dht1dTransposeKernel;
        cl_kernel _fourier2dKernel;
        cl_kernel _butterflyDht2dKernel;

        cl_command_queue _queue;

        void showSliceWithNumber(const int & sliceNumber);

        void initOpenCL();

        void reconstruct();
        void reconstructCPU();

        void visualize();

    signals:
        void slicesProcessed(QVariant slices);
        void imgFilesChanged();

    public slots:
        void setImgFiles(const QVariant & fileNames);
        Q_INVOKABLE void nextSlice(const int & ds);
    };
}

#endif // RECONSTRUCTOR_H
