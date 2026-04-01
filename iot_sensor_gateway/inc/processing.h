// processing.h
#pragma once


void* worker_thread(void* arg);

int msg_parser(void* msg, void* arg);
int data_processor(void* arg, const char* sensor_name, float val_rpt);
