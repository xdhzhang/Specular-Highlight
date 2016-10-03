#ifndef BASIC_H
#define BASIC_H

#define QCV_DEF_PADDING                         10
#define QCV_DEF_THRESHOLD_ZERO                  1e-6

#include <QtMath>

/* Box filter */
void boxcar_sliding_window(double **out, double **in, double **temp,
                           int h, int w, int radius);
void boxcar_sliding_window_x(double *out, double *in,
                             int h, int w, int radius);
void boxcar_sliding_window_y(double *out, double *in,
                             int h, int w, int radius);
/* Gaussian filter */
void gaussian_recursive(double **image, double **temp, double sigma,
                       int order, int h, int w);
void gaussian_recursive_x(double **od, double **id, int w, int h,
                          double a0, double a1, double a2, double a3,
                          double b1, double b2, double coefp, double coefn);
void gaussian_recursive_y(double **od, double **id, int w, int h,
                          double a0, double a1, double a2, double a3,
                          double b1, double b2, double coefp, double coefn);

//void saveImage(char* fileName, uchar *data, int h, int w, int channel);

/* Basic functions */
inline double *get_color_weighted_table(double sigma_range, int len) {
    double *table_color = new double[len];
    double *color_table_x = &table_color[0];

    for(int i = 0; i < len; ++i)
        (*color_table_x++) = qExp(-double(i*i)/(2*sigma_range*sigma_range));

    return table_color;
}

inline void color_weighted_table_update(double *table_color, double dist_color, int len) {
    double *color_table_x = &table_color[0];

    for(int i = 0; i < len; ++i)
        (*color_table_x++) = qExp(-double(i*i)/(2*dist_color*dist_color));
}

inline void vec_min_val(uchar &min_val, uchar *in, int len) {
    min_val = in[0];
    for(int i = 1; i < len; ++i)
        if(in[i] < min_val) min_val = in[i];
}

inline void vec_max_val(uchar &max_val, uchar *in, int len) {
    max_val = in[0];
    for(int i = 1; i < len; ++i)
        if(in[i] > max_val) max_val = in[i];
}

template<typename T>
inline void down_sample(T **out, T **in, int h, int w, int scale_exp) {
    int ho = (h >> scale_exp);
    int wo = (w >> scale_exp);

    T *out_y, *in_x;
    for(int y = 0; y < ho; ++y) {
        out_y = &out[y][0];
        in_x = in[y << scale_exp];
        for(int x = 0; x < wo; ++x)
            *out_y++ = in_x[x << scale_exp];
    }
}

inline double linear_interpolate_xy(double **image, double x, double y, int h, int w) {
    int x0 = int(x);
    int xt = qMin(x0 + 1, w - 1);
    int y0 = int(y);
    int yt = qMin(y0 + 1, h - 1);
    double dx = x - x0;
    double dy = y - y0;
    double dx1 = 1 - dx;
    double dy1 = 1 - dy;
    double d00 = dx1*dy1;
    double d0t = dx*dy1;
    double dt0 = dx1*dy;
    double dtt = dx*dy;

    return (d00*image[y0][x0] +
            d0t*image[y0][xt] +
            dt0*image[yt][x0] +
            dtt*image[yt][xt]);
}

/* Memory */
template<typename T>
inline T *** alloc_3(int n, int r, int c, int padding = QCV_DEF_PADDING) {
    int rc = r*c;
    T *a = new T[n*rc + padding];
    T **p = new T*[n*r];
    T ***pp = new T**[n];

    for(int i = 0; i < n; ++i)
        for(int j = 0; j < r; ++j) {
            int a_index = i*rc + j*c;
            a[a_index] = 0;
            p[i*r + j] = &a[a_index];
        }

    for(int i = 0; i < n; ++i)
        pp[i] = &p[i*r];

    return pp;
}

template<typename T>
inline T ** alloc(int r, int c, int padding = QCV_DEF_PADDING) {
    T *a = new T[r*c + padding];
    T **p = new T*[r];

    for(int i = 0; i < r; ++i) {
        int a_index = i*c;
        a[a_index] = 0;
        p[i] = &a[a_index];
    }

    return p;
}

template<typename T>
inline void free(T ***p) {
    if(p != nullptr) {
        delete[] p[0][0];
        delete[] p[0];
        delete[] p;
        p = nullptr;
    }
}

template<typename T>
inline void free(T **p) {
    if(p != nullptr) {
        delete[] p[0];
        delete[] p;
        p = nullptr;
    }
}

#endif // BASIC_H
