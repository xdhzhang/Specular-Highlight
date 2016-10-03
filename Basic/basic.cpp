#include "Basic/basic.h"

#include <QDebug>
#include <stdio.h>

/* Box filter */
void boxcar_sliding_window(double **out, double **in, double **temp,
                           int h, int w, int radius) {
    boxcar_sliding_window_x(temp[0], in[0], h, w, radius);
    boxcar_sliding_window_y(out[0], temp[0], h, w, radius);
}

void boxcar_sliding_window_x(double *out, double *in,
                             int h, int w, int radius) {
    double scale = 1.f/(2*radius + 1);
    for(int y = 0; y < h; ++y) {
        double t = in[y*w]*radius;

        // do left edge
        for(int x = 0; x < radius + 1; ++x)
            t += in[y*w + x];

        out[y*w] = t*scale;

        for(int x = 1; x < radius + 1; ++x) {
            int c = y*w + x;
            t += in[c + radius];
            t -= in[y*w];
            out[c] = t*scale;
        }

        // main loop
        for(int x = radius + 1; x < w - radius; ++x) {
            int c = y*w + x;
            t += in[c + radius];
            t -= in[c - radius - 1];
            out[c] = t*scale;
        }

        // do right edge
        for(int x = w - radius; x < w; ++x) {
            int c = y*w + x;
            t += in[y*w + w - 1];
            t -= in[c - radius - 1];
            out[c] = t*scale;
        }
    }
}

void boxcar_sliding_window_y(double *out, double *in,
                             int h, int w, int radius) {
    double scale = 1.f/(2*radius + 1);
    for(int x = 0; x < w; ++x) {
        double t = in[x]*radius;

        // do left edge
        for(int y = 0; y < radius + 1; ++y)
            t += in[y*w + x];

        out[x] = t*scale;

        for(int y = 1; y < radius + 1; ++y) {
            int c = y*w + x;
            t += in[c + radius*w];
            t -= in[x];
            out[c] = t*scale;
        }

        // main loop
        for(int y = radius + 1; y < h - radius; ++y) {
            int c = y*w + x;
            t += in[c + radius*w];
            t -= in[c - radius*w - w];
            out[c] = t*scale;
        }

        // do right edge
        for(int y = h - radius; y < h; ++y) {
            int c = y*w + x;
            t += in[(h - 1)*w + x];
            t -= in[c - radius*w - w];
            out[c] = t*scale;
        }
    }
}

/* Gaussian filter */
void gaussian_recursive(double **image, double **temp, double sigma,
                        int order, int h, int w) {
    const double
            nsigma = sigma < 0.1f ? 0.1f : sigma,
            alpha = 1.695f/nsigma,
            ema = qExp(-alpha),
            ema2 = qExp(-2*alpha),
            b1 = -2*ema,
            b2 = ema2;

    double
            a0 = 0, a1 = 0, a2 = 0,
            a3 = 0, coefp = 0, coefn = 0;

    switch(order) {
    case 0: {
        const double k = (1 - ema)*(1 - ema)/(1 + 2*alpha*ema - ema2);
        a0 = k;
        a1 = k*(alpha - 1)*ema;
        a2 = k*(alpha + 1)*ema;
        a3 = -k*ema2;
    } break;

    case 1: {
        const double k = (1 - ema)*(1 - ema)/ema;
        a0 = k*ema;
        a1 = a3 = 0;
        a2 = -a0;
    } break;

    case 2: {
        const double
                ea = qExp(-alpha),
                k = -(ema2 - 1)/(2*alpha*ema),
                kn = (-2*(-1 + 3*ea - 3*ea*ea + ea*ea*ea))/
                (3*ea + 1 + 3*ea*ea + ea*ea*ea);
        a0 = kn;
        a1 = -kn*(1 + k*alpha)*ema;
        a2 = kn*(1 - k*alpha)*ema;
        a3 = -kn*ema2;
    } break;

    default:
        qDebug() << "gaussianFilter: invalid order parameter!";
        return;
    }

    coefp = (a0 + a1)/(1 + b1 + b2);
    coefn = (a2 + a3)/(1 + b1 + b2);

    gaussian_recursive_x(temp, image, w, h,
                         a0, a1, a2, a3,
                         b1, b2, coefp, coefn);
    gaussian_recursive_y(image, temp, w, h,
                         a0, a1, a2, a3,
                         b1, b2, coefp, coefn);
}

void gaussian_recursive_x(double **od, double **id, int w, int h,
                          double a0, double a1, double a2, double a3,
                          double b1, double b2, double coefp, double coefn) {
    double xp = 0.f;     // previous input
    double yp = 0.f;     // previous output
    double yb = 0.f;     // previous output by 2

    for(int y = 0; y < h; ++y) {
        xp = id[y][0]; yb = coefp*xp; yp = yb;

        for(int x = 0; x < w; ++x) {
            double xc = id[y][x];
            double yc = a0*xc + a1*xp - b1*yp - b2*yb;

            od[y][x] = yc;
            xp = xc; yb = yp; yp = yc;
        }
    }

    // reverse pass
    // ensures response is symmetrical
    double xn = 0.f;
    double xa = 0.f;
    double yn = 0.f;
    double ya = 0.f;

    for(int y = 0; y < h; ++y) {
        xn = xa = id[y][w - 1]; yn = coefn*xn; ya = yn;
        for(int x = w - 1; x >= 0; --x) {
            double xc = id[y][x];
            double yc = a2*xn + a3*xa - b1*yn - b2*ya;

            xa = xn; xn = xc; ya = yn; yn = yc;
            od[y][x] = od[y][x] + yc;
        }
    }
}

void gaussian_recursive_y(double **od, double **id, int w, int h,
                          double a0, double a1, double a2, double a3,
                          double b1, double b2, double coefp, double coefn) {
    double xp = 0.f;     // previous input
    double yp = 0.f;     // previous output
    double yb = 0.f;     // previous output by 2

    for(int x = 0; x < w; ++x) {
        xp = id[0][x]; yb = coefp*xp; yp = yb;

        for(int y = 0; y < h; ++y) {
            double xc = id[y][x];
            double yc = a0*xc + a1*xp - b1*yp - b2*yb;

            od[y][x] = yc;
            xp = xc; yb = yp; yp = yc;
        }
    }

    // reverse pass
    // ensures response is symmetrical
    double xn = 0.f;
    double xa = 0.f;
    double yn = 0.f;
    double ya = 0.f;

    for(int x = 0; x < w; ++x) {
        xn = xa = id[h - 1][x]; yn = coefn*xn; ya = yn;
        for(int y = h - 1; y >= 0; --y) {
            double xc = id[y][x];
            double yc = a2*xn + a3*xa - b1*yn - b2*ya;

            xa = xn; xn = xc; ya = yn; yn = yc;
            od[y][x] = od[y][x] + yc;
        }
    }
}

//void saveImage(char* fileName, uchar *data, int h, int w, int channel) {
//    FILE *file_out;
//    uchar maxx = 255;

//    fopen_s(&file_out, fileName, "wb");

//    if(channel == 1)
//        fprintf(file_out, "P5\n%d %d\n%f\n", w, h, maxx);
//    else if(channel == 3)
//        fprintf(file_out, "P6\n%d %d\n%f\n", w, h, maxx);
//    else
//        fprintf(file_out, "P9\n%d %d\n%d\n%f\n", w, h, channel, maxx);

//    fwrite(data, sizeof(uchar), w*h*channel, file_out);
//    fclose(file_out);
//}
