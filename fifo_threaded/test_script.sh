#!/bin/bash
./client_fifo 1000 10 &
./client_fifo 1010 10 &
./client_fifo 1020 10 &
./client_fifo 1030 10 &
./client_fifo 1040 10 &
./client_fifo 1150 10 &
exec bash
