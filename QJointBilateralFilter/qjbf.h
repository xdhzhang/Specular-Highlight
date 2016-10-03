#ifndef QJBF_H
#define QJBF_H

#define QCV_DEF_CTBF_GAUSSIAN_BILATERAL_FILTER          0
#define QCV_DEF_CTBF_BOX_BILATERAL_FILTER               1
#define QCV_DEF_CTBF_INTENSITY_RANGE					256
#define QCV_DEF_CTBF_SIGMA_SPATIAL_DEFAULT              0.03    //0.03 ~ 16/512
#define QCV_DEF_CTBF_SIGMA_RANGE_DEFAULT				0.08    //0.08 ~ 20/255
#define QCV_DEF_CTBF_H_MIN                              24
#define QCV_DEF_CTBF_W_MIN                              32

#include <QtMath>

class QJBF {
public:
    QJBF();
    ~QJBF();

    void init(int h_original,int w_original,
              int spatial_filter = QCV_DEF_CTBF_GAUSSIAN_BILATERAL_FILTER,
              double sigma_spatial = QCV_DEF_CTBF_SIGMA_SPATIAL_DEFAULT,
              double sigma_range = QCV_DEF_CTBF_SIGMA_RANGE_DEFAULT);

    void joint_bilateral_filter(float **image_filtered, float **image, uchar **texture,
                                uchar **mask, int nr_scale = 8,
                                double sigma_spatial = 0, double sigma_range = 0);

    void clean();

private:
    int m_h;
    int m_w;
    int m_h_original;
    int m_w_original;
    int m_nr_shift;
    int m_nr_scale;
    int m_radius;
    int m_spatial_filter;

    double m_sigma_range;
    double m_sigma_spatial;

    double ***m_jk;
    double **m_wk;
    double **m_box;
    double *m_grayscale;
    double *m_table;

    uchar **m_image_y_downsampled_texture;

    float **m_image_y_downsampled_f;
};

#endif // QJBF_H
