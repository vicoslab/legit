/*******************************************************************************
* Copyright (c) 2013, Luka Cehovin
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the University of Ljubljana nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL LUKA CEHOVIN BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
/* -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */

#include "common/canvas.h"
#include "common/utils/utils.h"
#include "common/utils/defs.h"
#include <opencv2/imgproc/imgproc.hpp>

#ifndef FOURCC_MACRO
#define FOURCC_MACRO(c1, c2, c3, c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))
#endif

namespace legit
{

namespace common
{

void drawcross(Mat& image, Point center, Scalar color, int size, bool sideways)
{
  if (sideways)
    {
      line(image, center + Point(-size, 0), center + Point(size, 0), color, 1);
      line(image, center + Point(0, -size), center + Point(0, size), color, 1);
    }
  else
    {
      line( image, Point( center.x - size, center.y - size ), Point( center.x + size, center.y + size), color, 1, LINE_AA, 0);
      line( image, Point( center.x + size, center.y - size ), Point( center.x - size, center.y + size ), color, 1, LINE_AA, 0 );
    }
}

void drawellipse(Mat& image, Point center, Matrix2f covariance, Scalar color, int width)
{

  drawcross(image, center, color, 3 , true);

  // compute square root
  Mat C = Mat(2, 2, CV_32FC1) ;
  C.at<float>(0,0) = covariance.m00 ;
  C.at<float>(0,1) = covariance.m01 ;
  C.at<float>(1,0) = covariance.m10 ;
  C.at<float>(1,1) = covariance.m11 ;

  SVD M = SVD(C) ;

  Mat w ;
  sqrt(M.w,M.w) ;
  w=w.diag(M.w);
  Mat K = Mat(2, 2, CV_32FC1) ;
  K = M.u*w ;
  covariance.m00 = K.at<float>(0,0) ;
  covariance.m01 = K.at<float>(0,1) ;
  covariance.m10 = K.at<float>(1,0) ;
  covariance.m11 = K.at<float>(1,1) ;

  Matrix3f t = covariance | center;

  Point previous = transform_point(Point2f(1, 0), t);

  for (float a = (float)PI / 20; a < 2 * (float)PI; a += (float)PI / 20)
    {
      Point next = Point2f(cos(a), sin(a)) * t;
      line(image, previous, next, color, width);
      previous = next;
    }

  line(image, previous, transform_point(Point2f(1, 0), t), color, width);

}

void canvas_draw_transformed_rectangle(Canvas& canvas, cv::Rect r, Matrix3f t, Scalar color, int width)
{

  Point2f p1 = transform_point(Point2f(r.x, r.y), t);
  Point2f p2 = transform_point(Point2f(r.x + r.width, r.y), t);
  Point2f p3 = transform_point(Point2f(r.x + r.width, r.y + r.height), t);
  Point2f p4 = transform_point(Point2f(r.x, r.y + r.height), t);

  canvas.line(p1, p2, color, width);
  canvas.line(p2, p3, color, width);
  canvas.line(p3, p4, color, width);
  canvas.line(p4, p1, color, width);

}


void ProxyCanvas::resize(cv::Size size)
{
  canvas-> resize(size);
}

void ProxyCanvas::clear()
{
  canvas-> clear();
}

void ProxyCanvas::draw(Mat& img, cv::Point o, int flags)
{
  canvas->draw(img, o + offset, flags);
}

void ProxyCanvas::rectangle(cv::Point tl, cv::Point br, Scalar color, int width)
{
  canvas->rectangle(tl + offset, br + offset, color, width);
}

void ProxyCanvas::rectangle(cv::Rect r, Scalar color, int width)
{
  canvas->rectangle(r + offset, color, width);
}

void ProxyCanvas::rectangle(RotatedRect r, Scalar color, int width)
{
  RotatedRect r1(r);
  r1.center = r1.center + offset;

  canvas->rectangle(r1, color, width);
}

void ProxyCanvas::line(cv::Point p1, cv::Point p2, Scalar color, int width)
{
  canvas->line(p1 + offset, p2 + offset, color, width);
}

void ProxyCanvas::cross(cv::Point p, Scalar color, int size)
{
  canvas->cross(p + offset, color, size);
}

void ProxyCanvas::ellipse(cv::Point center, Matrix2f covariance, Scalar color, int width)
{
  canvas->ellipse(center + offset, covariance, color, width);
}

void ProxyCanvas::circle(cv::Point center, int radius, Scalar color, int width)
{
  canvas->circle(center + offset, radius, color, width);
}

void ProxyCanvas::text(cv::Point p, string str, Scalar color)
{
  canvas->text(p + offset, str, color);
}

void ProxyCanvas::set_zoom(float z)
{
  canvas->set_zoom(z);
};

float ProxyCanvas::get_zoom()
{
  return canvas->get_zoom();
}

int ProxyCanvas::width()
{
  return canvas->width();
};

int ProxyCanvas::height()
{
  return canvas->height();
};

void ProxyCanvas::push(int times)
{
  canvas-> push(times);
};


ImageCanvas::ImageCanvas(Size size) : zoom(1)
{

  image.create(size, CV_8UC3);
  image.setTo(0);

}

ImageCanvas::ImageCanvas(Mat& img) : zoom(1)
{

  image = img;

}

ImageCanvas::~ImageCanvas()
{


}

int ImageCanvas::width()
{
  return image.cols;
}

int ImageCanvas::height()
{
  return image.rows;
}

void ImageCanvas::resize(Size size)
{
  image.create(size, CV_8UC3);

}

void ImageCanvas::clear()
{
  image.setTo(Scalar());

}

void ImageCanvas::draw(Mat& src, Point offset, int flags)
{

  Mat img;
  if (zoom != 1)
    {
      offset *= zoom;

      img = Mat();

      cv::resize(src, img, Size(src.cols * zoom, src.rows * zoom));
    }
  else
    {
      src.copyTo(img);
    }

  Rect srcroi = get_roi(img) + offset;
  Rect dstroi = get_roi(image);
  dstroi = intersection(srcroi, dstroi);
  srcroi = dstroi - offset;

  if (dstroi.width < 1 || dstroi.height < 1)
    return;

  Mat imagesrc;

  if (img.depth() == CV_32F || img.depth() == CV_64F)
    {

      double alpha = 255, beta = 0;
      if (flags & IMAGE_SCALE)
        {
          double min, max;
          minMaxLoc(img, &min, &max, NULL, NULL, Mat());

          if (flags & IMAGE_STRETCH && max != min)
            {
              alpha *= 1 / (max - min);
              beta = -min * alpha;
            }
          else
            {
              alpha *= 1 / max;
            }
        }

      img(srcroi).convertTo(imagesrc, CV_8U, alpha, beta);

    }
  else if (img.depth() == CV_8U)
    {
      imagesrc = img(srcroi);
    }
  else return;

  Mat imagedst = image(dstroi);

  if (img.channels() == 1)
    {

      Mat channels[] = {imagesrc, imagesrc, imagesrc};

      merge(channels, 3, imagedst);

    }
  else if (img.channels() == 3)
    {

      imagesrc.copyTo(imagedst);

    }

}

void ImageCanvas::rectangle(Rect r, Scalar color, int width)
{

  cv::rectangle(image, r.tl() * zoom, r.br() * zoom, color, width);

}

void ImageCanvas::rectangle(Point tl, Point br, Scalar color, int width)
{

  cv::rectangle(image, tl * zoom, br * zoom, color, width);

}

void ImageCanvas::rectangle(RotatedRect r, Scalar color, int width)
{

  float w2 = r.size.width;
  float h2 = r.size.height;

  Matrix3f t = simple_affine_transformation(r.center.x, r.center.y, r.angle, 1, 1);

  Point2f p1(- w2, - h2);
  Point2f p2(- w2, h2);
  Point2f p3(w2, h2);
  Point2f p4(w2, - h2);

  p1 = transform_point(p1, t);
  p2 = transform_point(p2, t);
  p3 = transform_point(p3, t);
  p4 = transform_point(p4, t);

  cv::line(image, p1 * zoom, p2 * zoom, color, width);
  cv::line(image, p2 * zoom, p3 * zoom, color, width);
  cv::line(image, p3 * zoom, p4 * zoom, color, width);
  cv::line(image, p4 * zoom, p1 * zoom, color, width);

}

void ImageCanvas::line(Point p1, Point p2, Scalar color, int width)
{

  cv::line(image, p1 * zoom, p2 * zoom, color, width);

}

void ImageCanvas::cross(Point p, Scalar color, int size)
{
  drawcross(image, p * zoom, color, size);

}

void ImageCanvas::ellipse(Point center, Matrix2f covariance, Scalar color, int width)
{

  drawellipse(image, center * zoom, covariance * zoom, color, width);

}

void ImageCanvas::circle(Point center, int radius, Scalar color, int width)
{

  if (radius < 1) return;

  cv::ellipse(image, center * zoom, cv::Size(radius * zoom, radius * zoom), 0, 0, 360, color, width);

}

void ImageCanvas::text(Point p, string str, Scalar color)
{

  const char* text = str.c_str();

  if (strchr(text, '\n') != NULL)
    {

      char* tmp = strcpy(new char[strlen(text)+1], text);
      char *pch;
      pch = strtok(tmp,"\n");
      while (pch != NULL)
        {
          int baseline;
          Size sz = getTextSize(pch, FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseline);

          putText(image, pch, p * zoom, FONT_HERSHEY_SIMPLEX, 0.4 * zoom, color);
          p.y+=sz.height + 10;
          pch = strtok (NULL, "\n");
        }

      delete[] tmp;
    }
  else
    {
      putText(image, text, p * zoom, FONT_HERSHEY_SIMPLEX, 0.4 * zoom, color);
    }

}

VideoCanvas::VideoCanvas(string filename, int fps, Size size, bool isColor) : ImageCanvas(size)
{

  writer = Ptr<VideoWriter>(new VideoWriter(filename,
                            //CV_FOURCC('M', 'J', 'P', 'G')
                            //CV_FOURCC('I','4','2','0')
                            FOURCC_MACRO('D','I','V','X')
                            , fps, size, isColor));

}

VideoCanvas::~VideoCanvas()
{

}

void VideoCanvas::push(int times)
{

  if (times < 1)
    return;

  for (int i = 0; i < times; i++)
    {
      //if (!writer.empty())
      *writer << image;
    }

}

}

}

