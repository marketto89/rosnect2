/*
 * This file is part of the OpenKinect Project. http://www.openkinect.org
 *
 * Copyright (c) 2011 individual OpenKinect contributors. See the CONTRIB file
 * for details.
 *
 * This code is licensed to you under the terms of the Apache License, version
 * 2.0, or, at your option, the terms of the GNU General Public License,
 * version 2.0. See the APACHE20 and GPL2 files for the text of the licenses,
 * or the following URLs:
 * http://www.apache.org/licenses/LICENSE-2.0
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * If you redistribute this file in source form, modified or unmodified, you
 * may:
 *   1) Leave this header intact and distribute it under the same terms,
 *      accompanying it with the APACHE20 and GPL20 files, or
 *   2) Delete the Apache 2.0 clause and accompany it with the GPL2 file, or
 *   3) Delete the GPL v2 clause and accompany it with the APACHE20 file
 * In all cases you must keep the copyright notice intact and include a copy
 * of the CONTRIB file.
 *
 * Binary distributions must follow the binary distribution requirements of
 * either License.
 */


#include <iostream>
#include <signal.h>


#include <ros/ros.h>

#include <opencv2/opencv.hpp>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>

#include <pcl_ros/point_cloud.h>

#include <pcl/visualization/pcl_visualizer.h>

bool protonect_shutdown = false;

libfreenect2::FrameMap frames;


libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);

libfreenect2::Freenect2 freenect2;
libfreenect2::Freenect2Device *dev = freenect2.openDefaultDevice();

void sigint_handler(int s)
{
    protonect_shutdown = true;
}

void my_handler(int s){
    protonect_shutdown = true;
    printf("Caught signal %d\n",s);
    listener.release(frames);
    dev->stop();
    dev->close();
    ros::shutdown();
    exit(1);


}

int main(int argc, char *argv[])
{
    ros::init(argc,argv,"rosnect2");

    if(dev == 0)
    {
        std::cout << "no device connected or failure opening the default one!" << std::endl;
        return -1;
    }

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    //signal(SIGINT,sigint_handler);
    protonect_shutdown = false;


    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);
    dev->start();

    std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
    std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;

    ros::NodeHandle nh;
    ros::Publisher cloud_pub = nh.advertise<pcl::PointCloud<pcl::PointXYZRGB> >
            ("/points",1);

    pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud;
    cv::Mat color, irMat, depthMat, ir, depth;

//    pcl::visualization::PCLVisualizer viewer;
    while(!protonect_shutdown)
    {
        listener.waitForNewFrame(frames);
//        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
//        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

//        color = cv::Mat(1080, 1920, CV_8UC4, (reinterpret_cast<unsigned char **>(rgb>data))[0]);
//        irMat = cv::Mat(ir->height, ir->width, CV_32FC1, ir->data);
//        depthMat = cv::Mat(depth->height, depth->width, CV_32FC1, depth->data);

//        unsigned char **pprgba = reinterpret_cast<unsigned char **>(rgb->data);
//        cv::Mat rgba(1080, 1920, CV_8UC4, pprgba[0]);
//        cv::Mat bgra(1080, 1920, CV_8UC4);
//        cv::cvtColor(rgba, bgra, cv::COLOR_RGBA2BGRA);
//        cv::imshow("rgb", bgra);
//        cv::imshow("ir", cv::Mat(ir->height, ir->width, CV_32FC1, ir->data) / 20000.0f);
//        cv::imshow("depth", cv::Mat(depth->height, depth->width, CV_32FC1, depth->data) / 4500.0f);

//        cv::flip(irMat, ir, 1);
//        cv::flip(depthMat, depth, 1);

//        int key = cv::waitKey(1);
//        protonect_shutdown = protonect_shutdown || (key > 0 && ((key & 0xFF) == 27)); // shutdown on escape

        cloud = (*depth->cloud).makeShared();

//        viewer.removeAllPointClouds();
//        viewer.resetCamera();
//        viewer.addPointCloud(cloud);
//        viewer.spin();

        listener.release(frames);
        //libfreenect2::this_thread::sleep_for(libfreenect2::chrono::milliseconds(100));

        cloud_pub.publish(*cloud);

        ros::spinOnce();
        //ros::Duration(0.005).sleep();
    }

    // TODO: restarting ir stream doesn't work!
    // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
    dev->stop();
    dev->close();
    ros::shutdown();

    return 0;
}
