## Settings
- 0. get your current serial port: 
  - ` sudo dmesg | grep tty `
    - <b>physical</b> - `/dev/ttySx (in the vm)`
    - <b>usb</b>	    - `/dev/ttyACMX (on pc linux)`
   
- 1. compile the file (if needed)
  - `gcc -o <output_file_name> <input_file_name.c> (in the right directory) `
- 2. launch the program (if the previous step had succeeded)
  - sudo `./<output_file_name> -s d=/dev/ttyXXXX` <- see <b>0</b>. step
  - for help `sudo ./<output_file_name> -h` for <b>help</b>
 
