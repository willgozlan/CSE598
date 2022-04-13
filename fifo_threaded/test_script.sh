#!/bin/bash
./client_fifo 300 10 &
./client_fifo 200 10 &
./client_fifo 500 50 &
./client_fifo 500 98 &
./client_fifo 3 99 p &
exec bash
