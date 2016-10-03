#ifndef QMAT_H
#define QMAT_H

#ifndef __cplusplus
#  error qmat.hpp header must be compiled as C++
#endif

#include <opencv2/core/mat.hpp>
#include <QLabel>

using namespace cv;

namespace qcv {

class QMat : public Mat {
public:
    QMat()
        : Mat() {}

    QMat(int rows, int cols, int type)
        : Mat(rows, cols, type) {}

    QMat(Size size, int type)
        : Mat(size, type) {}

    QMat(int rows, int cols, int type, const Scalar& s)
        : Mat(rows, cols, type, s) {}

    QMat(Size size, int type, const Scalar& s)
        : Mat(size, type, s) {}

    QMat(int ndims, const int* sizes, int type)
        : Mat(ndims, sizes, type) {}

    QMat(int ndims, const int* sizes, int type, const Scalar& s)
        : Mat(ndims, sizes, type, s) {}

    QMat(const Mat& m)
        : Mat(m) {}

    QMat(int rows, int cols, int type, void* data, size_t step=AUTO_STEP)
        : Mat(rows, cols, type, data, step) {}

    QMat(Size size, int type, void* data, size_t step=AUTO_STEP)
        : Mat(size, type, data, step) {}

    QMat(int ndims, const int* sizes, int type, void* data, const size_t* steps=0)
        : Mat(ndims, sizes, type, data, steps) {}

    QMat(const Mat& m, const Range& rowRange, const Range& colRange=Range::all())
        : Mat(m, rowRange, colRange) {}

    QMat(const Mat& m, const Rect& roi)
        : Mat(m, roi) {}

    QMat(const Mat& m, const Range* ranges)
        : Mat(m, ranges) {}

    template<typename _Tp> explicit QMat(const std::vector<_Tp>& vec, bool copyData=false)
        : Mat(vec, copyData) {}

    template<typename _Tp, int n> explicit QMat(const Vec<_Tp, n>& vec, bool copyData=true)
        : Mat(vec, copyData) {}

    template<typename _Tp, int m, int n> explicit QMat(const Matx<_Tp, m, n>& mtx, bool copyData=true)
        : Mat(mtx, copyData) {}

    template<typename _Tp> explicit QMat(const Point_<_Tp>& pt, bool copyData=true)
        : Mat(pt, copyData) {}

    template<typename _Tp> explicit QMat(const Point3_<_Tp>& pt, bool copyData=true)
        : Mat(pt, copyData) {}

    template<typename _Tp> explicit QMat(const MatCommaInitializer_<_Tp>& commaInitializer)
        : Mat(commaInitializer) {}

    //! download data from GpuMat
    explicit QMat(const cuda::GpuMat& m)
        : Mat(m) {}

    virtual ~QMat() {}

    QMat& operator = (const MatExpr& expr);
    QMat& operator = (const Scalar& s);

    void draw(QLabel *lable);

    QMat cvtColor(int code, int dstCn = 0);
    void cvtColor_(int code, int dstCn = 0);

};

} // namespace qcv

#endif // QMAT_H
