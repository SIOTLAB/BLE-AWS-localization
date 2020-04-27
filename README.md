# Trend Predictions in RSSI-based Indoor Localization using Trilateration

## Introduction 
With the drastic increase in population and services leveraging the Internet of Things (IoT), it has become an urgent problem on how to effectively monitor people and things.
Trilateration aims to address this issue by determining the location of moving objects using distances between the object and multiple stations. 
This project presents an effective way of tracking objects using trilateration in a closed space. 
We analyzed the data generated from the stations which consisted of coordinates, timestamps, and identifiers. 
After running a clustering algorithm on the data, we infer information on the behavior of the object, frequently visited places, and predict objects’s location. 
Field test results at demonstrate that the model has an average and top accuracy of 73\% and 90\%, respectively.

## System Overview
Below is a high-level system architecture diagram.
![Cloud Archiecture](https://matrixvideo.s3.amazonaws.com/BlockDiagram-1.png)

## Implementation
We use an RSSI-based system as it is cost-efficient, easy to implement, and can be used by other RSSI-based technologies such as WiFi and ZigBee. We chose ESP32, which is a low-cost, low-power, integrated WiFi and dual-mode Bluetooth solution. We use the WiFi integrated in the ESP32 to connect to the internet and transmit the data to the cloud. Each ESP32 device acts as a station and is used to receive BLE beacons and filter the scanned data. Beacons are sent by objects that are being tracked.

## Repo Organization
The following folders contain the code for the various system elements:

> * Station
> * Lambda
> * Anomaly-Detection
> * Gaussian-Mixture

The station contains the code of ESP32 which acts as station and intercept beacon's signal and trasmits it to AWS cloud.

Lambda consists of the code to calculate distance using RSSI and TX power. It also calculates the coordiantes using triliteration ans saves the coordiantes in a text file in AWS S3

Anomaly-Detection contains the Java code for Isolation Forest to pre-process the acired data from the text file. 

Gaussian-Mixture contains the code to apply clustering algorithm on the pre-processed data.

## Station Provisioning
The code for the ESP32 can be opened in the Arduino IDE and uploaded to the ESP32. A total of three stations are needed to triliterate the coordiantes of the beacon. 

Before compiling the Arduino code for the Sensor node, please make sure these libraries are installed.

## Gaussian-Mixture Provisioning
The code for Gaussian Mixture Clustering algorithm was executed in Google Colab and the pre-processed data was accessed from Google Drive. 

### Contributors
* Aarth Tandel
* Anvesh Chennupati
* Behnam Dezfouli 
