#include "QJointBilateralFilter/qjbf.h"
#include "Basic/basic.h"

#include <QDebug>

QJBF::QJBF() {
    m_box = nullptr;
    m_jk = nullptr;
    m_wk = nullptr;
    m_grayscale = nullptr;
    m_image_y_downsampled_f = nullptr;
    m_image_y_downsampled_texture = nullptr;
}

QJBF::~QJBF() {
    clean();
}

void QJBF::clean() {
    free(m_box);
    free(m_wk);
    free(m_jk);

    if(m_grayscale != nullptr) {
        delete[] m_grayscale;
        m_grayscale = nullptr;
    }

    free(m_image_y_downsampled_f);
    free(m_image_y_downsampled_texture);
}

void QJBF::init(int h_original,int w_original, int spatial_filter,
                double sigma_spatial, double sigma_range) {
    m_h_original = h_original;
    m_w_original = w_original;
    m_spatial_filter = spatial_filter;
    m_sigma_spatial = sigma_spatial;
    m_sigma_range = sigma_range;
    m_nr_scale = QCV_DEF_CTBF_INTENSITY_RANGE;

    int shift_h = int(qLn((double)m_h_original/QCV_DEF_CTBF_H_MIN)/qLn(2.) + 0.5);
    int shift_w = int(qLn((double)m_w_original/QCV_DEF_CTBF_W_MIN)/qLn(2.) + 0.5);

    m_nr_shift = qMax(1, qMin(shift_h, shift_w));

//    qDebug() << "m_nr_shift: " << m_nr_shift;

    m_h = (h_original >> m_nr_shift);
    m_w = (w_original >> m_nr_shift);

    if(m_spatial_filter == QCV_DEF_CTBF_BOX_BILATERAL_FILTER)
        m_radius = int(m_sigma_spatial*qMin(m_h, m_w) + 0.5f);
    else if(m_spatial_filter != QCV_DEF_CTBF_GAUSSIAN_BILATERAL_FILTER) {
        qDebug() << "Note: ONLY support box and Gaussian spatial filter!";
        qDebug() << "Switching to Gaussian spatial filter automatically";
        m_spatial_filter = QCV_DEF_CTBF_GAUSSIAN_BILATERAL_FILTER;
    }

    /* Memory allocation */
    m_box = alloc<double>(m_h, m_w);
    m_wk = alloc<double>(m_h, m_w);
    m_jk = alloc_3<double>(2, m_h, m_w);

    m_table = get_color_weighted_table(m_sigma_range*QCV_DEF_CTBF_INTENSITY_RANGE,
                                       QCV_DEF_CTBF_INTENSITY_RANGE);
    m_grayscale = new double[m_nr_scale];

    m_image_y_downsampled_f = alloc<float>(m_h, m_w);
    m_image_y_downsampled_texture = alloc<uchar>(m_h, m_w);
}

void QJBF::joint_bilateral_filter(float **image_filtered, float **image, uchar **texture, uchar **mask,
                                  int nr_scale, double sigma_spatial, double sigma_range) {
    if(sigma_range > QCV_DEF_THRESHOLD_ZERO) {
        m_sigma_range = sigma_range;
        color_weighted_table_update(m_table,
                                    m_sigma_range*QCV_DEF_CTBF_INTENSITY_RANGE,
                                    QCV_DEF_CTBF_INTENSITY_RANGE);
    }

    if(sigma_spatial > QCV_DEF_THRESHOLD_ZERO)
        m_sigma_spatial = sigma_spatial;

    down_sample(m_image_y_downsampled_f, image, m_h_original, m_w_original, m_nr_shift);
    down_sample(m_image_y_downsampled_texture, texture, m_h_original, m_w_original, m_nr_shift);

    uchar image_min, image_max;
    vec_min_val(image_min, texture[0], m_h_original*m_w_original);
    vec_max_val(image_max, texture[0], m_h_original*m_w_original);

    m_nr_scale = nr_scale;

    m_grayscale[0] = (double)image_min;
    m_grayscale[m_nr_scale - 1] = qMin(255, (int)image_max + 1);

    double delta_scale = double(m_nr_scale - 1)/(image_max - image_min);
    double shift_inv = 1./(1 << m_nr_shift);

    for(int i = 1; i < m_nr_scale - 1; ++i)
        m_grayscale[i] = (double)image_min + i*double(image_max - image_min)/(m_nr_scale - 1);

    int jk_0 = 0;
    int jk_1 = 1;
    for(int i = 0; i < m_nr_scale; ++i) {
        qreal **jk;

        jk = (i == 0 ? m_jk[jk_0] : m_jk[jk_1]);

        for(int y = 0; y < m_h; ++y)
            for(int x = 0; x < m_w; ++x) {
                int index = int(qAbs(m_grayscale[i] - m_image_y_downsampled_texture[y][x]) + 0.5f);
                jk[y][x] = m_table[index]*m_image_y_downsampled_f[y][x];
                m_wk[y][x] = m_table[index];
            }

        if(m_spatial_filter == QCV_DEF_CTBF_BOX_BILATERAL_FILTER) {
            boxcar_sliding_window(jk, jk, m_box, m_h, m_w, m_radius);
            boxcar_sliding_window(m_wk, m_wk, m_box, m_h, m_w, m_radius);
        } else if(m_spatial_filter == QCV_DEF_CTBF_GAUSSIAN_BILATERAL_FILTER) {
            gaussian_recursive(jk, m_box, m_sigma_spatial*qMin(m_h, m_w), 0, m_h, m_w);
            gaussian_recursive(m_wk, m_box, m_sigma_spatial*qMin(m_h, m_w), 0, m_h, m_w);
        }

        for(int y = 0; y < m_h; ++y)
            for(int x = 0; x < m_w; ++x)
                jk[y][x] /= m_wk[y][x];

        uchar *image_y = texture[0];
        if(i > 0) {
            for(int y = 0; y < m_h_original; ++y)
                for(int x = 0; x < m_w_original; ++x)
                    if(mask[y][x]) {
                        double kf = double((*image_y++) - image_min)*delta_scale;
                        int k = int(kf);
                        double ys = qMin(y*shift_inv, (double)m_h - 1);
                        double xs = qMin(x*shift_inv, (double)m_w - 1);

                        if(k == (i - 1)) {
                            double alpha = (k + 1) - kf;
                            image_filtered[y][x] = float(alpha*linear_interpolate_xy(m_jk[jk_0], xs, ys, m_h, m_w) +
                                                         (1.f - alpha)*linear_interpolate_xy(m_jk[jk_1], xs, ys, m_h, m_w));
                        } else if(k == i && i == (m_nr_scale - 1)) {
                            image_filtered[y][x] = (float)linear_interpolate_xy(m_jk[jk_1], xs, ys, m_h, m_w);
                        }
                    } else *image_y++;
            jk_1 = jk_0;
            jk_0 = (jk_0 + 1)%2;
        }
    }
}
