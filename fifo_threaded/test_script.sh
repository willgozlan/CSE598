#!/bin/bash
./client_fifo 100 10 & #300 10 &
./client_fifo 200 10 &
./client_fifo 300 50 & #500 50 &
./client_fifo 300 98 & #500 98 &
./client_fifo 3 99 p &
exec bash
