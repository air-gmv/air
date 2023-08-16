set print pretty on
set pagination off
set confirm off
set print elements 0

target remote localhost:1234
tbreak RVS_Output
continue 
dump memory memdump.bin &rvs_all_data32_array &rvs_all_data32_array+1
kill
quit

