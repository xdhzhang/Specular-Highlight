#include "QHighLight/qhighlight.h"
#include "Basic/basic.h"

#include <QDebug>

QHighLight::QHighLight() {
    m_image_sf = nullptr;
    m_mask_dark_pixel = nullptr;
    m_max_chrom = nullptr;
}

QHighLight::~QHighLight() {
    clean();
}

void QHighLight::clean() {
    free(m_image_sf);
    free(m_mask_dark_pixel);
    free(m_max_chrom);
}

void QHighLight::init(QMat image,
                      uchar threshold_dark_pixel,
                      float threshold_sigma_change) {
    m_image = image;
    m_threshold_dark_pixel = threshold_dark_pixel;
    m_threshold_sigma_change = threshold_sigma_change;
    m_h = m_image.rows;
    m_w = m_image.cols;

    m_diffuse_image = QMat(m_h, m_w, m_image.type());
    m_image_sf = alloc<uchar>(m_h, m_w);
    m_mask_dark_pixel = alloc<uchar>(m_h, m_w);
    m_max_chrom = alloc_3<float>(2, m_h, m_w);

    m_bf.init(m_h, m_w);
}

int QHighLight::diffuse(int nr_iter) {
    float **max_chrom = m_max_chrom[0];
    float **max_chrom_bf = m_max_chrom[1];

    m_nr_iteration = nr_iter;

    if(m_nr_iteration > 0)
        for(int i = 0; i < nr_iter; ++i) {
            if(i == 0)
                compute_approximated_maximum_diffuse_chromaticity(max_chrom);

            m_bf.joint_bilateral_filter(max_chrom_bf, max_chrom, m_image_sf,
                                        m_mask_dark_pixel, 10, 0.25, 10.f/255);

            for(int y = 0; y < m_h; ++y)
                for(int x = 0; x < m_w; ++x)
                    max_chrom[y][x] = qMax(max_chrom[y][x], max_chrom_bf[y][x]);
        }
    else {
        int i = 0;
        while(i >= 0) {
            if(i == 0)
                compute_approximated_maximum_diffuse_chromaticity(max_chrom);

            m_bf.joint_bilateral_filter(max_chrom_bf, max_chrom, m_image_sf,
                                        m_mask_dark_pixel, 10, 0.25, 10.f/255);

            int counter = 0;
            for(int y = 0; y < m_h; ++y)
                for(int x = 0; x < m_w; ++x) {
                    if(max_chrom_bf[y][x] - max_chrom[y][x] > m_threshold_sigma_change)
                        ++counter;

                    max_chrom[y][x] = qMax(max_chrom[y][x], max_chrom_bf[y][x]);
                }

            ++i;

            if(counter == 0) {
                m_nr_iteration = i;
                i = -1;
            }
        }
    }

    compute_diffuse_reflection_from_maximum_diffuse_chromaticity(max_chrom);

    return m_nr_iteration;
}

void QHighLight::compute_approximated_maximum_diffuse_chromaticity(float **image_max_chrom) {
    uchar *image_approximated_x = m_image_sf[0];
    uchar *mask_x = m_mask_dark_pixel[0];

    for(int y = 0; y < m_h; ++y)
        for(int x = 0; x < m_w; ++x) {
            uchar r = m_image.at<Vec3b>(y, x)[0];
            uchar g = m_image.at<Vec3b>(y, x)[1];
            uchar b = m_image.at<Vec3b>(y, x)[2];

            int i_max = qMax(qMax(r, g), b);

            if(i_max > m_threshold_dark_pixel) {
                *mask_x++ = 255;

                int i_sum = r + g + b;
                float c = (float)i_max/i_sum;

                image_max_chrom[y][x] = (i_sum > 10 ? c : 0);

                float chrom[3] = { (float)r/i_sum, (float)g/i_sum, (float)b/i_sum };
                float c_min = qMin(qMin(chrom[0], chrom[1]), chrom[2]);
                float c_max = qMax(qMax(chrom[0], chrom[1]), chrom[2]);
                float cappro = (c_max - c_min)/(1 - 3*c_min);

                *image_approximated_x++ = uchar(255*(cappro*1.5 - 0.5) + 0.5);
            } else {
                *mask_x++ = 0;
                *image_approximated_x++ = 0;
            }
        }
}

void QHighLight::compute_diffuse_reflection_from_maximum_diffuse_chromaticity(float **max_diffuse_chromaticity) {
    float t3 = 1.f/3.f;

    for(int y = 0; y < m_h; ++y)
        for(int x = 0; x < m_w; ++x)
            if(m_mask_dark_pixel[y][x]) {
                float t1 = 3.f*max_diffuse_chromaticity[y][x] - 1.f;
                uchar r = m_image.at<Vec3b>(y, x)[0];
                uchar g = m_image.at<Vec3b>(y, x)[1];
                uchar b = m_image.at<Vec3b>(y, x)[2];

                if(t1 > 0) {
                    double i_sum = r + g + b;
                    double i_max = qMax(qMax(r, g), b);
                    float c = float(i_max/i_sum);

                    float t0 = t1*c;

                    if(qAbs(t0) < QCV_DEF_THRESHOLD_ZERO)
                        m_diffuse_image.at<Vec3b>(y, x) = Vec3b(r, g, b);
                    else {
                        float t2 = 3.f*c - 1.f;
                        float diffuse = float(i_max*t2/t0);
                        float specular = float(t3*(i_sum - diffuse));
                        float r_f = r - specular;
                        float g_f = g - specular;
                        float b_f = b - specular;

                        if(r_f < 0.f) r_f = 0.f; else if(r_f > 255.f) r_f = 255.f;
                        if(g_f < 0.f) g_f = 0.f; else if(g_f > 255.f) g_f = 255.f;
                        if(b_f < 0.f) b_f = 0.f; else if(b_f > 255.f) b_f = 255.f;

                        m_diffuse_image.at<Vec3b>(y, x) = Vec3b(uchar(r_f + 0.5f),
                                                                uchar(g_f + 0.5f),
                                                                uchar(b_f + 0.5f));
                    }
                }
            } else
                m_diffuse_image.at<Vec3b>(y, x) = m_image.at<Vec3b>(y, x);
}
