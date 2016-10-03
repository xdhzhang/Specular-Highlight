#ifndef QHIGHLIGHT_H
#define QHIGHLIGHT_H

#define QCV_DEF_DARK_PIXEL								20
#define QCV_DEF_THRESHOLD_SIGMA_CHANGE					0.03f

#include "QMat/qmat.h"
#include "QJointBilateralFilter/qjbf.h"

using namespace qcv;

class QHighLight {
public:
    QHighLight();
    ~QHighLight();

    void init(QMat image,
              uchar threshold_dark_pixel = QCV_DEF_DARK_PIXEL,
              float threshold_sigma_change = QCV_DEF_THRESHOLD_SIGMA_CHANGE);

    int diffuse(int nr_iter = 0);

    void clean();

    QMat getDiffuseImage() const { return m_diffuse_image; }

private:
    QJBF m_bf;

    QMat m_image;
    QMat m_diffuse_image;

    int m_h;
    int m_w;
    uchar m_threshold_dark_pixel;
    float m_threshold_sigma_change;
    int m_nr_iteration;

    uchar **m_image_sf;
    uchar **m_mask_dark_pixel;
    float ***m_max_chrom;

    void compute_approximated_maximum_diffuse_chromaticity(float **image_max_chrom);

    void compute_diffuse_reflection_from_maximum_diffuse_chromaticity(float **max_diffuse_chromaticity);
};

#endif // QHIGHLIGHT_H
